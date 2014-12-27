TARGET = png2img
CFLAGS = -O2
SOURCES = png2img.cpp
OBJ = $(SOURCES:%.cpp=%.o)

# --- Windows Only--- #
ifeq ($(OS), Windows_NT)
	LIBS = -lfltk_images -lfltk_png -lfltk -lz -lole32 -luuid -lcomctl32 -lgdi32 -lcomdlg32
	RM = del /Q
# --- Linux Only--- #
else ifeq ($(shell uname), Linux)
	LIBS = -lfltk_images -lfltk -lpng -lX11 -lXext -ldl -lXft -lfontconfig -lXinerama
	RM = @rm -f
	RUN_PATH = ./
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	g++ -o "$@" $(OBJ) $(LIBS)

%.o: %.cpp
	g++ $(CFLAGS) -c $< -o $@

gen: $(TARGET)
	$(RUN_PATH)$(TARGET) ..

clean:
	$(RM) $(TARGET) *.o images.cpp images.h