#ifndef configuration_h
#define configuration_h

#include <string>
#include <FL/Fl_Window.H>

#include "dao.h"

struct ConfigData {
    int x, y, w, h;
    bool isWindowMaximized;
    int browserMusicWidth;
    int volumeLevel;
    std::string lastSearch;
    int searchType;
    int musicIndex;
    int musicIndexRandom;
};

class Configuration
{
private:
    static Configuration *_instance;

    bool _shouldMaximizeWindow = false;
    bool _shouldRandomize = false;
    bool _shouldRepeatSong = false;
    bool _shouldFetchLyrics = true;
    bool _shouldScrollTitle = true;
    bool _isCancelSync = false;
    bool _isListChanged;

    std::string _lastSearch;

    Fl_Color _background;
    Fl_Color _foreground;
    Fl_Color _textcolor;

    Configuration();

public:
    static Configuration *instance();

    void         save                    (ConfigData, Dao*);
    ConfigData   load                    (Dao*);

    bool         shouldMaximizeWindow    (void);
    void         shouldMaximizeWindow    (bool v);
    bool         shouldRandomize         (void);
    void         shouldRandomize         (bool v);
    bool         shouldRepeatSong        (void);
    void         shouldRepeatSong        (bool v);
    bool         shouldFetchLyrics       (void);
    void         shouldFetchLyrics       (bool v);
    bool         shouldScrollTitle       (void);
    void         shouldScrollTitle       (bool v);
    bool         isCancelSync            (void);
    void         isCancelSync            (bool v);
    bool         isListChanged           (void);
    void         isListChanged           (bool v);

    std::string  lastSearch              (void);
    void         lastSearch              (std::string v);

    Fl_Color     background              (void);
    void         background              (Fl_Color v);
    Fl_Color     foreground              (void);
    void         foreground              (Fl_Color v);
    Fl_Color     textcolor               (void);
    void         textcolor               (Fl_Color v);
};

#endif
