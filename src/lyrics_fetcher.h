#ifndef lyrics_fetcher_h
#define lyrics_fetcher_h

#include <string>
#include <FL/Fl_Text_Buffer.H>

#include "dao.h"
#include "music.h"
#include "sound.h"

// class LyricsData;

class LyricsFetcher
{
private:
    Dao* dao;
    Sound* sound;
    Fl_Text_Buffer* text_buffer;

public:
    LyricsFetcher(Dao*, Sound*, Fl_Text_Buffer*);

    void fetch (Music* music);
};

// GLOBAL IMPORT
extern bool     FLAG_LYRICS;

#endif
