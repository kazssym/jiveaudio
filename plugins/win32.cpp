/*
 * win32 - Win32-specific unit for JiveAudio
 * Copyright (C) 2003-2008 Hypercore Software Design, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
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

#include "win32.h"
#if __BORLANDC__
#pragma package (smart_init)
#endif

#if _WIN32
 
#include <cstdio>
#include <dshow.h>

#ifndef PLUGINID
#define PLUGINID "@linuxfront.com/" PACKAGE_NAME ",version=" PACKAGE_VERSION
#endif

extern "C" void CALLBACK UnregisterPlugin (HWND, HINSTANCE, LPSTR, int);
extern "C" void CALLBACK RegisterPlugin (HWND, HINSTANCE, LPSTR, int);

void CALLBACK UnregisterPlugin (HWND, HINSTANCE, LPSTR, int)
{
}

void CALLBACK RegisterPlugin (HWND, HINSTANCE, LPSTR, int)
{
}

dshow_player::dshow_player () :
    graph (0)
{
}

dshow_player::~dshow_player ()
{
    stop ();
}

void
dshow_player::start ()
{
    if (graph != 0) {
	    return;
    }

    close_stream ();

    WCHAR wcname[FILENAME_MAX];
    MultiByteToWideChar (CP_ACP, MB_PRECOMPOSED, file_name, -1,
			 wcname, FILENAME_MAX);

    CoCreateInstance (CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER,
		      IID_IGraphBuilder, (void **) &graph);
    graph->RenderFile (wcname, 0);

    IMediaControl *control;
    graph->QueryInterface (IID_IMediaControl, (void **) &control);
    control->Run ();
    control->Release ();
}

void
dshow_player::stop ()
{
    if (graph == 0) {
	    return;
    }

    IMediaControl *control;
    graph->QueryInterface (IID_IMediaControl, (void **) &control);
    control->Stop ();
    control->Release ();
    graph->Release ();
    graph = 0;
}

#endif /* _WIN32 */
