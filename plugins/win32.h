/* -*-c++-*-
 * win32 - Win32-specific unit for JiveAudio
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

#ifndef win32H
#define win32H 1

#include "player.h"

#if _WIN32

class IGraphBuilder;

class dshow_player : public file_media_player
{
public:
    dshow_player ();
public:
    virtual ~dshow_player ();

    void start ();
    void stop ();
protected:
    IGraphBuilder *graph;
};

#endif /* _WIN32 */

#endif
