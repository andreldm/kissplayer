#include "util.h"

#include <sstream>

#include <FL/Fl.H>
#include <FL/filename.H>

#include <taglib/fileref.h>

// #if defined WIN32
//     #include <time.h>
//     #include <windows.h>
// #else
//     #include <stdlib.h>
//     #include <curl/curl.h>
//     static CURL* curl = curl_easy_init();
// #endif

using namespace std;

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
string util_format_time (int time)
{
    stringstream stream;

    time = (int) time / 1000;

    int minutes = (int) (time / 60);
    int seconds = (int) (time % 60);

    stream << (minutes <= 9 ? "0" : "") << minutes << ":" << (seconds <= 9 ? "0" : "") << seconds;

    return stream.str();
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
    if(max < 1) return;

    ptrdiff_t (*p_myrandom)(ptrdiff_t) = myrandom;
    srand((int)time(NULL));
    for(int i = 0; i < max; i++) {
        listRandom.push_back(i);
    }

    random_shuffle(listRandom.begin(), listRandom.end(), p_myrandom);
}

void util_replace_all(string& str, const string& from, const string& to)
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void util_erase_between(string& str, const string& start, const string& end)
{
    size_t start_pos = 0;
    size_t end_pos = 0;
    while((start_pos = str.find(start, start_pos)) != string::npos) {
        end_pos = str.find(end, start_pos);
        if(end_pos != string::npos) {
            str.erase(start_pos, end_pos - start_pos + 1);
        } else {
            str.replace(start_pos, start.length(), "");
        }
    }
}

void util_adjust_width(Fl_Widget* w, int padding)
{
  int ww = 0, hh = 0;
  w->measure_label(ww, hh);
  w->size(ww + padding, w->h());
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

size_t util_write_string(void* ptr, size_t size, size_t count, void* stream)
{
    ((string*)stream)->append((char*)ptr, 0, size* count);
    return size* count;
}

void util_uppercase_initials(string& str)
{
    for (uint i = 0; i < str.length(); i++) {
        if (i == 0 && islower(str[i])) {
            str[i] = toupper(str[i]);
            continue;
        }

        if(str[i-1] && str[i-1] == '_' && islower(str[i])) {
            str[i] = toupper(str[i]);
            continue;
        }
    }
}

/**
* Place the window at the center of the parent window or screen
*/
void util_center_window(Fl_Widget* window, Fl_Widget* parent)
{
    int x = 0, y = 0;

    if(parent) {
        x = parent->x() + (parent->w() / 2) - (window->w() / 2);
        y = parent->y() + (parent->h() / 2) - (window->h() / 2);
    } else {
        x = (Fl::w() / 2) - (window->w() / 2);
        y = (Fl::h() / 2) - (window->h() / 2);
    }

    window->resize(x, y, window->w(), window->h());
}
