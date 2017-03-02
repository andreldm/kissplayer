#ifndef lyrics_fetcher_h
#define lyrics_fetcher_h

#include <string>
#include <FL/Fl_Text_Buffer.H>

#include "context.h"
#include "music.h"

class LyricsFetcher
{
private:
    Context* context;
    Fl_Text_Buffer* text_buffer;

public:
    LyricsFetcher(Context*, Fl_Text_Buffer*);

    void fetch (Music* music);
};

#endif
