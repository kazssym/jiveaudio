/* -*-C++-*- */
/*
 * JiveAudio - multimedia plugin for Mozilla
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

#ifndef player_posixH
#define player_posixH 1

#include "player.h"

#if __unix

#include <sys/types.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if _POSIX_THREADS
#include <pthread.h>
#endif

class external_player : public file_media_player
{
public:
    external_player (const char *command_name);
    ~external_player ();

    virtual void exec (int stream_fileno);

    void start ();
    void stop ();
protected:
    const char *command_name;
    int fileno;
#if _POSIX_THREADS
    pthread_t thread;
    pthread_mutex_t thread_mutex;
#endif /* _POSIX_THREADS */
    pid_t child;

    pid_t spawn (int fileno);
#if _POSIX_THREADS
    void run ();
    static void *run (void *);
#endif
};

class sox_media_player : public external_player
{
public:
    sox_media_player (const char *command_name);

    void exec (int stream_fileno);
    bool open_stream (const char *mime_type);
protected:
    const char *file_format;
};

#endif /* __unix */

#endif
