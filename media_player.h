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

#ifndef _MEDIA_PLAYER_H
#define _MEDIA_PLAYER_H 1

#include <cstdio>

class media_player
{
public:
  media_player();
  virtual ~media_player();

  void set_loop(bool loop) throw ();
  virtual bool open_stream(const char *mime_type) = 0;
  virtual size_t stream_buffer_size() const = 0;
  virtual size_t write_stream(const void *buf, size_t nbytes) = 0;
  virtual void close_stream() = 0;
  virtual void play() = 0;
  virtual void stop() = 0;

protected:
  bool loop;
};

inline void
media_player::set_loop(bool loop) throw ()
{
  this->loop = loop;
}

class file_media_player
  : public media_player
{
public:
  file_media_player();
  ~file_media_player();

  bool open_stream(const char *mime_type);
  size_t stream_buffer_size() const;
  size_t write_stream(const void *buf, size_t nbytes);
  void close_stream();

protected:
  char file_name[L_tmpnam];

private:
  int fildes;
};

#endif /* !_MEDIA_PLAYER_H */
