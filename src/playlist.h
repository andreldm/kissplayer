#ifndef playlist_h
#define playlist_h

#include <deque>

#include "constants.h"
#include "context.h"
#include "music.h"
#include "widget/ksp_browser.h"

class Playlist {
private:
    Context* context;
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
              Playlist            (Context* context, KSP_Browser* browser_music);

    bool      parse_args          (int argc, char** argv);
    void      parse_dnd           (std::string urls);
    void      search              (std::string text, SearchType type);
    bool      hasNext             (void);
    bool      hasPrevious         (void);
    bool      play                (float volume, bool playAtBrowser = false);
    bool      stop                (void);
    void      next                (void);
    void      previous            (void);
    bool      isEmpty             (void);
    Music     getCurrentMusic     (void);

    int       getMusicIndex       (void)  { return musicIndex; }
    void      setMusicIndex       (int v) { musicIndex = v; }
    int       getMusicIndexRandom (void)  { return musicIndexRandom; }
    void      setMusicIndexRandom (int v) { musicIndexRandom = v; }
};

#endif
