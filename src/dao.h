#ifndef dao_h
#define dao_h

#include <string>
#include <deque>
#include "music.h"

extern int FLAG_SEARCH_TYPE;

class COD_VALUE
{
public:
    int cod;
    std::string value;
};

void                    dao_open_db             (void);
void                    dao_close_db            (void);
void                    dao_start_db            (void);
void                    dao_begin_transaction   (void);
void                    dao_commit_transaction  (void);

void                    dao_set_key             (std::string key, std::string value);
std::string             dao_get_key             (std::string key);

void                    dao_insert_music        (std::string title, std::string artist, std::string album, std::string filepath);
void                    dao_clear_all_music     (void);
void                    dao_get_all_music       (std::deque<Music>& listMusic);
void                    dao_search_music        (const char* text, std::deque<Music>& listMusic);

void                    dao_insert_directory    (const char* directory);
void                    dao_delete_directory    (int cod);
void                    dao_get_directories     (std::deque<COD_VALUE>& dirList);

#endif
