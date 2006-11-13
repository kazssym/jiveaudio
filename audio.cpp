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
#include <cstring>

#if HAVE_SYSLOG_H
#  include <syslog.h>
#else /* !HAVE_SYSLOG_H */
#  define LOG_DEBUG 7
extern "C" void syslog(int prio, const char *format, ...);
#endif /* !HAVE_SYSLOG_H */

#if defined _WIN32
#  include "dshow_media_player.h"
#else
#  include <X11/Intrinsic.h>
#  include <X11/StringDefs.h>
#  include "exec_media_player.h"
#endif

using namespace std;

struct audio_data
{
  bool loop;
  bool autostart;
#if defined _WIN32
  HWND window;
  WNDPROC old_proc;
#else
  Widget window;
#endif
  media_player *player;
};

#if defined _WIN32
BOOL WINAPI
DllMain(HINSTANCE instance, DWORD reason, LPVOID)
{
  return true;
}
#endif /* defined _WIN32 */

/* Initializes this plugin.  */
NPError
NPP_Initialize()
{
#if defined _WIN32
  HRESULT result = CoInitializeEx(0, COINIT_MULTITHREADED);
  if (result != S_OK && result != S_FALSE)
    return NPERR_GENERIC_ERROR;
#endif
  return NPERR_NO_ERROR;
}

/* Finalizes this plugin.  */
void
NPP_Shutdown()
{
#if defined _WIN32
  CoUninitialize();
#endif
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
#endif /* defined XP_UNIX */

/* Returns the value of a plugin variable.  */
NPError
NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
  if (instance != 0)
    syslog(LOG_DEBUG, "%s: NPP_GetValue %d", __FILE__, variable);
  switch (variable)
    {
#if defined XP_UNIX
    case NPPVpluginNameString:
      *((const char **) value) = PACKAGE_NAME " Plugin";
      break;
    case NPPVpluginDescriptionString:
      *((const char **) value) = PACKAGE_STRING;
      break;
#endif
    default:
      break;
    }

  return NPERR_NO_ERROR;
}

/* Sets the value of a browser variable.  */
NPError
NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
  syslog(LOG_DEBUG, "%s: NPP_SetValue %d", __FILE__, variable);
  return NPERR_NO_ERROR;
}

inline void
release_window(audio_data *data)
{
  if (data->window == 0)
    return;

#if defined _WIN32
  SetWindowLong(data->window, GWL_WNDPROC,
                reinterpret_cast <LONG> (data->old_proc));
#endif
  data->window = 0;
}

inline void
stop(audio_data *data)
{
  if (data->player == 0)
    return;

  data->player->stop();
  delete data->player;
  data->player = 0;
}

NPError
NPP_New(NPMIMEType mime_type, NPP instance, uint16 mode,
	int16 argc, char **argn, char **argv, NPSavedData *savedData)
{
  syslog(LOG_DEBUG, "%s: NPP_New \"%s\" %d", __FILE__, mime_type, mode);
  instance->pdata = NPN_MemAlloc(sizeof (audio_data));
  if (instance->pdata == 0)
    return NPERR_OUT_OF_MEMORY_ERROR;

  audio_data *data = static_cast <audio_data *> (instance->pdata);
  data->loop = false;
  data->autostart = mode == NP_FULL;
  data->window = 0;
  data->player = 0;

  for (int i = 0; i != argc; ++i)
    {
      syslog(LOG_DEBUG, "  name=\"%s\" value=\"%s\"", argn[i], argv[i]);
      if (strcmp(argn[i], "loop") == 0)
	{
	  char c = argv[i][0];
	  if (c == 't' || c == 'T')
	    data->loop = true;
	  else if (c == 'f' || c == 'F')
	    data->loop = false;
	}
      else if (strcmp(argn[i], "autostart") == 0)
	{
	  char c = argv[i][0];
	  if (c == 't' || c == 'T')
	    data->autostart = true;
	  else if (c == 'f' || c == 'F')
	    data->autostart = false;
	}
    }

#ifdef WINDOWLESS
  /* FIXME: Windowless plugins are apparently not implemented in
     Mozilla 1.0 on POSIX systems.  */
  NPN_SetValue(instance, NPPVpluginWindowBool, (void *) false);
#endif
  return NPERR_NO_ERROR;
}

NPError
NPP_Destroy(NPP instance, NPSavedData **savedData)
{
  syslog(LOG_DEBUG, "%s: NPP_Destroy", __FILE__);

  audio_data *data = static_cast <audio_data *> (instance->pdata);
  stop(data);
  release_window(data);
  NPN_MemFree(data);
  return NPERR_NO_ERROR;
}

void
NPP_Print(NPP instance, NPPrint *print)
{
  syslog(LOG_DEBUG, "%s: NPP_Print", __FILE__);
}

