#ifndef util_h
#define util_h

#include <string>
#include <vector>
#include <deque>
#include "music.h"

#define     SEARCH_TYPE_ALL             0
#define     SEARCH_TYPE_TITLE           1
#define     SEARCH_TYPE_ARTIST          2
#define     SEARCH_TYPE_ALBUM           3

#define     KISS_MAJOR_VERSION          0
#define     KISS_MINOR_VERSION          5
#define     KISS_PATCH_VERSION          9

#define     DEFAULT_BACKGROUND_COLOR    0xDDEEFF00

void            parseArgs           (int argc, char** argv, std::deque<Music>& listMusic);
void            trim                (std::string& str);
const char*     formatTime          (int secs);
void            randomize           (std::vector<int>**, int);
void            misc_sync_library   (void);

int             stringToInt         (std::string value);
std::string     intToString         (int value);
float           stringToFloat       (std::string value);
std::string     floatToString       (float value);

// GLOBAL
extern bool FLAG_CANCEL_SYNC;

#endif
