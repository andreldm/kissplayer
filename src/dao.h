#ifndef dao_h
#define dao_h

#include <string>
#include <deque>

#include "constants.h"
#include "music.h"
#include "os_specific.h"

class COD_VALUE
{
public:
    int cod;
    std::string value;
};

class Dao
{
public:
    int             init                        (OsSpecific*);
    void            open_db                     (void);
    void            close_db                    (void);
    void            begin_transaction           (void);
    void            commit_transaction          (void);

    std::string     get_key                     (std::string key);
    std::string     open_get_key                (std::string key);
    int             get_key_int                 (std::string key);
    int             open_get_key_int            (std::string key);

    void            set_key                     (std::string key, std::string value);
    void            open_set_key                (std::string key, std::string value);
    void            set_key_int                 (std::string key, int value);

    void            insert_music                (Music& music);
    void            mark_music_not_found        (void);
    void            delete_music_not_found      (void);
    void            get_all_music               (std::deque<Music>& listMusic);
    void            search_music                (std::string text, SearchType type, std::deque<Music>& listMusic);

    void            insert_directory            (std::string& dir);
    void            delete_directory            (int cod);
    void            get_directories             (std::deque<COD_VALUE>& dirList);
};

#endif
