/* JiveAudio - plugins for Mozilla
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

#include "common.h"

#if defined _WIN32
#  include <dshow.h>
#endif

#include <stdio.h>
#include <signal.h>

#ifdef STDC_HEADERS
#  include <string.h>
#endif

#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif

#ifdef HAVE_FCNTL_H
#  include <fcntl.h>
#else
#  define O_RDONLY 0
#  define O_WRONLY 1
#  define O_RDWR   2
#endif

struct audio_data
{
#if defined _WIN32
  IGraphBuilder *graph;
#else /* !_WIN32 */
  pid_t child;
  const char *format;
#endif /* !_WIN32 */
};

#if defined _WIN32
BOOL WINAPI
DllMain(HINSTANCE instance, DWORD reason, LPVOID)
{
  return TRUE;
}
#endif /* _WIN32 */

NPError
NPP_Initialize(void)
{
  fprintf(stderr, "libaudioplugin: NPP_Initialize\n");
#if defined _WIN32
  CoInitialize(NULL);
#endif /* _WIN32 */
  return NPERR_NO_ERROR;
}

void
NPP_Shutdown(void)
{
  fprintf(stderr, "libaudioplugin: NPP_Shutdown\n");
#if defined _WIN32
  CoUninitialize();
#endif /* _WIN32 */
}

#if defined XP_UNIX
char *
NPP_GetMIMEDescription(void)
{
  return "audio/basic:au,snd:Basic Audio;"
    "audio/x-wav:wav:WAV Audio;"
    "audio/wav::WAV Audio";
}
#endif /* XP_UNIX */

NPError
NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
  fprintf(stderr, "libaudioplugin: NPP_GetValue (%d)\n", variable);
  switch (variable)
    {
#if defined XP_UNIX
    case NPPVpluginNameString:
      *((const char **) value) = "JiveAudio Plugin";
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

NPError
NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
  fprintf(stderr, "libaudioplugin: NPP_SetValue (%d)\n", variable);
  return NPERR_NO_ERROR;
}

static void
stop(audio_data *data)
{
#if defined _WIN32
  IMediaControl *control;
  data->graph->QueryInterface(IID_IMediaControl, (void **) &control);
  control->Stop();
  control->Release();
#else /* !_WIN32 */
  if (data->child > 0) {
    kill(data->child, SIGTERM);
    waitpid(data->child, NULL, 0);
    data->child = 0;
  }
#endif /* !_WIN32 */
}

NPError
NPP_New(NPMIMEType MIMEType, NPP instance, uint16 mode,
	int16 argc, char **argn, char **argv, NPSavedData *savedData)
{
  fprintf(stderr, "libaudioplugin: NPP_New (%s, %d)\n", MIMEType, mode);
  for (int i = 0; i != argc; ++i)
    fprintf(stderr, "\t%s=%s\n", argn[i], argv[i]);

  instance->pdata = NPN_MemAlloc(sizeof (audio_data));
  if (instance->pdata == 0)
    return NPERR_OUT_OF_MEMORY_ERROR;

  audio_data *data = static_cast <audio_data *> (instance->pdata);
#if defined _WIN32
  CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
                   IID_IGraphBuilder, (void **) &data->graph);
#else /* !_WIN32 */
  data->child = 0;
#endif /* !_WIN32 */
  return NPERR_NO_ERROR;
}

NPError
NPP_Destroy(NPP instance, NPSavedData **savedData)
{
  fprintf(stderr, "libaudioplugin: NPP_Destroy\n");
  audio_data *data = static_cast <audio_data *> (instance->pdata);
  stop(data);
#if defined _WIN32
  data->graph->Release();
#endif /* _WIN32 */
  NPN_MemFree(data);
  return NPERR_NO_ERROR;
}

void
NPP_Print(NPP instance, NPPrint *print)
{
  fprintf(stderr, "libaudioplugin: NPP_Print\n");
}

NPError
NPP_SetWindow(NPP instance, NPWindow *window)
{
  fprintf(stderr, "libaudioplugin: NPP_SetWindow\n");
  return NPERR_NO_ERROR;
}

int16
NPP_HandleEvent(NPP instance, NPEvent *event)
{
  fprintf(stderr, "libaudioplugin: NPP_HandleEvent\n");
  return 0;
}

NPError
NPP_NewStream(NPP instance, NPMIMEType MIMEType, NPStream *stream,
	      NPBool seekable, uint16 *mode)
{
  fprintf(stderr, "libaudioplugin: NPP_NewStream (%s, %d)\n", MIMEType, seekable);
  audio_data *data = static_cast <audio_data *> (instance->pdata);
  stop(data);
#if !defined _WIN32
  if (strcmp(MIMEType, "audio/x-wav") == 0
      || strcmp (MIMEType, "audio/wav") == 0)
    data->format = "wav";
  else
    data->format = "ul";
#endif /* !_WIN32 */
  *mode = NP_ASFILEONLY;
  return NPERR_NO_ERROR;
}

NPError
NPP_DestroyStream(NPP instance, NPStream *stream, NPError reason)
{
  fprintf(stderr, "libaudioplugin: NPP_DestroyStream (%d)\n", reason);
  return NPERR_NO_ERROR;
}

int32
NPP_WriteReady(NPP instance, NPStream *stream)
{
  fprintf(stderr, "libaudioplugin: NPP_WriteReady\n");
  return 4096;
}

int32
NPP_Write(NPP instance, NPStream *stream, int32 off, int32 len, void *buf)
{
  fprintf(stderr, "libaudioplugin: NPP_WriteReady (%ld, %ld)\n", (long) off, (long) len);
  return len;
}

void
NPP_StreamAsFile(NPP instance, NPStream *stream, const char *name)
{
  fprintf(stderr, "libaudioplugin: NPP_StreamAsFile (%s)\n", name);
  audio_data *data = static_cast <audio_data *> (instance->pdata);
#if defined _WIN32
  WCHAR wname[FILENAME_MAX];
  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, name, -1, wname, FILENAME_MAX);
  data->graph->RenderFile(wname, NULL);

  IMediaControl *control;
  data->graph->QueryInterface(IID_IMediaControl, (void **) &control);
  control->Run();
  control->Release();
#else /* !_WIN32 */
  int fileno = open(name, O_RDONLY);
  if (fileno == -1) {
    perror(name);
    return;
  }

  data->child = fork();
  if (data->child == -1) {
    perror("fork");
    return;
  }

  if (data->child == 0) {
    int null;

    dup2(fileno, STDIN_FILENO);
    close(fileno);
    null = open("/dev/null", O_WRONLY);
    dup2(null, STDOUT_FILENO);
    close(null);
    execlp("play", "play", "-t", data->format, "-", (char *) 0);
    perror("execlp");
    _exit(1);
  }

  close(fileno);
#endif /* !_WIN32 */
}

void
NPP_URLNotify(NPP instance, const char *URL, NPReason reason, void *data)
{
  fprintf(stderr, "libaudioplugin: NPP_URLNotify\n");
}
