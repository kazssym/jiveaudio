/* JiveAudio - multimedia plugin for Mozilla
   Copyright (C) 2003 Hypercore Software Design, Ltd.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.  */

#define _GNU_SOURCE 1
#define _REENTRANT 1

#include "common.h"

#include <npapi.h>

#include <new>
#include <cstdio>
#include <cstring>
#include <csignal>

#if defined _WIN32
#  include <dshow.h>
#else
#  include "exec_media_player.h"
#endif

using namespace std;

struct audio_data
{
  bool loop;
#if defined _WIN32
  IGraphBuilder *graph;
#else /* !_WIN32 */
  media_player *player;
#endif /* !_WIN32 */
};

#if defined _WIN32
BOOL WINAPI
DllMain(HINSTANCE instance, DWORD reason, LPVOID)
{
  return TRUE;
}
#endif /* _WIN32 */

/* Initializes this plugin.  */
NPError
NPP_Initialize()
{
#if defined _WIN32
  HRESULT result = CoInitialize(0);
  if (result != S_OK && result != S_FALSE)
    return NPERR_GENERIC_ERROR;
#endif /* _WIN32 */
  return NPERR_NO_ERROR;
}

/* Finalizes this plugin.  */
void
NPP_Shutdown()
{
#if defined _WIN32
  CoUninitialize();
#endif /* _WIN32 */
}

#if defined XP_UNIX
/* Returns the MIME description.  */
char *
NPP_GetMIMEDescription()
{
  return "audio/basic:au,snd:Basic Audio;"
    "audio/x-wav:wav:WAV Audio;"
    "audio/wav::WAV Audio;"
    "audio/x-midi:mid,midi:MIDI Sequence;"
    "audio/midi::MIDI Sequence";
}
#endif /* XP_UNIX */

/* Returns the value of a plugin variable.  */
NPError
NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
  if (instance != 0)
    fprintf(stderr, "%s: NPP_GetValue (%d)\n", __FILE__, variable);
  switch (variable)
    {
#if defined XP_UNIX
    case NPPVpluginNameString:
      *((const char **) value) = PACKAGE_NAME " Plugin";
      break;

    case NPPVpluginDescriptionString:
      *((const char **) value) = PACKAGE_STRING;
      break;
#endif /* XP_UNIX */

    default:
      break;
    }

  return NPERR_NO_ERROR;
}

/* Sets the value of a browser variable.  */
NPError
NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
  fprintf(stderr, "%s: NPP_SetValue (%d)\n", __FILE__, variable);
  return NPERR_NO_ERROR;
}

static void
stop(audio_data *data)
{
#if defined _WIN32
  if (data->graph != 0)
    {
      IMediaControl *control;
      data->graph->QueryInterface(IID_IMediaControl, (void **) &control);
      control->Stop();
      control->Release();
      data->graph->Release();
      data->graph = 0;
    }
#else /* !_WIN32 */
  if (data->player != 0)
    {
      data->player->stop();
      delete data->player;
      data->player = 0;
    }
#endif /* !_WIN32 */
}

NPError
NPP_New(NPMIMEType mime_type, NPP instance, uint16 mode,
	int16 argc, char **argn, char **argv, NPSavedData *savedData)
{
  fprintf(stderr, "%s: NPP_New (\"%s\", %d)\n", __FILE__, mime_type, mode);
  instance->pdata = NPN_MemAlloc(sizeof (audio_data));
  if (instance->pdata == 0)
    return NPERR_OUT_OF_MEMORY_ERROR;

  audio_data *data = static_cast <audio_data *> (instance->pdata);
  data->loop = false;
  for (int i = 0; i != argc; ++i)
    {
      fprintf(stderr, "%s: %s=\"%s\"\n", __FILE__, argn[i], argv[i]);
      if (strcmp(argn[i], "loop") == 0)
	data->loop = argv[i][0] != 'f';
    }
#if defined _WIN32
  data->graph = 0;
#else /* !_WIN32 */
  data->player = 0;
#endif /* !_WIN32 */

  /* FIXME: Windowless plugins are apparently not implemented in
     Mozilla 1.0 on POSIX systems.  */
  NPN_SetValue(instance, NPPVpluginWindowBool, (void *) false);
  return NPERR_NO_ERROR;
}

