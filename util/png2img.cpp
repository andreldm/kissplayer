#include <FL/Fl.H>
#include <FL/Fl_PNG_Image.H>

#include <dirent.h>
#include <iostream>

/*
 * g++ png2img.cpp -lfltk -lfltk_images -lpng -o png2img
 * or
 * g++ b.cpp -o b.exe -lfltk_images -lfltk_png -lz -lfltk -lole32 -luuid -lcomctl32 -mwindows
 * 
 * static unsigned char foo[] = { ... };
 * static Fl_RGB_Image bar(for, $1, $2, $3, $4);
 * 
 * $1 = img->w()
 * $2 = img->h()
 * $3 = img->d()
 * $4 = img->ld()
 * 
 * Only works for PNG. For more formats, check Fluid_Image.cxx:45
 */

using namespace std;

void write(const char* filepath);
bool hasEnding (string const &fullString, string const &ending);
const char* getName(const char* filepath, bool append_png);
FILE* out_cpp;
FILE* out_h;

int main(int argc, char* argv[])
{
  if(argc < 2) {
    printf("Converts png files to img object suitable for FLTK\nUsage: png2bin <directory>\n");
    return 1;
  }
  
  FILE *f = fl_fopen("images.cpp", "wb");
  if (f) out_cpp = f;
  f = fl_fopen("images.h", "wb");
  if (f) out_h = f;
  
  if(!out_cpp || !out_h){
    printf("Couldn't create output files");
    return 1;
  }
  
  bool filefound = false;
  const char* dirpath = argv[1];
  DIR *dir;

  if ((dir=opendir(dirpath)) != NULL)
  {
  struct dirent *ent;
      while((ent=readdir(dir)) != NULL)
      {
          string filename = ent->d_name;
          if (hasEnding(filename, "png")) {
            filefound = true;
            filename.insert(0, dirpath);
            write(filename.c_str());
          }
      }
      closedir(dir);
  }
  
  if(!filefound) {
    printf("No png files have been found in %s\n", dirpath);
  }

  return 0;
}

bool hasEnding (string const &fullString, string const &ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

void write(const char* filepath)
{
  Fl_PNG_Image *img = new Fl_PNG_Image(filepath);

  if (!img) {
    printf("Can't open file.\n");
    return;
  }
  const char* s = img->data()[0];
  int length = (img->w() * img->d() + img->ld()) * img->h();
  
  const unsigned char *w = (const unsigned char *)s;
  const unsigned char *e = w+length;
  
  const char* var_name_png = getName(filepath, true);
  const char* var_name_img = getName(filepath, false);
  fprintf(stdout, "static unsigned char %s[] = {", var_name_png);

  for (; w < e;) {
    unsigned char c = *w++;
    fprintf(stdout, "%d", c);
    if (w<e) putc(',', stdout);
  }
  fprintf(stdout, "};\n");
  fprintf(stdout, "static Fl_RGB_Image %s(%s, %d, %d, %d, %d);\n\n", var_name_img, var_name_png, img->w(), img->h(), img->d(), img->ld());
}

const char* getName(const char* filepath, bool append_png) {
    string* s = new string(filepath);
    size_t foundSlash = s->find_last_of("/\\");
    string str = s->substr(foundSlash+1);
    
    delete s;

    size_t foundDot = str.find_last_of(".");
    str = str.substr(0, foundDot);
    if(append_png) str += "_png";

    string* copy = new string(str.c_str());
    return copy->c_str();
}
