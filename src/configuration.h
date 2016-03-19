#ifndef configuration_h
#define configuration_h

#include <string>
#include <FL/Fl_Window.H>

#include "dao.h"

struct SaveData {
    int x, y, w, h;
    bool is_window_maximized;
    int browser_music_width;
    int volume_level;
    std::string last_search;
    int search_type;
    int music_index;
    int music_index_random;
};

class Configuration
{
private:
    static Configuration *_instance;

    bool _shouldMaximizeWindow;
    bool _shouldRandomize;
    bool _shouldRepeatSong;
    bool _shouldFetchLyrics;
    bool _shouldScrollTitle;
    bool _isCancelSync;
    bool _isListChanged;

    Fl_Color _background;
    Fl_Color _foreground;
    Fl_Color _textcolor;

    Configuration();

public:
    static Configuration *instance();

    void     save                    (SaveData, Dao*);
    void     load                    (Dao*);

    bool     shouldMaximizeWindow    (void);
    void     shouldMaximizeWindow    (bool v);
    bool     shouldRandomize         (void);
    void     shouldRandomize         (bool v);
    bool     shouldRepeatSong        (void);
    void     shouldRepeatSong        (bool v);
    bool     shouldFetchLyrics       (void);
    void     shouldFetchLyrics       (bool v);
    bool     shouldScrollTitle       (void);
    void     shouldScrollTitle       (bool v);
    bool     isCancelSync            (void);
    void     isCancelSync            (bool v);
    bool     isListChanged           (void);
    void     isListChanged           (bool v);

    Fl_Color background              (void);
    void     background              (Fl_Color v);
    Fl_Color foreground              (void);
    void     foreground              (Fl_Color v);
    Fl_Color textcolor               (void);
    void     textcolor               (Fl_Color v);
};

#endif
