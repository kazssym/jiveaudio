/*
 * JiveAudio - multimedia plugin
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

#ifndef debugH
#define debugH 1

#if __cplusplus
#define _C_LINKAGE extern "C"
#else
#define _C_LINKAGE extern
#endif

#if HAVE_SYSLOG_H

#include <syslog.h>

#define log syslog

#else /* !HAVE_SYSLOG_H */

#define LOG_DEBUG 7

_C_LINKAGE void log (int prio, const char *format, ...);

#define syslog log

#endif /* !HAVE_SYSLOG_H */

#endif
