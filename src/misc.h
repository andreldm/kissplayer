#ifndef misc_h
#define misc_h

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <stdlib.h>

#include <FL/Fl.H>
#include <FL/fl_ask.H>

#include <taglib/taglib.h>
#include <taglib/fileref.h>
#include <taglib/tstring.h>
#include <taglib/tag.h>

#include "name_cod.h"
#include "dir_tools.h"
#include "dao.h"
#include "window_loading.h"

#if defined WIN32
#include <time.h>
#else
#include <stdlib.h>
#endif

#ifndef SEARCH_TYPE_ALL
#define SEARCH_TYPE_ALL     0
#endif

#ifndef SEARCH_TYPE_TITLE
#define SEARCH_TYPE_TITLE   1
#endif

#ifndef SEARCH_TYPE_ARTIST
#define SEARCH_TYPE_ARTIST  2
#endif

#ifndef SEARCH_TYPE_ALBUM
#define SEARCH_TYPE_ALBUM   3
#endif

using namespace std;

void trim(string &str);
const char *formatTime(int secs);
void randomize(vector<int> **, int);
void synchronizeLibrary();
int stringToInt(string value);
string intToString(int value);

extern bool     FLAG_CANCEL_SYNC;

#if defined WIN32

#include <windows.h>
wchar_t *CodePageToUnicode(int codePage, const char *src);
char *UnicodeToCodePage(int codePage, const wchar_t *src);

#endif //WIN32

#endif //misc_h
