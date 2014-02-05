# This makefile should build the project on Linux and Windows(MinGW)
# An autotools setup in being crafted, for now we must live with this makefile

SOURCES = src/dao.cpp \
src/images.cpp \
src/lyrics_fetcher.cpp \
src/main.cpp \
src/music.cpp \
src/sound.cpp \
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

# --- Windows Only--- #
ifeq ($(OS), Windows_NT)
	SOURCES += src/win/windows_specific.cpp
	RESOURCE = src/win/kiss.res

	RM = del /Q
	DEVNULL = NUL
	FixPath = $(subst /,\,$1)

	# Configure the depencies if needed!
	LIBS = -lfltk -lole32 -luuid -lcomctl32 -lwsock32 -lgdi32 -lcomdlg32 -lfltk_images -lfltk_png -lz -lcurl -ltag -lfmodex -lsqlite3 -lintl
	CFLAGS = -O2 -mwindows
	TARGET = KISS\ Player.exe

# --- Linux Only--- #
else ifeq ($(shell uname), Linux)
	SOURCES += src/linux/linux_specific.cpp

	RM = rm -f
	DEVNULL = /dev/null
	FixPath = $1

	# Configure the depencies if needed!
	LIBS = -lfltk -lX11 -lXext -lXft -lXtst -lXpm -lXinerama -lfontconfig -lpthread -ldl -lm -lcurl -ltag -lsqlite3 -lfltk_images -lpng -lfmodex#64
	CFLAGS = -O2
	TARGET = kissplayer
endif

CPP_SOURCES = $(filter %.cpp, $(SOURCES))
C_SOURCES = $(filter %.c, $(SOURCES))

CPP_OBJ = $(CPP_SOURCES:%.cpp=%.o)
C_OBJ = $(C_SOURCES:%.c=%.o)

all: $(TARGET)

$(TARGET): $(RESOURCE) $(C_OBJ) $(CPP_OBJ)
	g++ -o "$@" $^ $(LIBS)

%.o: %.cpp
	g++ $(CFLAGS) -c $< -o $@

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

# --- Windows Only--- #
ifeq ($(OS), Windows_NT)
$(RESOURCE): src/win/kiss.rc
	windres $^ -O coff -o $@
endif

clean:
	$(RM) $(call FixPath, $(CPP_OBJ) $(C_OBJ) src/win/kiss.res) $(subst \,,"$(TARGET)") 2> $(DEVNULL)
