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

#include <csignal>

#ifdef HAVE_FCNTL_H
#  include <fcntl.h>
#else
#  define O_RDONLY 0
#  define O_WRONLY 1
#  define O_RDWR   2
#endif

#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif

#ifdef _POSIX_THREADS
#  include <pthread.h>
#endif

#if !defined O_BINARY
#  define O_BINARY 0
#endif

using namespace std;

exec_media_player::exec_media_player(const char *_command_name):
  command_name(_command_name),
  fileno(-1),
#ifdef _POSIX_THREADS
  thread(0),
#endif /* _POSIX_THREADS */
  child(0)
{
#ifdef _POSIX_THREADS
  pthread_mutex_init(&thread_mutex, 0);
#endif /* _POSIX_THREADS */
}

exec_media_player::~exec_media_player()
{
  stop();
#ifdef _POSIX_THREADS
  pthread_mutex_destroy(&thread_mutex);
#endif /* _POSIX_THREADS */
}

void
exec_media_player::exec(int stream_fileno)
{
  dup2(stream_fileno, 0);
  close(stream_fileno);

  execlp(command_name, command_name, "-", (const char *) 0);
  perror(command_name);
}

pid_t
exec_media_player::spawn(int fileno)
{
  int p = fork();
  if (p == -1)
    return 0;

  if (p == 0)
    {
      setsid();

      int null_fileno = open("/dev/null", O_WRONLY);
      if (null_fileno == -1)
	{
	  perror("/dev/null");
	  _exit(1);
	}
      dup2(null_fileno, 1);
      close(null_fileno);

      exec(fileno);
      _exit(1);
    }

  return p;
}

#ifdef _POSIX_THREADS
void
exec_media_player::run()
{
  pthread_mutex_lock(&thread_mutex);
  if (fileno != -1)
    {
      int status;
      do
	{
	  child = spawn(fileno);
	  pthread_mutex_unlock(&thread_mutex);
	  if (child == 0)
	    {
	      perror("fork");
	      return;
	    }

	  if (waitpid(child, &status, 0) == -1)
	    {
	      perror("waitpid");
	      return;
	    }

	  lseek(fileno, 0, SEEK_SET);
	  pthread_mutex_lock(&thread_mutex);
	}
      while (loop && WIFEXITED(status) && fileno != -1);
      child = 0;
    }
  pthread_mutex_unlock(&thread_mutex);
}

void *
exec_media_player::run(void *arg)
{
  exec_media_player *player = static_cast <exec_media_player *> (arg);
  if (player == 0)
    return 0;

  player->run();
  return player;
}
#endif /* _POSIX_THREADS */

void
exec_media_player::start()
{
  if (fileno != -1)
    return;

  fileno = open(file_name, O_RDONLY | O_BINARY);
  if (fileno == -1)
    {
      perror(file_name);
      return;
    }

#ifdef _POSIX_THREADS
  pthread_create(&thread, 0, &run, this);
#else /* !_POSIX_THREADS */
  child = spawn(fileno);
  if (child == 0)
    {
      perror("fork");
      return;
    }
#endif /* _POSIX_THREADS */
}

void
exec_media_player::stop()
{
  if (fileno == -1)
    return;

  close(fileno);
  fileno = -1;

#ifdef _POSIX_THREADS
  if (thread != 0)
    {
      pthread_mutex_lock(&thread_mutex);
      if (child != 0)
	kill(-child, SIGTERM);
      pthread_mutex_unlock(&thread_mutex);
      pthread_join(thread, 0);
      thread = 0;
    }
#else /* !_POSIX_THREADS */
  if (child != 0)
    {
      kill(-child, SIGTERM);
      waitpid(child, 0, 0);
      child = 0;
    }
#endif /* !_POSIX_THREADS */
}
