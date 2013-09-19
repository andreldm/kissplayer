#include "dir_tools.h"

#include <FL/Fl.H>
#include <FL/filename.H>
#include <sys/types.h>
#include <unistd.h>
#include "tinydir.h"

using namespace std;

void scan(const char* directory, vector<string>* filelist)
{
    tinydir_dir dir;
    int i;
    tinydir_open_sorted(&dir, directory);

    for (i = 0; i < dir.n_files; i++)
    {
        tinydir_file file;
        tinydir_readfile_n(&dir, &file, i);
        
        if(file.name[0] == '.') {
            continue;
        }
        
        if (file.is_dir)
        {
            scan(file.path, filelist);
            continue;
        }
        
        const char* ext = fl_filename_ext(file.name);
        if(strcmp(ext, ".mp3") == 0 ||
            strcmp(ext, ".wma") == 0 ||
            strcmp(ext, ".ogg") == 0 ||
            strcmp(ext, ".wav") == 0 ||
            strcmp(ext, ".flac") == 0) {
            filelist->push_back(file.path);
        }
    }
    
    tinydir_close(&dir);
}
