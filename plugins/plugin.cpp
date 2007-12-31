/*
 * JiveAudio - multimedia plugin for Mozilla
 * Copyright (C) 2003-2006 Hypercore Software Design, Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#if _WIN32                      /* Win32 or Win64 */
#include <windows.h>
#endif

#define _GNU_SOURCE 1
#define _REENTRANT 1
 
#include "plugin.h"
#pragma package (smart_init)

#include "debug.h"
#include "player_posix.h"
#include "player_win32.h"

#include <new>
#include <cstring>
#include <cctype>
#include <cassert>

#if _WIN32

#pragma argsused
BOOL WINAPI DllMain (HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  switch (reason)
    {
    case DLL_THREAD_ATTACH:
      CoInitializeEx (NULL, COINIT_MULTITHREADED);
      break;
    case DLL_THREAD_DETACH:
      CoUninitialize ();
      break;
    default:
      break;
    }

  return TRUE;
}

#endif /* _WIN32 */

/* Initializes this plugin.  */
NPError NPP_Initialize ()
{
  return NPERR_NO_ERROR;
}

/* Finalizes this plugin.  */
void NPP_Shutdown ()
{
}

#ifdef XP_UNIX

/* Returns the MIME description.  */
char *NPP_GetMIMEDescription ()
{
  return ("audio/basic::G.711 &#x03bc;-law Audio;"
          "audio/x-wav:wav:RIFF Waveform Audio;"
          "audio/wav::RIFF Waveform Audio - Misuse");
}

#endif /* XP_UNIX */

/* Returns the value of a plugin variable.  */
#pragma argsused
NPError NPP_GetValue (NPP instance, NPPVariable var, void *value)
{
  plugin_data *data = plugin_data::from_instance (instance);

  switch (var)
    {
#ifdef XP_UNIX
    case NPPVpluginNameString:
      *static_cast <const char **> (value) = PACKAGE_NAME;
      break;
    case NPPVpluginDescriptionString:
      *static_cast <const char **> (value) = PACKAGE_STRING;
      break;
#endif
    case NPPVpluginScriptableNPObject:
      {
        assert (data != NULL);

        NPObject *object = NPN_CreateObject (instance, &data->runtime_class);
        *static_cast <NPObject **> (value) = object;
      }
      break;
    default:
      log (LOG_DEBUG, "NPP_GetValue: Unimplemented variable %#x", var);
      break;
    }

  return NPERR_NO_ERROR;
}

/* Sets the value of a browser variable.  */
#pragma argsused
NPError NPP_SetValue (NPP instance, NPNVariable var, void *value)
{
  log (LOG_DEBUG, "NPP_SetValue: Unimplemented variable %#x", var);
  return NPERR_NO_ERROR;
}

namespace plugin_method
{
  NPObject *allocate (NPP, NPClass *)
  {
    try
      {
        return new NPObject ();
      }
    catch (const std::bad_alloc &e)
      {
        return NULL;
      }
  }

  void deallocate (NPObject *object)
  {
    delete object;
  }

  void invalidate (NPObject *)
  {
    log (LOG_DEBUG, "invalidate: Unimplemented function");
  }

  bool hasMethod (NPObject *, NPIdentifier ident)
  {
    NPUTF8 *name = NPN_UTF8FromIdentifier (ident);
    log (LOG_DEBUG, "hasMethod: Unknown method '%s'", name);
    NPN_MemFree (name);
    return false;
  }

  bool invoke (NPObject *, NPIdentifier, const NPVariant *, uint32_t, NPVariant *)
  {
    log (LOG_DEBUG, "invoke: Unimplemented function");
    return false;
  }

  bool invokeDefault (NPObject *, const NPVariant *, uint32_t, NPVariant *)
  {
    log (LOG_DEBUG, "invokeDefault: Unimplemented function");
    return false;
  }

  bool hasProperty (NPObject *, NPIdentifier ident)
  {
    NPUTF8 *name = NPN_UTF8FromIdentifier (ident);
    log (LOG_DEBUG, "hasProperty: Unknown property '%s'", name);
    NPN_MemFree (name);
    return false;
  }

  bool getProperty (NPObject *, NPIdentifier, NPVariant *)
  {
    log (LOG_DEBUG, "getProperty: Unimplemented function");
    return false;
  }

  bool setProperty (NPObject *, NPIdentifier, const NPVariant *)
  {
    log (LOG_DEBUG, "setProperty: Unimplemented function");
    return false;
  }

  bool removeProperty (NPObject *, NPIdentifier)
  {
    log (LOG_DEBUG, "removeProperty: Unimplemented function");
    return false;
  }

  bool enumerate (NPObject *, NPIdentifier **, uint32_t *)
  {
    log (LOG_DEBUG, "enumerate: Unimplemented function");
    return false;
  }
}

const NPClass plugin_class = {
  NP_CLASS_STRUCT_VERSION,
  &plugin_method::allocate,
  &plugin_method::deallocate,
  &plugin_method::invalidate,
  &plugin_method::hasMethod,
  &plugin_method::invoke,
  &plugin_method::invokeDefault,
  &plugin_method::hasProperty,
  &plugin_method::getProperty,
  &plugin_method::setProperty,
  &plugin_method::removeProperty,
  &plugin_method::enumerate,
};

