#ifndef lyrics_fetcher_h
#define lyrics_fetcher_h

#include <string>
#include <FL/Fl_Text_Buffer.H>

void    lyrics_fetcher_run      (Fl_Text_Buffer* lyrics_text_buffer, std::string artist, std::string title);

#endif
