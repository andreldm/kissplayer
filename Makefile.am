DIST_SUBDIRS = m4 po data
SUBDIRS = po data
AUTOMAKE_OPTIONS=foreign subdir-objects
CFLAGS=-O2

# Define a C macro LOCALEDIR indicating where catalogs will be installed.
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

bin_PROGRAMS = kissplayer

kissplayer_SOURCES = src/dao.cpp \
  src/locale.cpp \
  src/images.cpp \
  src/lyrics_fetcher.cpp \
  src/main.cpp \
  src/music.cpp \
  src/sound.cpp \
  src/sync.cpp \
  src/util.cpp \
  src/tinycthread.c \
  src/window_about.cpp \
  src/window_loading.cpp \
  src/window_main.cpp \
  src/window_settings.cpp \
  src/widget/ksp_browser.cpp \
  src/widget/ksp_check_button.cpp \
  src/widget/ksp_menu_item.cpp \
  src/widget/ksp_slider.cpp \
  src/widget/ksp_tile.cpp \
  src/widget/ksp_volume_controller.cpp

EXTRA_DIST = img/kiss.ico \
  img/img_thumb_buttons.bmp \
  src/*.h \
  src/linux/*.h \
  src/linux/icon.xpm \
  src/widget/*.h \
  src/win/*.h \
  src/win/*.rc

####################################
# Windows Specific
####################################
if WINDOWS
kissplayer_LDFLAGS = -mwindows
kissplayer_CPPFLAGS = -DCURL_STATICLIB
kissplayer_SOURCES += src/win/windows_specific.cpp \
src/win/kiss.rc

.rc.o:
	windres -i $< -O coff -o $@
endif

####################################
# Linux Specific
####################################
if LINUX
kissplayer_SOURCES += src/linux/linux_specific.cpp
endif