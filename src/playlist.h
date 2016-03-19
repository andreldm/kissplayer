#ifndef playlist_h
#define playlist_h

#include <deque>

#include "constants.h"
#include "dao.h"
#include "music.h"
#include "sound.h"
#include "widget/ksp_browser.h"

class Playlist {
private:
    Dao* dao;
    Sound* sound;
    KSP_Browser* browser_music;
    std::deque<Music> listMusic;
    std::deque<int> listRandom;
    int musicIndex;
    int musicIndexRandom;
    int musicPlayingCod;
    float lastVolume;

    void update_browser         (void);
    void increment_music_index  (void);
    void decrement_music_index  (void);

public:
              Playlist           (Dao* dao, Sound* sound, KSP_Browser* browser_music);

    bool      parse_args         (int argc, char** argv);
    bool      search             (std::string text, SearchType type);
    bool      hasNext            (void);
    bool      hasPrevious        (void);
    bool      play               (float volume, bool playAtBrowser = false);
    bool      stop               (void);
    bool      next               (void);
    bool      previous           (void);
    bool      isEmpty            (void);
    Music     getCurrentMusic    (void);
};

#endif
