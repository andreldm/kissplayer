#ifndef lyrics_fetcher_h
#define lyrics_fetcher_h

#include <FL/Fl.H>
#include <FL/Fl_Help_View.H>
#include <string>
#include <curl/curl.h>

using namespace std;

void fetch_lyrics(Fl_Help_View *lyrics_pane, string artist, string title);
#endif
