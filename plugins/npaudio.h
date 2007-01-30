/*
 * JiveAudio - multimedia plugin for Mozilla -*-C++-*-
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

#ifndef npaudioH
#define npaudioH 1
        
#if !_WIN32
#define MOZ_X11 1
#endif

#include "player.h"

struct plugin_data
{
    bool loop;
    bool autostart;
#if _WIN32
    HWND window;
    WNDPROC old_proc;
#else /* !_WIN32 */
#endif /* !_WIN32 */
    class player *player;
};

#endif
