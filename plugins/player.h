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

#ifndef playerH
#define playerH 1

/**
 * Abstract media player.
 */
class player {
public:
#if _WIN32
    typedef HWND window_type;
#else
    typedef void *window_type;
#endif

public:
    player ();

public:
    virtual ~player ();

public:
    void set_loop (bool loop)
    {
        a_loop = loop;
    }
    bool loop () const
    {
        return a_loop;
    }
protected:
    bool a_loop;

public:
    virtual bool open_stream (const char *mime_type) = 0;
    virtual size_t stream_buffer_size () const = 0;
    virtual size_t write_stream (const void *buf, size_t nbytes) = 0;
    virtual void close_stream () = 0;

public:
    virtual void start () = 0;
    virtual void stop () = 0;

    virtual void set_window (window_type window);
};

#endif
