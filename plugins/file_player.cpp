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
#include <config.h>
#endif
#define _GNU_SOURCE 1
#define _REENTRANT 1

#if _WIN32
#define STRICT 1
#include <windows.h>
#pragma hdrstop
#endif

#include "plugins/player.h"

#include <cstdlib>
#include <cstdio>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_FCNTL_H
#include <fcntl.h>
#else
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2
#endif

#if HAVE_IO_H
#include <io.h>
#endif

#ifndef O_EXCL
#define O_EXCL 0
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

using namespace std;

file_media_player::file_media_player () :
    file_name (0),
    fildes (-1)
{
}

file_media_player::~file_media_player ()
{
    close_stream ();
    clean ();
}

void
file_media_player::clean ()
{
    if (file_name == 0)
        return;

    remove (file_name);
    free (file_name);
    file_name = 0;
}

bool
file_media_player::open_stream (const char *mime_type)
{
    if (fildes != -1)
        close (fildes);

    clean ();
#if HAVE_TEMPNAM
    file_name = tempnam (0, "jive");
#else
    file_name = static_cast <char *> (malloc (L_tmpnam));
    tmpnam (file_name);
#endif
    fildes = open (file_name, O_WRONLY | O_CREAT | O_EXCL | O_BINARY, 0600);
    if (fildes == -1)
        return false;

    return true;
}

long
file_media_player::stream_buffer_size () const
{
    if (fildes == -1)
        return 0;

    return 4096;
}

long
file_media_player::write_stream (const void *buf, long nbytes)
{
    if (fildes == -1)
        return -1;

    size_t k = write (fildes, buf, nbytes);
    return k;
}

void
file_media_player::close_stream ()
{
    if (fildes == -1)
        return;

    close (fildes);
    fildes = -1;
}
