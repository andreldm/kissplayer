#include "util.h"

#include <sstream>
#include <map>
#include <deque>

#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/filename.H>

#include <taglib/taglib.h>
#include <taglib/fileref.h>
#include <taglib/tstring.h>
#include <taglib/tag.h>

#include "dao.h"
#include "os_specific.h"
#include "window_loading.h"

#if defined WIN32
    #include <time.h>
    #include <windows.h>
#else
    #include <stdlib.h>
#endif

using namespace std;

#ifndef WIN32
#include <curl/curl.h>
static CURL* curl = curl_easy_init();
#endif

#define PATH_LENGTH 8192

/**
* Parses the arguments to check if there are music files to play
*/
void util_parse_args(int argc, char** argv, deque<Music>& listMusic)
{
    for(int i = 1; i < argc; i++) {
        string arg(argv[i]);

        if(util_is_ext_supported(arg)) {
            Music m;
            TagLib::FileRef* f = new TagLib::FileRef(arg.c_str());
            if(!f->isNull()) {
                m.title = f->tag()->title().toCString();
                m.artist = f->tag()->artist().toCString();
                m.album = f->tag()->album().toCString();
                m.filepath = arg;
                listMusic.push_back(m);
            }
            delete(f);
        }
    }
}

/**
* Parses the drag and drop url list
*/
bool util_parse_dnd(string urls, deque<Music>& listMusic)
{
    istringstream lines(urls);
    string url;
    bool list_changed = false;

    while (getline(lines, url)) {
        if(util_is_ext_supported(url)) {
#ifdef WIN32
            wchar_t filepath[PATH_LENGTH];
            fl_utf8towc(url.c_str(), url.size(), filepath, PATH_LENGTH);
#else
            char* url_unescaped = curl_easy_unescape(curl , url.c_str(), 0, NULL);
            url = url_unescaped;
            util_replace_all(url, "file://", "");
            curl_free(url_unescaped);
            const char* filepath = url.c_str();
#endif
            Music m;
            TagLib::FileRef* f = new TagLib::FileRef(filepath);
            if(!f->isNull()) {
                m.title = f->tag()->title().toCString();
                m.artist = f->tag()->artist().toCString();
                m.album = f->tag()->album().toCString();
                m.filepath = url;

                if(!list_changed) {
                    listMusic.clear();
                    list_changed = true;
                }
                listMusic.push_back(m);
            }
            delete(f);
        }
    }

    return list_changed;
}

bool util_is_ext_supported(string filename)
{
    const char* ext = fl_filename_ext(filename.c_str());

    if(strcmp(ext, ".mp3") == 0) return true;
    if(strcmp(ext, ".wma") == 0) return true;
    if(strcmp(ext, ".ogg") == 0) return true;
    if(strcmp(ext, ".wav") == 0) return true;
    if(strcmp(ext, ".flac") == 0) return true;

    return false;
}

/**
* Removes whitespaces at the beginning and end of a string
*/
void util_trim(string &str)
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
const char* util_format_time (int time)
{
    stringstream stream;

    time = (int) time / 1000;

    int minutes = (int) (time / 60);
    int seconds = (int) (time % 60);

    stream << (minutes <= 9 ? "0" : "") << minutes << ":" << (seconds <= 9 ? "0" : "") << seconds;

    return stream.str().c_str();
}

/**
* Generates random numbers
*/
ptrdiff_t myrandom (ptrdiff_t i)
{
    return rand()%i;
}

/**
* Generates a vector filled with unique random numbers
*/
void util_randomize(deque<int>& listRandom, int max)
{
    listRandom.clear();

    ptrdiff_t (*p_myrandom)(ptrdiff_t) = myrandom;
    srand((int)time(NULL));
    for(int i = 0; i < max; i++) {
        listRandom.push_back(i);
    }

    random_shuffle(listRandom.begin(), listRandom.end(), p_myrandom);
}

/**
* Seeks directories for music files and adds them to the DB.
*/
void util_sync_library()
{
    deque<COD_VALUE> listDir;
    dao_get_directories(listDir);

    if(listDir.size() == 0) {
        fl_beep();
        fl_message_title("Warning");
        fl_message("Please, add at least one directory on the Settings Window.");
        listDir.clear();
        return;
    }

    window_loading_show();
    window_loading_set_dir_max(listDir.size());

    dao_open_db();
    dao_clear_all_music();
    dao_close_db();

    dao_begin_transaction();

    for(int i = 0; i < listDir.size(); i++) {
        Fl::check();
#ifdef WIN32
        deque<wstring> listFiles;
        wchar_t dir[PATH_LENGTH];
        fl_utf8towc(listDir.at(i).value.c_str(), listDir.at(i).value.size(), dir, PATH_LENGTH);
#else
        deque<string> listFiles;
        const char* dir = listDir.at(i).value.c_str();
#endif

        os_specific_scanfolder(dir, listFiles);
        window_loading_set_file_max(listFiles.size());

        for(int j = 0; j < listFiles.size(); j++) {
            //cout<<"Dir: "<<i+1<<"/"<<listDir.size()<<" - File: "<<j+1<<"/"<<listFiles.size()<< endl;
            if(FLAG_CANCEL_SYNC) break;

#ifdef WIN32
            const wchar_t* filepath = listFiles.at(j).c_str();
#else
            const char* filepath = listFiles.at(j).c_str();
#endif

            string title = "";
            string artist = "";
            string album = "";

            TagLib::FileRef* f = new TagLib::FileRef(filepath);
            if(!f->isNull()) {
                title = f->tag()->title().toCString(true);
                artist = f->tag()->artist().toCString(true);
                album = f->tag()->album().toCString(true);
            }
            delete(f);

#ifdef WIN32
            char path[PATH_LENGTH];
            fl_utf8fromwc(path, PATH_LENGTH, filepath, lstrlenW(filepath));
#else
            const char* path = filepath;
#endif

            dao_insert_music(title, artist, album, path);
            window_loading_set_file_value(j+1);
            Fl::check();
        }
        listFiles.clear();
        if(FLAG_CANCEL_SYNC) break;

        window_loading_set_dir_value(i+1);
    }
    window_loading_close();

    dao_commit_transaction();
    FLAG_CANCEL_SYNC = false;
}

void util_replace_all(string& str, const string& from, const string& to)
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

int util_s2i(string value)
{
    if(value.empty()) {
        return -1;
    }
    return atoi (value.c_str());
}

string util_i2s(int value)
{
    stringstream out;
    out << value;
    return out.str();
}

float util_s2f(string value)
{
    if(value.empty()) {
        return -1;
    }
    return atof (value.c_str());
}

string util_f2s(float value)
{
    stringstream out;
    out << value;
    return out.str();
}