#pragma argsused
NPError NPP_New (NPMIMEType mime_type, NPP instance, uint16 mode,
	               int16 argc, char **argn, char **argv,
                 NPSavedData *savedData)
{
  using std::strcmp;
  using std::tolower;

  plugin_data *data;
  try
    {
      data = new plugin_data ();
    }
  catch (const std::bad_alloc &e)
    {
      return NPERR_OUT_OF_MEMORY_ERROR;
    }

  data->runtime_class = plugin_class;
  data->autostart = (mode == NP_FULL);
  data->loop = false;
  data->window = 0;

  for (int i = 0; i != argc; ++i)
    {
      if (strcmp (argn[i], "autostart") == 0)
        {
          int c = tolower (argv[i][0]);
          data->autostart = (c == 't');
        }
      else if (strcmp (argn[i], "loop") == 0)
        {
          int c = tolower (argv[i][0]);
          data->loop = (c == 't');
        }
      else
        {
          log (LOG_DEBUG, "NPP_New: Unknown parameter '%s'", argn[i]);
        }
    }

#if _WIN32
  /* FIXME: Windowless plugins are apparently not implemented
     on POSIX systems.  */
  NPN_SetValue (instance, NPPVpluginWindowBool,
                reinterpret_cast <void *> (false));
#endif

  instance->pdata = data;
  return NPERR_NO_ERROR;
}

#pragma argsused
NPError NPP_Destroy (NPP instance, NPSavedData **savedData)
{
  plugin_data *data = plugin_data::from_instance (instance);
  delete data;

  return NPERR_NO_ERROR;
}

#pragma argsused
void NPP_Print (NPP instance, NPPrint *print)
{
  log (LOG_DEBUG, "NPP_Print: Unimplemented function");
}

#pragma argsused
NPError NPP_SetWindow (NPP instance, NPWindow *window)
{
  plugin_data *data = plugin_data::from_instance (instance);
  assert (data != NULL);

#if _WIN32
  if (data->window != (HWND) window->window)
    {
      data->window = (HWND) window->window;
    }
#else /* !_WIN32 */
#if 0
  Display *display =
    static_cast <NPSetWindowCallbackStruct *> (window->ws_info)->display;
  Widget w =
    XtWindowToWidget (display, reinterpret_cast <Window> (window->window));
  if (data->window != w)
    {
      data->window = w;

      Arg args[1];
      XtSetArg (args[0], XtNbackground,
                BlackPixelOfScreen (XtScreen (data->window)));
      XtSetValues (data->window, args, 1);
    }
#endif
  log (LOG_DEBUG, "NPP_SetWindow: Unimplemented function");
#endif /* !_WIN32 */

  return NPERR_NO_ERROR;
}

#if _WIN32

inline bool clear (HDC dc, const RECT *rect)
{
  HBRUSH black = (HBRUSH) GetStockObject (BLACK_BRUSH);
  if (black != 0)
    {
      return FillRect (dc, rect, black);
    }
  return false;
}

#endif /* _WIN32 */

#pragma argsused
int16 NPP_HandleEvent (NPP instance, void *event_data)
{
  NPEvent *event = static_cast <NPEvent *> (event_data);
  assert (event != NULL);

#if _WIN32
  switch (event->event)
    {
    case WM_PAINT:
      return clear ((HDC) event->wParam, (LPRECT) event->lParam);
    default:
      log (LOG_DEBUG, "NPP_HandleEvent: Unknown event %#x", event->event);
      break;
    }
#else /* !_WIN32 */
  log (LOG_DEBUG, "NPP_HandleEvent: Unimplemented function");
#endif /* !_WIN32 */
  return false;
}

#pragma argsused
NPError NPP_NewStream (NPP instance, NPMIMEType mime_type, NPStream *stream,
	                   NPBool seekable, uint16 *mode)
{
  plugin_data *data = plugin_data::from_instance (instance);
  assert (data != NULL);

  try
    {
#if _WIN32
      data->player.reset (new dshow_player ());
#else
      if (strstr (mime_type, "mid") != 0)
        {
          data->player.reset (new external_player ("timidity"));
        }
      else
        {
          data->player.reset (new sox_media_player ("play"));
        }
#endif
    }
  catch (const std::bad_alloc &e)
    {
      return NPERR_OUT_OF_MEMORY_ERROR;
    }

  data->player->set_loop (data->loop);
  data->player->set_window (data->window);

  if (!data->player->open_stream (mime_type))
    {
      return NPERR_GENERIC_ERROR;
    }

  return NPERR_NO_ERROR;
}

#pragma argsused
NPError NPP_DestroyStream (NPP instance, NPStream *stream, NPReason reason)
{
  plugin_data *data = plugin_data::from_instance (instance);
  assert (data != NULL);

  //data->player->close_stream ();
  if (reason == NPRES_DONE && data->autostart)
    {
      data->player->start ();
    }

  return NPERR_NO_ERROR;
}

#pragma argsused
int32 NPP_WriteReady (NPP instance, NPStream *stream)
{
  plugin_data *data = plugin_data::from_instance (instance);
  assert (data != NULL);

  return data->player->stream_buffer_size ();
}

#pragma argsused
int32 NPP_Write (NPP instance, NPStream *stream,
                 int32 off, int32 size, void *buf)
{
  plugin_data *data = plugin_data::from_instance (instance);
  assert (data != NULL);

  long n = data->player->write_stream (buf, size);
  return n;
}

#pragma argsused
void NPP_StreamAsFile (NPP instance, NPStream *stream, const char *name)
{
}

#pragma argsused
void NPP_URLNotify (NPP instance, const char *URL, NPReason reason,
                    void *data)
{
  log (LOG_DEBUG, "NPP_URLNotify: Unimplemented function");
}
