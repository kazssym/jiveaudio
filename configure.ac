# Process this file with autoconf to produce a configure script.
AC_INIT([JiveAudio], [3.0.2], [jiveaudio@googlegroups.com], [jiveaudio])
AC_CONFIG_SRCDIR([plugins/audio.cpp])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.9])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CXX
AC_USE_SYSTEM_EXTENSIONS

gl_EARLY

# Checks for libraries.
AC_CHECK_LIB([mad], [mad_decoder_init], [MAD_LIBS="-lmad $MAD_LIBS"])
AC_SUBST([MAD_LIBS])dnl

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h io.h sys/wait.h syslog.h mad.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([tempnam])

gl_INIT

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AM_PATH_GTK_2_0([2.0.0])

PACKAGE_VERSION_MAJOR=`expr "$PACKAGE_VERSION" : ['0*\([0-9]*\)']`
PACKAGE_VERSION_MINOR=`expr "$PACKAGE_VERSION" : ['[0-9]*\\.0*\\([0-9]*\\)']`
PACKAGE_VERSION_EXTRA=`expr "$PACKAGE_VERSION" : ['[0-9]*\\.[0-9]*\\.0*\\([0-9]*\\)']`
AC_DEFINE_UNQUOTED([PACKAGE_VERSION_MAJOR], [${PACKAGE_VERSION_MAJOR:-0}],
[Define to the major version number of this package.])
AC_DEFINE_UNQUOTED([PACKAGE_VERSION_MINOR], [${PACKAGE_VERSION_MINOR:-0}],
[Define to the minor version number of this package.])
AC_DEFINE_UNQUOTED([PACKAGE_VERSION_EXTRA], [${PACKAGE_VERSION_EXTRA:-0}],
[Define to the release number of this package.])
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
AC_CONFIG_LINKS([prcpucfg.h:sdk/md/$cpucfg])
AC_OUTPUT
