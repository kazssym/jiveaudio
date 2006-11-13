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

#if HAVE_CONFIG_H
#  include "config.h"
#endif

#if _WIN32
#  define STRICT 1
#  include <windows.h>
#  define STDC_HEADERS 1
#  define HAVE_FCNTL_H 1
#  define HAVE_IO_H 1
#endif

#if HAVE_UNISTD_H
#  include <unistd.h>
#endif
