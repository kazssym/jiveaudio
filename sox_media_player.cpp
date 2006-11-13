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

#include "exec_media_player.h"

#include <cstring>

using namespace std;

sox_media_player::sox_media_player(const char *_command_name):
  exec_media_player(_command_name)
{
}

void
sox_media_player::exec(int stream_fileno)
{
  dup2(stream_fileno, 0);
  close(stream_fileno);

  execlp(command_name, command_name, "-t", file_format,
	 "-", (const char *) 0);
  perror(command_name);
}

bool
sox_media_player::open_stream(const char *mime_type)
{
  file_format = "ul";
  if (strstr(mime_type, "wav") != 0)
    file_format = "wav";
  return exec_media_player::open_stream(mime_type);
}
