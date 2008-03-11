/*
 * jive - main unit for JiveAudio
 * Copyright (C) 2003-2008 Hypercore Software Design, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#if _WIN32                      /* Win32 or Win64 */
#include <windows.h>
#endif

#include "audio.h"
#if __BORLANDC__
#pragma package (smart_init)
#endif

#include "debug.h"
#include "player_posix.h"
#include "win32.h"

#include <new>
#include <cstring>
#include <cctype>
#include <cassert>

struct plugin_method
{
  static NPObject *allocate (NPP, NPClass *)
  {
    return new (std::nothrow) NPPlugin;
  }

  static void deallocate (NPObject *object)
  {
    delete object;
  }

  static void invalidate (NPObject *)
  {
    log (LOG_DEBUG, "invalidate: Unimplemented function");
  }

  static bool hasMethod (NPObject *, NPIdentifier ident)
  {
    NPUTF8 *name = NPN_UTF8FromIdentifier (ident);
    log (LOG_DEBUG, "hasMethod: Unknown method '%s'", name);
    NPN_MemFree (name);
    return false;
  }

  static bool invoke (NPObject *, NPIdentifier, const NPVariant *, uint32_t, NPVariant *)
  {
    log (LOG_DEBUG, "invoke: Unimplemented function");
    return false;
  }

  static bool invokeDefault (NPObject *, const NPVariant *, uint32_t, NPVariant *)
  {
    log (LOG_DEBUG, "invokeDefault: Unimplemented function");
    return false;
  }

  static bool hasProperty (NPObject *, NPIdentifier ident)
  {
    NPUTF8 *name = NPN_UTF8FromIdentifier (ident);
    log (LOG_DEBUG, "hasProperty: Unknown property '%s'", name);
    NPN_MemFree (name);
    return false;
  }

  static bool getProperty (NPObject *, NPIdentifier, NPVariant *)
  {
    log (LOG_DEBUG, "getProperty: Unimplemented function");
    return false;
  }

  static bool setProperty (NPObject *, NPIdentifier, const NPVariant *)
  {
    log (LOG_DEBUG, "setProperty: Unimplemented function");
    return false;
  }

  static bool removeProperty (NPObject *, NPIdentifier)
  {
    log (LOG_DEBUG, "removeProperty: Unimplemented function");
    return false;
  }

  static bool enumerate (NPObject *, NPIdentifier **, uint32_t *)
  {
    log (LOG_DEBUG, "enumerate: Unimplemented function");
    return false;
  }

  static bool construct (NPObject *, const NPVariant *, uint32_t, NPVariant *)
  {
    log (LOG_DEBUG, "construct: Unimplemented function");
    return false;
  }
};

static NPClass plugin_class =
  {
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
    &plugin_method::construct,
  };

/**
 * Initializes this plugin.
 */
NPError NPP_Initialize ()
{
#if _WIN32
  HRESULT result = CoInitializeEx (NULL, COINIT_MULTITHREADED);
  if (result != S_OK && result != S_FALSE)
    {
      return NPERR_MODULE_LOAD_FAILED_ERROR;
    }
#endif

  return NPERR_NO_ERROR;
}

/**
 * Finalizes this plugin.
 */
void NPP_Shutdown ()
{
#if _WIN32
  CoUninitialize ();
#endif
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

        NPPlugin *object =
          static_cast <NPPlugin *> (NPN_CreateObject (instance, &plugin_class));
        object->data = data;
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
NPError NPP_SetValue (NPP, NPNVariable var, void *)
{
  log (LOG_DEBUG, "NPP_SetValue: Unimplemented variable %#x", var);
  return NPERR_NO_ERROR;
}

NPError NPP_New (NPMIMEType, NPP instance, uint16 mode,
                 int16 argc, char **argn, char **argv,
                 NPSavedData *)
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

  data->autostart = (mode == NP_FULL);
  data->loop = false;
  data->window = 0;

  for (int i = 0; i != argc; ++i)
    {
      if (strcmp (argn[i], "type") == 0 ||
          strcmp (argn[i], "src") == 0 ||
          strcmp (argn[i], "name") == 0 ||
          strcmp (argn[i], "height") == 0 ||
          strcmp (argn[i], "width") == 0)
        {
          log (LOG_DEBUG, "NPP_New: Ignored common parameter '%s'", argn[i]);
        }
      else if (strcmp (argn[i], "autostart") == 0)
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
  NPN_SetValue (instance, NPPVpluginWindowBool, NULL);
#endif

  instance->pdata = data;
  return NPERR_NO_ERROR;
}

NPError NPP_Destroy (NPP instance, NPSavedData **)
{
  plugin_data *data = plugin_data::from_instance (instance);
  delete data;

  return NPERR_NO_ERROR;
}

void NPP_Print (NPP, NPPrint *)
{
  log (LOG_DEBUG, "NPP_Print: Unimplemented function");
}

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

int16 NPP_HandleEvent (NPP, void *event_data)
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

NPError NPP_NewStream (NPP instance, NPMIMEType mime_type, NPStream *,
	                     NPBool, uint16 *)
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

NPError NPP_DestroyStream (NPP instance, NPStream *, NPReason reason)
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

int32 NPP_WriteReady (NPP instance, NPStream *)
{
  plugin_data *data = plugin_data::from_instance (instance);
  assert (data != NULL);

  return data->player->stream_buffer_size ();
}

int32 NPP_Write (NPP instance, NPStream *,
                 int32, int32 size, void *buf)
{
  plugin_data *data = plugin_data::from_instance (instance);
  assert (data != NULL);

  long n = data->player->write_stream (buf, size);
  return n;
}

void NPP_StreamAsFile (NPP, NPStream *, const char *)
{
}

void NPP_URLNotify (NPP, const char *, NPReason,
                    void *)
{
  log (LOG_DEBUG, "NPP_URLNotify: Unimplemented function");
}
