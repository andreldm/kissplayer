CC = g++
LIBS = -lfltk -lole32 -luuid -lcomctl32 -lwsock32 -lgdi32 -lcomdlg32 -lfltk_images -lfltk_png -lz -lcurl -ltag -lfmodex -lsqlite3
CFLAGS = -O2 -mwindows
OUTPUT_NAME = "bin/KISS Player.exe"

all: src/win/kiss.res
	$(CC) src/win/kiss.res src/*.cpp src/win/*.cpp -o $(OUTPUT_NAME) $(LIBS) $(CFLAGS) 

src/win/kiss.res: src/win/kiss.rc
	windres src/win/kiss.rc -O coff -o src/win/kiss.res

patch:
	diff -ru src_orig src > patch.txt