/* -*-C++-*- */
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

#ifndef npaudioH
#define npaudioH 1
        
#if !_WIN32
#define MOZ_X11 1
#include <X11/Intrinsic.h>
#endif

#include "player.h"

#include <memory>
#include <npruntime.h>

struct plugin_data
{
    NPClass runtime_class;
    bool autostart;
    bool loop;
#if _WIN32
    HWND window;
#else /* !_WIN32 */
    Widget window;
#endif /* !_WIN32 */
    std::auto_ptr <class player> player;

    static plugin_data *from_instance (NPP instance)
    {
        if (instance != NULL) {
            return static_cast <plugin_data *> (instance->pdata);
        }
        return NULL;
    }
    
    static plugin_data *from_object (NPObject *object)
    {
        if (object != NULL) {
            return reinterpret_cast <plugin_data *> (object->_class);
        }
        return NULL;
    }
};

#endif
