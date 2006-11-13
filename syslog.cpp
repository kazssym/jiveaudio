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

#include <cstdio>
#include <cstdarg>

using namespace std;

#if !HAVE_SYSLOG_H
extern "C" void
syslog(int prio, const char *format, ...)
  throw ()
{
  va_list args;
  va_start(args, format);

#if defined _WIN32
  char buf[256];
  vsnprintf(buf, 256, format, args);
  OutputDebugString(buf);
  OutputDebugString("\n");
#else /* !_WIN32 */
  fprintf(stderr, format, args);
  fputs("\n", stderr);
#endif /* _WIN32 */

  va_end(args);
}
#endif /* !HAVE_SYSLOG_H */
