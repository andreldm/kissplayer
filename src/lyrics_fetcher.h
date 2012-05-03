#ifndef lyrics_fetcher_h
#define lyrics_fetcher_h

#include <FL/Fl.H>
#include <FL/Fl_Text_Buffer.H>
#include <curl/curl.h>
#include <string>

using namespace std;

void fetch_lyrics(Fl_Text_Buffer *lyrics_text_buffer, string artist, string title);
#endif
