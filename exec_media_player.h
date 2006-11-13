/* JiveAudio - multimedia plugin for Mozilla -*-C++-*-
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

#ifndef _EXEC_MEDIA_PLAYER_H
#define _EXEC_MEDIA_PLAYER_H 1

#include "media_player.h"

class exec_media_player:
  public file_media_player
{
public:
  exec_media_player(const char *command_name);
  ~exec_media_player();

  virtual void exec(int stream_fileno);

  void play();
  void stop();

protected:
  const char *command_name;
  int fileno;
#ifdef _POSIX_THREADS
  pthread_t thread;
  pthread_mutex_t thread_mutex;
#endif /* _POSIX_THREADS */
  pid_t child;

  pid_t spawn(int fileno);
#ifdef _POSIX_THREADS
  void run();
  static void *run(void *);
#endif
};

class sox_media_player:
  public exec_media_player
{
public:
  sox_media_player(const char *command_name);

  void exec(int stream_fileno);
  bool open_stream(const char *mime_type);

protected:
  const char *file_format;
};

#endif /* !_EXEC_MEDIA_PLAYER_H */
