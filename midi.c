/* JiveAudio - plugins for Mozilla
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

#include "common.h"

#define _GNU_SOURCE 1

#include <stdio.h>
#include <signal.h>

#ifdef STDC_HEADERS
#  include <string.h>
#endif

#ifdef HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif

#ifdef HAVE_FCNTL_H
#  include <fcntl.h>
#else
#  define O_RDONLY 0
#  define O_WRONLY 1
#  define O_RDWR   2
#endif

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

NPError
NPP_Initialize(void)
{
  fprintf(stderr, "libmidiplugin: NPP_Initialize\n");
  return NPERR_NO_ERROR;
}

void
NPP_Shutdown(void)
{
  fprintf(stderr, "libmidiplugin: NPP_Shutdown\n");
}

NPError
NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
  fprintf(stderr, "libmidiplugin: NPP_GetValue (%d)\n", variable);
  switch (variable)
    {
    case NPPVpluginNameString:
      *((const char **) value) = "JiveAudio MIDI Plugin";
      break;

    case NPPVpluginDescriptionString:
      *((const char **) value) = PACKAGE_STRING;
      break;

    default:
      break;
    }

  return NPERR_NO_ERROR;
}

NPError
NPP_SetValue(NPP instance, NPPVariable variable, void *value)
{
  fprintf(stderr, "libmidiplugin: NPP_SetValue (%d)\n", variable);
  return NPERR_NO_ERROR;
}

static pid_t child = 0;

static void
stop(void)
{
  if (child > 0) {
    kill(child, SIGTERM);
    waitpid(child, NULL, 0);
    child = 0;
  }
}

NPError
NPP_New(NPMIMEType MIMEType, NPP instance, uint16 mode,
	int16 argc, char **argn, char **argv, NPSavedData *savedData)
{
  int i;

  fprintf(stderr, "libmidiplugin: NPP_New (%s, %d)\n", MIMEType, mode);
  for (i = 0; i != argc; ++i)
    fprintf(stderr, "\t%s=%s\n", argn[i], argv[i]);
  return NPERR_NO_ERROR;
}

NPError
NPP_Destroy(NPP instance, NPSavedData **savedData)
{
  fprintf(stderr, "libmidiplugin: NPP_Destroy\n");
  stop();
  return NPERR_NO_ERROR;
}

void
NPP_Print(NPP instance, NPPrint *print)
{
  fprintf(stderr, "libmidiplugin: NPP_Print\n");
}

NPError
NPP_SetWindow(NPP instance, NPWindow *window)
{
  fprintf(stderr, "libmidiplugin: NPP_SetWindow\n");
  return NPERR_NO_ERROR;
}

int16
NPP_HandleEvent(NPP instance, NPEvent *event)
{
  fprintf(stderr, "libmidiplugin: NPP_HandleEvent\n");
  return 0;
}

NPError
NPP_NewStream(NPP instance, NPMIMEType MIMEType, NPStream *stream,
	      NPBool seekable, uint16 *mode)
{
  fprintf(stderr, "libmidiplugin: NPP_NewStream (%s, %d)\n", MIMEType, seekable);
  stop();
  *mode = NP_ASFILEONLY;
  return NPERR_NO_ERROR;
}

NPError
NPP_DestroyStream(NPP instance, NPStream *stream, NPError reason)
{
  fprintf(stderr, "libmidiplugin: NPP_DestroyStream (%d)\n", reason);
  return NPERR_NO_ERROR;
}

int32
NPP_WriteReady(NPP instance, NPStream *stream)
{
  fprintf(stderr, "libmidiplugin: NPP_WriteReady\n");
  return 4096;
}

int32
NPP_Write(NPP instance, NPStream *stream, int32 off, int32 len, void *buf)
{
  fprintf(stderr, "libmidiplugin: NPP_WriteReady (%ld, %ld)\n", (long) off, (long) len);
  return len;
}

void
NPP_StreamAsFile(NPP instance, NPStream *stream, const char *name)
{
  int fileno;

  fprintf(stderr, "libmidiplugin: NPP_StreamAsFile (%s)\n", name);
  fileno = open(name, O_RDONLY);
  if (fileno == -1) {
    perror(name);
    return;
  }

  child = fork();
  if (child == -1) {
    perror("fork");
    return;
  }

  if (child == 0) {
    int null;

    dup2(fileno, STDIN_FILENO);
    close(fileno);
    null = open("/dev/null", O_WRONLY);
    dup2(null, STDOUT_FILENO);
    close(null);
    execlp("timidity", "timidity", "-", (char *) 0);
    perror("execlp");
    _exit(EXIT_FAILURE);
  }

  close(fileno);
}

void
NPP_URLNotify(NPP instance, const char *URL, NPReason reason, void *data)
{
  fprintf(stderr, "libmidiplugin: NPP_URLNotify\n");
}
