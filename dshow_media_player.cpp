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

#include "dshow_media_player.h"

using namespace std;

#if defined _WIN32

dshow_media_player::dshow_media_player():
  graph(0)
{
}

dshow_media_player::~dshow_media_player()
{
  stop();
}

void
dshow_media_player::start()
{
  if (graph != 0)
    return;

  WCHAR wname[FILENAME_MAX];
  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, file_name, -1, wname, FILENAME_MAX);

  CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER,
                   IID_IGraphBuilder, (void **) &graph);
  graph->RenderFile(wname, 0);

  IMediaControl *control;
  graph->QueryInterface(IID_IMediaControl, (void **) &control);
  control->Run();
  control->Release();
}

void
dshow_media_player::stop()
{
  if (graph == 0)
    return;

  IMediaControl *control;
  graph->QueryInterface(IID_IMediaControl, (void **) &control);
  control->Stop();
  control->Release();
  graph->Release();
  graph = 0;
}

#endif /* _WIN32 */
