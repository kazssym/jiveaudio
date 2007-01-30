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

#if HAVE_CONFIG_H
#include <config.h>
#endif
#define _GNU_SOURCE 1
#define _REENTRANT 1

#if _WIN32
#define STRICT 1
#include <windows.h>
#endif

#if !_WIN32
#define MOZ_X11 1
#endif
#include <npapi.h>

#include <new>
#include <cstring>

#if HAVE_SYSLOG_H
#include <syslog.h>
#else /* !HAVE_SYSLOG_H */
#define LOG_DEBUG 7
extern "C" void syslog (int prio, const char *format, ...);
#endif /* !HAVE_SYSLOG_H */

#if _WIN32
#else
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "exec_media_player.h"
#endif
#include "player_win32.h"

struct plugin_data
{
    bool loop;
    bool autostart;
#if _WIN32
    HWND window;
    WNDPROC old_proc;
#else
    Widget window;
#endif
    media_player *player;
};

#pragma argsused
void
NPP_Print (NPP instance, NPPrint *print)
{
    syslog (LOG_DEBUG, "NPP_Print");
}

#if _WIN32
inline boolean
clear (HDC dc, const RECT *rect)
{
    HBRUSH black =
        static_cast <HBRUSH> (GetStockObject (BLACK_BRUSH));
    if (black == 0)
        return false;

    return FillRect (dc, rect, black);
}

extern "C" LRESULT CALLBACK
WindowProc (HWND w, UINT message, WPARAM param1, LPARAM param2)
{
    plugin_data *data =
        reinterpret_cast <plugin_data *>
            (GetWindowLong (w, GWL_USERDATA));

    LRESULT result;
    switch (message) {
    case WM_ERASEBKGND:
        RECT bounds;
        if (!GetClientRect (w, &bounds))
            result = false;
        else
            result = clear (reinterpret_cast <HDC> (param1), &bounds);
        break;
    case WM_PAINT:
        result = DefWindowProc (w, message, param1, param2);
        break;
    default:
        result = CallWindowProc (data->old_proc,
                                 w, message, param1, param2);
        break;
    }
    return result;
}
#endif /* _WIN32 */

NPError
NPP_SetWindow (NPP instance, NPWindow *window)
{
    syslog (LOG_DEBUG, "NPP_SetWindow");

#if !_WIN32
    plugin_data *data = static_cast <plugin_data *> (instance->pdata);

#if _WIN32
    HWND w = static_cast <HWND> (window->window);
#else
    Display *display =
        static_cast <NPSetWindowCallbackStruct *> (window->ws_info)->display;
    Widget w =
        XtWindowToWidget(display, reinterpret_cast <Window> (window->window));
#endif
    if (data->window != w) {
        data->window = w;

#if _WIN32
        SetWindowLong (data->window, GWL_USERDATA,
                       reinterpret_cast <LONG> (data));
        LONG old = SetWindowLong (data->window, GWL_WNDPROC,
                                  reinterpret_cast <LONG> (&WindowProc));
        data->old_proc = reinterpret_cast <WNDPROC> (old);
        InvalidateRect (data->window, 0, true);
#else
        Arg args[1];
        XtSetArg (args[0], XtNbackground,
	          BlackPixelOfScreen(XtScreen(data->window)));
        XtSetValues (data->window, args, 1);
#endif

        if (data->player != 0)
            data->player->set_window (data->window);
    }
#endif /* !WINDOWLESS */
    return NPERR_NO_ERROR;
}

#pragma argsused
int16
NPP_HandleEvent (NPP instance, void *event_data)
{
    syslog (LOG_DEBUG, "NPP_HandleEvent");

    NPEvent *event = static_cast <NPEvent *> (event_data);
#ifdef XP_WIN
    switch (event->event) {
    case WM_PAINT:
        syslog (LOG_DEBUG, "  event = WM_PAINT");
        return clear (reinterpret_cast <HDC> (event->wParam),
                      reinterpret_cast <LPRECT> (event->lParam));
    default:
        syslog (LOG_DEBUG, "  event = %x", event->event);
        break;
    }
#endif
#ifdef XP_UNIX
#endif
    return false;
}

#pragma argsused
NPError
NPP_NewStream (NPP instance, NPMIMEType mime_type, NPStream *stream,
	       NPBool seekable, uint16 *mode)
{
    using std::bad_alloc;

    syslog (LOG_DEBUG, "NPP_NewStream \"%s\" %d",
            mime_type, seekable);

    plugin_data *data = static_cast <plugin_data *> (instance->pdata);

    //stop (data);
    try {
#if _WIN32
        data->player = new dshow_player ();
#else
        if (strstr (mime_type, "mid") != 0)
            data->player = new external_player ("timidity");
        else
            data->player = new sox_media_player ("play");
#endif
    } catch (const bad_alloc &e) {
        return NPERR_OUT_OF_MEMORY_ERROR;
    }

    data->player->set_loop (data->loop);
    if (data->window != 0)
        data->player->set_window (data->window);

    if (!data->player->open_stream (mime_type))
        return NPERR_GENERIC_ERROR;
    return NPERR_NO_ERROR;
}

#pragma argsused
NPError
NPP_DestroyStream (NPP instance, NPStream *stream, NPReason reason)
{
    syslog (LOG_DEBUG, "NPP_DestroyStream %d", reason);

    plugin_data *data = static_cast <plugin_data *> (instance->pdata);

    //data->player->close_stream ();
    if (reason == NPRES_DONE && data->autostart)
        data->player->start ();
    return NPERR_NO_ERROR;
}

int32
NPP_WriteReady (NPP instance, NPStream *stream)
{
    plugin_data *data = static_cast <plugin_data *> (instance->pdata);

    return data->player->stream_buffer_size ();
}

int32
NPP_Write (NPP instance, NPStream *stream,
           int32 off, int32 len, void *buf)
{
    plugin_data *data = static_cast <plugin_data *> (instance->pdata);

    size_t k = data->player->write_stream (buf, len);
    if (k == (size_t) -1)
        return -1;
    return k;
}

void
NPP_StreamAsFile (NPP instance, NPStream *stream, const char *name)
{
}

void
NPP_URLNotify (NPP instance, const char *URL, NPReason reason,
               void *data)
{
    syslog (LOG_DEBUG, "NPP_URLNotify");
}
