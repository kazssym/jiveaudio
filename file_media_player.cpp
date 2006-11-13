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

#include "media_player.h"

#ifdef HAVE_FCNTL_H
#  include <fcntl.h>
#else
#  define O_RDONLY 0
#  define O_WRONLY 1
#  define O_RDWR   2
#endif

#if !defined O_EXCL
#  define O_EXCL 0
#endif

#if !defined O_BINARY
#  define O_BINARY 0
#endif

using namespace std;

file_media_player::file_media_player():
  fileno(-1)
{
  tmpnam(file_name);
}

file_media_player::~file_media_player()
{
  remove(file_name);
}

bool
file_media_player::open_stream(const char *mime_type)
{
  if (fileno != -1)
    close(fileno);

  remove(file_name);
#if !defined O_CREAT
  fileno = creat(file_name, 0600);
#else
  fileno = open(file_name, O_WRONLY | O_CREAT | O_EXCL | O_BINARY, 0600);
#endif
  if (fileno == -1)
    return false;

  return true;
}

size_t
file_media_player::stream_buffer_size() const
{
  if (fileno == -1)
    return 0;

  return 4096;
}

ssize_t
file_media_player::write_stream(const void *buf, size_t nbytes)
{
  if (fileno == -1)
    return -1;

  ssize_t k = write(fileno, buf, nbytes);
  return k;
}

void
file_media_player::close_stream()
{
  if (fileno == -1)
    return;

  close(fileno);
  fileno = -1;
}

