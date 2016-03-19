#ifndef util_h
#define util_h

#include <string>
#include <deque>

#include <FL/Fl_Widget.H>

#include "music.h"

bool            util_parse_dnd          (std::string urls, std::deque<Music>& listMusic);
bool            util_is_ext_supported   (std::string filename);
void            util_trim               (std::string& str);
std::string     util_format_time        (int secs);
void            util_randomize          (std::deque<int>&, int);
void            util_replace_all        (std::string& str, const std::string& from, const std::string& to);
void            util_erase_between      (std::string& str, const std::string& start, const std::string& end);
void            util_adjust_width       (Fl_Widget* w, int padding = 6);
size_t          util_write_string       (void* ptr, size_t size, size_t count, void* stream);
void            util_uppercase_initials (std::string& str);

int             util_s2i                (std::string value);
std::string     util_i2s                (int value);
float           util_s2f                (std::string value);
std::string     util_f2s                (float value);

// GLOBAL IMPORT
extern bool FLAG_CANCEL_SYNC;

#endif
