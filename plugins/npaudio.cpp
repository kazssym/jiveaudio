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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#if _WIN32                      /* Win32 or Win64 */
#include <windows.h>
#endif

#define _GNU_SOURCE 1
#define _REENTRANT 1

#include <new>
#include <cassert>
#include <npapi.h>
#include "debug.h"

#include "npaudio.h"
#pragma package (smart_init)

#if _WIN32

#pragma argsused
BOOL WINAPI DllMain (HINSTANCE instance, DWORD reason, LPVOID)
{
    return TRUE;
}

#endif /* _WIN32 */

/* Initializes this plugin.  */
NPError NPP_Initialize ()
{
#if _WIN32
    HRESULT result = CoInitializeEx (0, COINIT_MULTITHREADED);
    if (result != S_OK && result != S_FALSE) {
        return NPERR_GENERIC_ERROR;
    }
#endif
    return NPERR_NO_ERROR;
}

/* Finalizes this plugin.  */
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
    return "audio/basic:au,snd:&#x03bc;-law PCM Audio;"
        "audio/x-wav:wav:RIFF Waveform Audio;"
        "audio/wav::RIFF Waveform Audio (Misuse)";
}

#endif /* XP_UNIX */
           
/* Returns the value of a plugin variable.  */
#pragma argsused
NPError NPP_GetValue (NPP instance, NPPVariable variable, void *value)
{
    switch (variable) {
#ifdef XP_UNIX
    case NPPVpluginNameString:
        *((const char **) value) = PACKAGE_NAME;
        break;
    case NPPVpluginDescriptionString:
        *((const char **) value) = PACKAGE_STRING " for Gecko";
        break;
#endif
    default:
        syslog (LOG_DEBUG, "NPP_GetValue %d", variable);
        break;
    }

    return NPERR_NO_ERROR;
}

/* Sets the value of a browser variable.  */
#pragma argsused
NPError NPP_SetValue (NPP instance, NPNVariable variable, void *value)
{
    syslog (LOG_DEBUG, "NPP_SetValue %d", variable);
    return NPERR_NO_ERROR;
}
  
#pragma argsused
NPError NPP_New (NPMIMEType mime_type, NPP instance, uint16 mode,
	             int16 argc, char **argn, char **argv,
                 NPSavedData *savedData)
{
    syslog (LOG_DEBUG, "NPP_New \"%s\" %d", mime_type, mode);

    instance->pdata = NPN_MemAlloc (sizeof (plugin_data));
    if (instance->pdata == 0)
        return NPERR_OUT_OF_MEMORY_ERROR;

    plugin_data *data = new (instance->pdata) plugin_data;
    data->loop = false;
    data->autostart = (mode == NP_FULL);
    data->window = 0;
    data->player = 0;

    for (int i = 0; i != argc; ++i) {
        if (strcmp (argn[i], "loop") == 0) {
	        int c = argv[i][0];
	        if (c == 't' || c == 'T')
	            data->loop = true;
	        else if (c == 'f' || c == 'F')
	            data->loop = false;
	    } else if (strcmp (argn[i], "autostart") == 0) {
	        int c = argv[i][0];
	        if (c == 't' || c == 'T')
	            data->autostart = true;
	        else if (c == 'f' || c == 'F')
	            data->autostart = false;
	    }
    }

#if _WIN32
    /* FIXME: Windowless plugins are apparently not implemented
       on POSIX systems.  */
    NPN_SetValue (instance, NPPVpluginWindowBool,
                  reinterpret_cast <void *> (false));
#endif
    return NPERR_NO_ERROR;
}

inline void stop (plugin_data *data)
{
    if (data->player == 0) {
        return;
    }

    data->player->stop ();
    delete data->player;
    data->player = 0;
}

#pragma argsused
NPError NPP_Destroy (NPP instance, NPSavedData **savedData)
{
    syslog (LOG_DEBUG, "NPP_Destroy");

    plugin_data *data = static_cast <plugin_data *> (instance->pdata);
    assert (data != NULL);

    delete data->player;
    data->~plugin_data ();

    NPN_MemFree (instance->pdata);
    return NPERR_NO_ERROR;
}
