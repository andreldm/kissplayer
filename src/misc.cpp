#include "misc.h"

/**
* This is a place for general purpose functions.
*/

/**
* Removes whitespaces at the beginning and end of a string
*/
void trim(string &str)
{
    string::size_type pos = str.find_last_not_of(" \f\n\r\t\v");
    str.erase(pos + 1);

    pos = str.find_first_not_of(" \f\n\r\t\v");
    str.erase(0, pos);
}

/**
* Takes a time value in miliseconds and returns formated in mm:ss.
* Ex: 123 secs -> 02:03 |  799 secs -> 13:19
*/
const char *formatTime (int time)
{
    stringstream stream;

    time = (int) time / 1000;

    int minutes = (int) (time / 60);
    int seconds = (int) (time % 60);

    stream << (minutes <= 9 ? "0" : "") << minutes << ":" << (seconds <= 9 ? "0" : "") << seconds;

    return stream.str().c_str();
}

// random generator function:
ptrdiff_t myrandom (ptrdiff_t i) { return rand()%i;}

void randomize(vector<int> **listRandom, int max)
{
    ptrdiff_t (*p_myrandom)(ptrdiff_t) = myrandom;
    srand((int)time(NULL));
    *listRandom = new vector<int>();
    for(int i = 0; i < max; i++)
        (*listRandom)->push_back(i);

    random_shuffle((*listRandom)->begin(), (*listRandom)->end(), p_myrandom);
}

void synchronizeLibrary()
{
    vector<NameCod *> *listDir = getAllDirectories();

    if(listDir->size() == 0)
    {
        fl_beep();
        fl_message_title("Warning");
        fl_message("Please, add at least one directory on the Directory Manager.");
        delete listDir;
        return;
    }

    Fl_Window *window = make_window_loading();
    progress_bar_dir->maximum(listDir->size());
    window->show();

    deleteAllMusics();
    beginTransaction();

    for(int i = 0; i < listDir->size(); i++)
    {
        vector<string> *listFiles = new vector<string>();
        Fl::check();
        travelDirectoryRecursive(listDir->at(i)->name, listFiles);
        progress_bar_file->maximum(listFiles->size());
        for(int j = 0; j < listFiles->size(); j++)
        {
            //cout<<"Dir: "<<i+1<<"/"<<listDir->size()<<" - File: "<<j+1<<"/"<<listFiles->size()<< endl;
            if(FLAG_CANCEL_SYNC) break;
            string filepath = listFiles->at(j).c_str();
            string title = "";
            string artist = "";
            string album = "";

            TagLib::FileRef *f = new TagLib::FileRef(filepath.c_str());
            title = f->tag()->title().to8Bit();
            artist = f->tag()->artist().to8Bit();
            album = f->tag()->album().to8Bit();
            delete(f);

            insertMusic(title, artist, album, filepath);
            progress_bar_file->value(j+1);
            Fl::check();
        }
        listFiles->clear();
        delete listFiles;
        if(FLAG_CANCEL_SYNC) break;

        progress_bar_dir->value(i+1);
    }
    delete listDir;
    Fl::delete_widget(window);

    commitTransaction();
    FLAG_CANCEL_SYNC = false;
}

int stringToInt(string value)
{
    if(value.empty()) return -1;
    return atoi (value.c_str());
}

string intToString(int value)
{
    stringstream out;
    out << value;
    return out.str();
}

float stringToFloat(string value)
{
    if(value.empty()) return -1;
    return atof (value.c_str());
}

string floatToString(float value)
{
    stringstream out;
    out << value;
    return out.str();
}
