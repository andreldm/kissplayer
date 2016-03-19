#ifndef dao_h
#define dao_h

#include <string>
#include <deque>

#include "constants.h"
#include "music.h"

class COD_VALUE
{
public:
    int cod;
    std::string value;
};

class Dao
{
private:
    std::string     db_filepath;

public:
    Dao                                         (void);

    int             init                        (void);
    void            open_db                     (void);
    void            close_db                    (void);
    void            begin_transaction           (void);
    void            commit_transaction          (void);

    void            set_key                     (std::string key, std::string value);
    std::string     get_key                     (std::string key);

    void            insert_music                (Music& music);
    void            mark_music_not_found        (void);
    void            delete_music_not_found      (void);
    void            get_all_music               (std::deque<Music>& listMusic);
    void            search_music                (std::string text, SearchType type, std::deque<Music>& listMusic);

    void            insert_directory            (const char* directory);
    void            delete_directory            (int cod);
    void            get_directories             (std::deque<COD_VALUE>& dirList);
};

#endif
