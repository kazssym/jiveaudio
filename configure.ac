# Process this file with autoconf to produce a configure script.
AC_INIT([JiveAudio], [2.0.7], [jiveaudio@lists.hypercore.co.jp], [jiveaudio])
AC_CONFIG_SRCDIR([audio.cpp])
AC_CONFIG_HEADERS([config.h])
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

case $host_os in
linux-gnu*)
  prcpucfg=_linux.cfg
  ;;
*)
  AC_MSG_WARN([Unknown host operating system; assuming GNU/Linux])
  prcpucfg=_linux.cfg
  ;;
esac
AC_CONFIG_LINKS([nspr/prcpucfg.h:nspr/$prcpucfg])
AC_CONFIG_FILES([Makefile plugin/Makefile java/Makefile nspr/Makefile])
AC_OUTPUT
