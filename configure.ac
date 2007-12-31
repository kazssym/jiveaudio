# Process this file with autoconf to produce a configure script.
AC_INIT([JiveAudio], [3.0.0], [jiveaudio@googlegroups.com], [jiveaudio])
AC_CONFIG_SRCDIR([plugins/plugin.cpp])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.9])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_PATH_X
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h io.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([tempnam])

case $host_os in
linux-gnu*)
  cpucfg=_linux.cfg
  ;;
mingw32*)
  cpucfg=_winnt.cfg
  ;;
*)
  AC_MSG_WARN([Unknown host operating system; assuming GNU/Linux])
  cpucfg=_linux.cfg
  ;;
esac
AC_CONFIG_FILES([Makefile sdk/Makefile plugins/Makefile doc/Makefile])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_LINKS([sdk/prcpucfg.h:sdk/md/$cpucfg])
AC_OUTPUT
