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

#include "debug.h"
#pragma package (smart_init)

#include <stdarg.h>
#include <stdio.h>

#if !HAVE_SYSLOG_H

#pragma argsused
void syslog (int prio, const char *format,...)
{
    va_list args;
#if _WIN32
    char buf[512];
#endif

    va_start (args, format);

#if _WIN32
    vsnprintf (buf, 512, format, args);
    OutputDebugString (buf);
#else /* !_WIN32 */
    vfprintf (stderr, format, args);
    fputs ("\n", stderr);
#endif /* !_WIN32 */

    va_end (args);
}

#endif /* !HAVE_SYSLOG_H */