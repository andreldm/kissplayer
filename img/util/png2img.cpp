/*
 * Outputs a char sequence for each image found in the informed directory, suitable for FLTK.
 * Linux:   g++ png2img.cpp -o png2img -lfltk_images -lfltk -lpng -lX11 -lXext -ldl -lXft -lfontconfig -lXinerama
 * Windows: g++ png2img.cpp -o png2img.exe -lfltk_images -lfltk_png -lfltk -lz -lole32 -luuid -lcomctl32 -lgdi32 -lcomdlg32
 * 
 * Output format:
 * static unsigned char foo[] = { ... };
 * static Fl_RGB_Image bar(for, $1, $2, $3, $4);
 * 
 * Where:
 * $1 = img->w()
 * $2 = img->h()
 * $3 = img->d()
 * $4 = img->ld()
 * 
 * Note: Only works for PNG. For more formats, check Fluid_Image.cxx:45
 */

#include <iostream>
#include <dirent.h>

#include <FL/Fl.H>
#include <FL/filename.H>
#include <FL/Fl_PNG_Image.H>

#if defined(WIN32) && !defined(__CYGWIN__)
  #define DIR_SEPARATOR "\\"
#else
  #define DIR_SEPARATOR "/"
#endif

using namespace std;

void write(const char* filepath);
bool hasEnding (string& filepath, const char* ext);
const char* getName(const char* filepath, bool append_png);
FILE* out_cpp;
FILE* out_h;
bool firstFile = true;

int main(int argc, char* argv[])
{
  if(argc < 2) {
    printf("Converts png files to img object suitable for FLTK.\nUsage: png2img <directory>\n");
    return 1;
  }

  FILE* f = fl_fopen("images.cpp", "wb");
  if(f) out_cpp = f;
  f = fl_fopen("images.h", "wb");
  if(f) out_h = f;

  if(!out_cpp || !out_h) {
    printf("ERROR: Couldn't create output files!");
    return 1;
  }

  bool filefound = false;
  const char* dirpath = argv[1];
  DIR* dir;

  if((dir = opendir(dirpath)) != NULL) {
    struct dirent* ent;
    while((ent=readdir(dir)) != NULL) {
      string filename = ent->d_name;
      if(hasEnding(filename, ".png")) {
        filefound = true;
        filename.insert(0, DIR_SEPARATOR);
        filename.insert(0, dirpath);
        write(filename.c_str());
      }
    }
    closedir(dir);
  }

  if(filefound) {
    fprintf(out_h, "\n#endif\n");
  } else {
    printf("No png files have been found in %s\n", dirpath);
  }

  return 0;
}

bool hasEnding (string& filepath, const char* ext)
{
  return (strcmp(fl_filename_ext(filepath.c_str()), ext) == 0);
}

void write(const char* filepath)
{
  printf("Writing file: %s\n", filepath);
  Fl_PNG_Image* img = new Fl_PNG_Image(filepath);

  if(!img) {
    printf("Can't open file.\n");
    return;
  }

  if(firstFile) {
    firstFile = false;
    fprintf(out_cpp, "#include \"images.h\"\n");

    fprintf(out_h, "#ifndef images_h\n");
    fprintf(out_h, "#define images_h\n\n");
    fprintf(out_h, "#include <FL/Fl_RGB_Image.H>\n\n");
  }

  const char* s = img->data()[0];
  int length = (img->w() * img->d() + img->ld()) * img->h();

  const unsigned char* w = (const unsigned char*)s;
  const unsigned char* e = w + length;

  const char* var_name_png = getName(filepath, true);
  const char* var_name_img = getName(filepath, false);
  fprintf(out_cpp, "\nstatic unsigned char %s[] = {", var_name_png);

  for(; w < e;) {
    unsigned char c = *w++;
    fprintf(out_cpp, "%d", c);
    if(w<e) putc(',', out_cpp);
  }
  fprintf(out_cpp, "};\n");
  fprintf(out_cpp, "Fl_RGB_Image %s(%s, %d, %d, %d, %d);\n", var_name_img, var_name_png, img->w(), img->h(), img->d(), img->ld());
  fprintf(out_h, "extern Fl_RGB_Image %s;\n", var_name_img);
}

const char* getName(const char* filepath, bool append_png) {
  int pad = 0;
  const char* filename = fl_filename_name(filepath);
  if(filename[0] == '.') {
    pad = 1;
  }
  
  string* str = new string(fl_filename_setext(const_cast<char *>(filename + pad), sizeof(filename + pad), NULL));

  str->insert(0, "img_");
  if(append_png) {
    str->append("_png");
  }

  return str->c_str();
}
