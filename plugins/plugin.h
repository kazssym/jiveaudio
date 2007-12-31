/* -*- C++ -*- */
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

#ifndef pluginH
#define pluginH 1
        
#if !_WIN32
#define MOZ_X11 1
#include <X11/Intrinsic.h>
#endif

#include "player.h"

#include <memory>
#include <npapi.h>
#include <npruntime.h>

struct plugin_data;

struct NPPlugin : NPObject
  {
    plugin_data *data;
  };

struct plugin_data
  {
    bool autostart;
    bool loop;
#if _WIN32
    HWND window;
#else /* !_WIN32 */
    Widget window;
#endif /* !_WIN32 */
    std::auto_ptr <class player> player;

    static inline plugin_data *from_instance (NPP instance)
    {
      if (instance != NULL)
        {
          return static_cast <plugin_data *> (instance->pdata);
        }
      return NULL;
    }

    static inline plugin_data *from_object (NPObject *object)
    {
      NPPlugin *plugin = static_cast <NPPlugin *> (object);
      if (plugin != NULL)
        {
          return plugin->data;
        }
      return NULL;
    }
  };

#endif
