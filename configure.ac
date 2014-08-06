################################################################
# Process this file with autoconf to produce a configure script.
################################################################

AC_PREREQ([2.68])
AC_INIT([KISS Player], [0.6.3], [andreldm1989@gmail.com], [kissplayer], [http://sourceforge.net/projects/kissplayer/])
AC_CONFIG_AUX_DIR(autotools)
AC_CONFIG_MACRO_DIR(m4)

####################################
# Check platform
####################################
AC_CANONICAL_TARGET

AC_MSG_CHECKING([operating system type])

case "$target" in
  *linux*)
    os=linux
    AC_MSG_RESULT([Linux])
    ;;
  *mingw*)
    os=windows
    AC_MSG_RESULT([Windows])
    ;;
  *)
    AC_MSG_ERROR(["Unrecognised target OS: $target"])
    ;;
esac
AM_CONDITIONAL(LINUX, test "x$os" = "xlinux")
AM_CONDITIONAL(WINDOWS, test "x$os" = "xwindows")

case "$target" in
  *64*)
    arch=64
    ;;
esac
AM_CONDITIONAL(IS64BIT, test "x$arch" = "x64")

####################################
# Init
####################################
AC_CONFIG_SRCDIR([src/main.cpp])
#AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.11 subdir-objects foreign])
AM_SILENT_RULES([yes])

####################################
# Check for programs
####################################
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_MAKE_SET

####################################
# Check for headers
####################################
AC_CHECK_HEADERS([unistd.h])

####################################
# Check for libraries
####################################

if test "x$os" = "xlinux"; then
  AC_CHECK_LIB([X11], [main], [], [echo "libX11 is required for this program"; exit -1])
  AC_CHECK_LIB([Xext], [main], [], [echo "libXext is required for this program"; exit -1])
  AC_CHECK_LIB([Xft], [main], [], [echo "libXft is required for this program"; exit -1])
  AC_CHECK_LIB([Xtst], [main], [], [echo "libXtst is required for this program"; exit -1])
  AC_CHECK_LIB([Xpm], [main], [], [echo "libXpm is required for this program"; exit -1])
  AC_CHECK_LIB([Xinerama], [main], [], [echo "libXinerama is required for this program"; exit -1])
  AC_CHECK_LIB([fontconfig], [main], [], [echo "libfontconfig is required for this program"; exit -1])
  AC_CHECK_LIB([pthread], [main], [], [echo "libpthread is required for this program"; exit -1])
  AC_CHECK_LIB([dl], [main], [], [echo "libdl is required for this program"; exit -1])
  AC_CHECK_LIB([curl], [main], [], [echo "libcurl is required for this program"; exit -1])

  if test "x$arch" = "x64"; then
    AC_CHECK_LIB([fmodex64], [main], [], [echo "libfmodex64 is required for this program"; exit -1])
  else
    AC_CHECK_LIB([fmodex], [main], [], [echo "libfmodex required for this program"; exit -1])
  fi
elif test "x$os" = "xwindows"; then
  AC_CHECK_LIB([ole32], [main], [], [echo "libole32 is required for this program"; exit -1])
  AC_CHECK_LIB([uuid], [main], [], [echo "libuuid is required for this program"; exit -1])
  AC_CHECK_LIB([comctl32], [main], [], [echo "libcomctl32 is required for this program"; exit -1])
  AC_CHECK_LIB([wsock32], [main], [], [echo "libwsock32 is required for this program"; exit -1])
  AC_CHECK_LIB([gdi32], [main], [], [echo "libgdi32 is required for this program"; exit -1])
  AC_CHECK_LIB([comdlg32], [main], [], [echo "libcomdlg32 is required for this program"; exit -1])
  AC_CHECK_LIB([intl], [main], [], [echo "libintl is required for this program"; exit -1])
  AC_CHECK_LIB([fmodex], [main], [], [echo "libfmodex required for this program"; exit -1])
  AC_CHECK_LIB([curldll], [main], [], [
    AC_CHECK_LIB([curl], [main], [], [echo "libcurl or libcurldll required for this program"; exit -1])
  ])
fi

AC_CHECK_LIB([z], [main])
AC_CHECK_LIB([png], [png_sig_cmp], [], [echo "libpng is required for this program"; exit -1])
AC_CHECK_LIB([sqlite3], [sqlite3_close], [], [echo "libsqlite3 is required for this program"; exit -1])
AC_CHECK_LIB([tag], [read], [], [echo "TagLib is required for this program"; exit -1])
AC_CHECK_LIB([fltk], [main], [], [echo "FLTK is required for this program"; exit -1])
AC_CHECK_LIB([fltk_images], [main], [], [echo "libfltk_images is required for this program"; exit -1])

####################################
# Support for the po directory
####################################
AM_PO_SUBDIRS

AC_CONFIG_FILES([
  Makefile
  data/Makefile
  data/kissplayer.desktop
  data/icons/Makefile
  data/icons/hicolor/Makefile
  data/icons/hicolor/48x48/Makefile
  data/icons/hicolor/48x48/apps/Makefile
  data/icons/hicolor/64x64/Makefile
  data/icons/hicolor/64x64/apps/Makefile
  data/icons/hicolor/scalable/Makefile
  data/icons/hicolor/scalable/apps/Makefile
  m4/Makefile
])
AC_CONFIG_FILES([po/Makefile], [AM_POSTPROCESS_PO_MAKEFILE])
AC_OUTPUT