NPError
NPP_Destroy(NPP instance, NPSavedData **savedData)
{
  fprintf(stderr, "%s: NPP_Destroy\n", __FILE__);

  audio_data *data = static_cast <audio_data *> (instance->pdata);
  stop(data);
  NPN_MemFree(data);
  return NPERR_NO_ERROR;
}

void
NPP_Print(NPP instance, NPPrint *print)
{
  fprintf(stderr, "%s: NPP_Print\n", __FILE__);
}

NPError
NPP_SetWindow(NPP instance, NPWindow *window)
{
  fprintf(stderr, "%s: NPP_SetWindow\n", __FILE__);
  return NPERR_NO_ERROR;
}

int16
NPP_HandleEvent(NPP instance, NPEvent *event)
{
  fprintf(stderr, "%s: NPP_HandleEvent\n", __FILE__);
  return 0;
}

NPError
NPP_NewStream(NPP instance, NPMIMEType mime_type, NPStream *stream,
	      NPBool seekable, uint16 *mode)
{
  fprintf(stderr, "%s: NPP_NewStream (\"%s\", %d)\n", __FILE__, mime_type, seekable);

  audio_data *data = static_cast <audio_data *> (instance->pdata);
  stop(data);
#if defined _WIN32
  *mode = NP_ASFILEONLY;
#else /* !_WIN32 */
  if (strstr(mime_type, "mid") != 0)
    data->player = new (nothrow) exec_media_player("timidity");
  else
    data->player = new (nothrow) sox_media_player("play");
  if (data->player == 0)
    return NPERR_OUT_OF_MEMORY_ERROR;

  data->player->set_loop(data->loop);
  data->player->open_stream(mime_type);
#endif /* !_WIN32 */
  return NPERR_NO_ERROR;
}

NPError
NPP_DestroyStream(NPP instance, NPStream *stream, NPReason reason)
{
  fprintf(stderr, "%s: NPP_DestroyStream (%d)\n", __FILE__, reason);

  audio_data *data = static_cast <audio_data *> (instance->pdata);
#if defined _WIN32
  if (reason != NPRES_DONE)
    stop(data);
#else /* !_WIN32 */
  data->player->close_stream();
  if (reason == NPRES_DONE)
    data->player->play();
#endif /* !_WIN32 */
  return NPERR_NO_ERROR;
}

int32
NPP_WriteReady(NPP instance, NPStream *stream)
{
  audio_data *data = static_cast <audio_data *> (instance->pdata);
#if defined _WIN32
  return 4096;
#else /* !_WIN32 */
  return data->player->stream_buffer_size();
#endif /* !_WIN32 */
}

int32
NPP_Write(NPP instance, NPStream *stream, int32 off, int32 len, void *buf)
{
  audio_data *data = static_cast <audio_data *> (instance->pdata);
#if defined _WIN32
  return len;
#else /* !_WIN32 */
  return data->player->write_stream(buf, len);
#endif /* !_WIN32 */
}

void
NPP_StreamAsFile(NPP instance, NPStream *stream, const char *name)
{
  fprintf(stderr, "%s: NPP_StreamAsFile (\"%s\")\n", __FILE__, name);

  audio_data *data = static_cast <audio_data *> (instance->pdata);
#if defined _WIN32
  WCHAR wname[FILENAME_MAX];
  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, name, -1, wname, FILENAME_MAX);

  CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER,
                   IID_IGraphBuilder, (void **) &data->graph);
  data->graph->RenderFile(wname, 0);

  IMediaControl *control;
  data->graph->QueryInterface(IID_IMediaControl, (void **) &control);
  control->Run();
  control->Release();
#endif /* _WIN32 */
}

void
NPP_URLNotify(NPP instance, const char *URL, NPReason reason, void *data)
{
  fprintf(stderr, "%s: NPP_URLNotify\n", __FILE__);
}
