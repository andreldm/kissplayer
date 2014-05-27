# This makefile should build the project on Linux and Windows(MinGW)
# An autotools setup is being crafted, for now we must live with this makefile

SOURCES = src/dao.cpp \
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

# --- Windows Only--- #
ifeq ($(OS), Windows_NT)
	SOURCES += src/win/windows_specific.cpp
	RESOURCE = src/win/kiss.res
	OUTPUTFOLDER = "KISS Player"

	RM = del /Q
	DEVNULL = NUL
	FixPath = $(subst /,\,$1)
	CreateFolder = @if not exist $1 mkdir $1
	DeleteFolder = @if exist $1 rd /S /Q $1
	CopyFile = @if exist $1 copy $1 $2 /Y
	CopyFolder = @if exist $1 xcopy $1 $2\$1 /I /E /Y /Q

	# Configure the dependencies if needed!
	LIBS = -lfltk -lole32 -luuid -lcomctl32 -lwsock32 -lgdi32 -lcomdlg32 -lfltk_images -lfltk_png -lz -lcurldll -ltag -lfmodex -lsqlite3 -lintl -mwindows
	CFLAGS = -O2
	TARGET = KISS\ Player.exe

# --- Linux Only--- #
else ifeq ($(shell uname), Linux)
	SOURCES += src/linux/linux_specific.cpp
	OUTPUTFOLDER = "/usr/bin/"
	OUTPUTFOLDERLOCAL = "$(HOME)/.kissplayer/"

	RM = @rm -f
	DEVNULL = /dev/null
	FixPath = $1
	CreateFolder = @mkdir -p $1
	DeleteFolder = @rm -r -f $1
	CopyFile = @cp $1 $2
	CopyFolder = @cp -r $1 $2

	# Configure the dependencies if needed!
	LIBS = -lfltk -lX11 -lXext -lXft -lXtst -lXpm -lXinerama -lfontconfig -lpthread -ldl -lm -lcurl -ltag -lsqlite3 -lfltk_images -lpng -lfmodex64
	CFLAGS = -O2
	TARGET = kissplayer
endif

CPP_SOURCES = $(filter %.cpp, $(SOURCES))
C_SOURCES = $(filter %.c, $(SOURCES))

CPP_OBJ = $(CPP_SOURCES:%.cpp=%.o)
C_OBJ = $(C_SOURCES:%.c=%.o)

PO_SOURCES = $(notdir $(wildcard po/*po))
PO_OBJ = $(addprefix locale/,$(PO_SOURCES:.po=.UTF-8/LC_MESSAGES/kissplayer.mo))

all: $(TARGET)

$(TARGET): $(RESOURCE) $(C_OBJ) $(CPP_OBJ) $(PO_OBJ)
	g++ -o "$@" $(RESOURCE) $(C_OBJ) $(CPP_OBJ) $(LIBS)

%.o: %.cpp
	g++ $(CFLAGS) -c $< -o $@

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

locale/%.UTF-8/LC_MESSAGES/kissplayer.mo: po/%.po
	$(call CreateFolder,"$(dir $@)")
	msgfmt -c -o $@ po/$*.po

update-locale: 
	xgettext -d kissplayer -s src/*.cpp src/widget/*.cpp --keyword=_ -o po/kissplayer.pot
	find po -name "*.po" -exec msgmerge -U '{}' po/kissplayer.pot --backup=off \;

# --- Windows Only--- #
ifeq ($(OS), Windows_NT)
$(RESOURCE): src/win/kiss.rc
	windres $^ -O coff -o $@
endif

clean:
	$(RM) $(call FixPath, $(CPP_OBJ) $(C_OBJ) src/win/kiss.res) $(subst \,,"$(TARGET)") 2> $(DEVNULL)
	$(call DeleteFolder,locale)
	@echo Clean complete!

install:
ifeq ($(OS), Windows_NT)
	$(call CreateFolder,$(OUTPUTFOLDER))
	$(call CopyFile,$(subst \,,"$(TARGET)"),$(OUTPUTFOLDER))
	$(call CopyFile,README.txt,$(OUTPUTFOLDER))
	$(call CopyFile,LICENSE.txt,$(OUTPUTFOLDER))
	$(call CopyFile,CHANGELOG.txt,$(OUTPUTFOLDER))
	$(call CopyFolder,locale,$(OUTPUTFOLDER))
	@echo Move the folder $(OUTPUTFOLDER) to anywhere you want
else ifeq ($(shell uname), Linux)
	$(call CopyFile,$(TARGET),$(OUTPUTFOLDER))
	$(call CreateFolder,$(OUTPUTFOLDERLOCAL))
	$(call CopyFolder,locale,$(OUTPUTFOLDERLOCAL))
# Kludge!
	@sudo chown -R `who am i | awk {'print $$1'}| xargs -IREPL echo REPL:` $(OUTPUTFOLDERLOCAL)
endif