#if defined _WIN32
inline boolean
clear(HWND w, HDC dc)
{
  RECT bounds;
  if (!GetClientRect(w, &bounds))
    return false;

  HBRUSH black = static_cast <HBRUSH> (GetStockObject(BLACK_BRUSH));
  if (black == 0)
    return false;

  return FillRect(dc, &bounds, black);
}

extern "C" LRESULT CALLBACK
WindowProc(HWND w, UINT message, WPARAM param1, LPARAM param2)
  throw ()
{
  audio_data *data =
    reinterpret_cast <audio_data *> (GetWindowLong(w, GWL_USERDATA));
  LRESULT result;
  switch (message)
    {
    case WM_ERASEBKGND:
      result = clear(w, reinterpret_cast <HDC> (param1));
      break;
    case WM_PAINT:
      result = DefWindowProc(w, message, param1, param2);
      break;
    default:
      result = CallWindowProc(data->old_proc, w, message, param1, param2);
      break;
    }
  return result;
}
#endif /* defined _WIN32 */

NPError
NPP_SetWindow(NPP instance, NPWindow *window)
{
  syslog(LOG_DEBUG, "%s: NPP_SetWindow", __FILE__);

  audio_data *data = static_cast <audio_data *> (instance->pdata);
#if defined _WIN32
#  if defined XP_WIN
  HWND w = static_cast <HWND> (window->window);
  if (data->window != w)
    {
      release_window(data);
      data->window = w;

      SetWindowLong(data->window, GWL_USERDATA,
                    reinterpret_cast <LONG> (data));
      LONG old = SetWindowLong(data->window, GWL_WNDPROC,
                               reinterpret_cast <LONG> (&WindowProc));
      data->old_proc = reinterpret_cast <WNDPROC> (old);
      InvalidateRect(data->window, 0, true);

      if (data->player != 0)
        data->player->set_window(data->window);
    }
#  endif
#else /* !defined _WIN32 */
#  if defined XP_UNIX
  Display *display =
    static_cast <NPSetWindowCallbackStruct *> (window->ws_info)->display;
  Widget w =
    XtWindowToWidget(display, reinterpret_cast <Window> (window->window));
  if (data->window != w)
    {
      release_window(data);
      data->window = w;

      Arg args[1];
      XtSetArg(args[0], XtNbackground,
	       BlackPixelOfScreen(XtScreen(data->window)));
      XtSetValues(data->window, args, 1);

      if (data->player != 0)
	data->player->set_window(data->window);
    }
#  endif
#endif /* !defined _WIN32 */
  return NPERR_NO_ERROR;
}

int16
NPP_HandleEvent(NPP instance, NPEvent *event)
{
  syslog(LOG_DEBUG, "%s: NPP_HandleEvent", __FILE__);
  return 0;
}

NPError
NPP_NewStream(NPP instance, NPMIMEType mime_type, NPStream *stream,
	      NPBool seekable, uint16 *mode)
{
  syslog(LOG_DEBUG, "%s: NPP_NewStream \"%s\" %d", __FILE__, mime_type, seekable);

  audio_data *data = static_cast <audio_data *> (instance->pdata);
  stop(data);
  try
    {
#if defined _WIN32
      data->player = new dshow_media_player();
#else
      if (strstr(mime_type, "mid") != 0)
	data->player = new exec_media_player("timidity");
      else
	data->player = new sox_media_player("play");
#endif
    }
  catch (const bad_alloc &e)
    {
      return NPERR_OUT_OF_MEMORY_ERROR;
    }

  data->player->set_loop(data->loop);
  if (data->window != 0)
    data->player->set_window(data->window);

  data->player->open_stream(mime_type);
  return NPERR_NO_ERROR;
}

NPError
NPP_DestroyStream(NPP instance, NPStream *stream, NPReason reason)
{
  syslog(LOG_DEBUG, "%s: NPP_DestroyStream %d", __FILE__, reason);

  audio_data *data = static_cast <audio_data *> (instance->pdata);
  data->player->close_stream();
  if (reason == NPRES_DONE && data->autostart)
    data->player->start();
  return NPERR_NO_ERROR;
}

int32
NPP_WriteReady(NPP instance, NPStream *stream)
{
  audio_data *data = static_cast <audio_data *> (instance->pdata);
  return data->player->stream_buffer_size();
}

int32
NPP_Write(NPP instance, NPStream *stream, int32 off, int32 len, void *buf)
{
  audio_data *data = static_cast <audio_data *> (instance->pdata);
  size_t k = data->player->write_stream(buf, len);
  if (k == (size_t) -1)
    return -1;
  return k;
}

void
NPP_StreamAsFile(NPP instance, NPStream *stream, const char *name)
{
}

void
NPP_URLNotify(NPP instance, const char *URL, NPReason reason, void *data)
{
  syslog(LOG_DEBUG, "%s: NPP_URLNotify", __FILE__);
}
