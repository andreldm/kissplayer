#ifndef util_h
#define util_h

#include <string>
#include <deque>

#include <FL/Fl_Widget.H>

#include "music.h"

#define     SEARCH_TYPE_ALL             0
#define     SEARCH_TYPE_TITLE           1
#define     SEARCH_TYPE_ARTIST          2
#define     SEARCH_TYPE_ALBUM           3

#define     KISS_MAJOR_VERSION          0
#define     KISS_MINOR_VERSION          6
#define     KISS_PATCH_VERSION          3

#define     DEFAULT_BACKGROUND_COLOR    0x5A595800
#define     DEFAULT_SELECTION_COLOR     0xFFFFFF00
#define     DEFAULT_FOREGROUND_COLOR    0xC8C8C800

#define     PATH_LENGTH                 8192

void            util_parse_args         (int argc, char** argv, std::deque<Music>& listMusic);
bool            util_parse_dnd          (std::string urls, std::deque<Music>& listMusic);
bool            util_is_ext_supported   (std::string filename);
void            util_trim               (std::string& str);
const char*     util_format_time        (int secs);
void            util_randomize          (std::deque<int>&, int);
void            util_replace_all        (std::string& str, const std::string& from, const std::string& to);
void            util_erease_between     (std::string& str, const std::string& start, const std::string& end);
void            util_adjust_width       (Fl_Widget* w, int padding = 6);

int             util_s2i                (std::string value);
std::string     util_i2s                (int value);
float           util_s2f                (std::string value);
std::string     util_f2s                (float value);

// GLOBAL IMPORT
extern bool FLAG_CANCEL_SYNC;

#endif
