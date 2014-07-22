DIST_SUBDIRS = po
SUBDIRS = po

AUTOMAKE_OPTIONS=foreign subdir-objects

# Define a C macro LOCALEDIR indicating where catalogs will be installed.
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

CFLAGS=-O2

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

if WINDOWS

kissplayer_LDFLAGS = -mwindows

kissplayer_SOURCES += src/win/windows_specific.cpp \
src/win/kiss.rc

.rc.o:
	windres -i $< -O coff -o $@

endif


if LINUX

kissplayer_SOURCES += src/linux/linux_specific.cpp

endif