#include "dao.h"

#include <sqlite3.h>
#include <iostream>
#include <stdlib.h>
#include "util.h"
#include "os_specific.h"

using namespace std;

static string DB_NAME = "db.s3db";

static sqlite3* db;
static char* ErrMsg;
static sqlite3_stmt *stmt;

void print_errors()
{
    if(ErrMsg) {
        cout << "SQL Error: " << ErrMsg << endl;
        sqlite3_free(&ErrMsg);
    }
}

void dao_open_db()
{
    sqlite3_open(DB_NAME.c_str(), &db);
}

void dao_close_db()
{
    sqlite3_close(db);
}

void dao_begin_transaction()
{
    dao_open_db();

    sqlite3_exec(db,"BEGIN;", 0, 0, &ErrMsg);
    print_errors();
}

void dao_commit_transaction()
{
    sqlite3_exec(db,"COMMIT;", 0, 0, &ErrMsg);
    print_errors();

    dao_close_db();
}

void dao_start_db()
{
    DB_NAME = getWorkingDirectory();
    DB_NAME.append("db.s3db");

    dao_open_db();
    const char *query;

    query = "CREATE TABLE IF NOT EXISTS [TB_MUSIC] ( \
                [cod] INTEGER PRIMARY KEY ON CONFLICT ABORT AUTOINCREMENT, \
                [title] VARCHAR(255), \
                [artist] VARCHAR(255), \
                [album] VARCHAR(255), \
                [filepath] VARCHAR UNIQUE ON CONFLICT IGNORE NOT NULL)";
    sqlite3_exec(db, query, 0, 0, &ErrMsg);
    print_errors();

    query = "CREATE TABLE IF NOT EXISTS [TB_DIRECTORY] ( \
                [cod] INTEGER PRIMARY KEY ON CONFLICT ABORT AUTOINCREMENT, \
                [directory] VARCHAR UNIQUE ON CONFLICT IGNORE NOT NULL)";
    sqlite3_exec(db, query, 0, 0, &ErrMsg);
    print_errors();

    query = "CREATE TABLE IF NOT EXISTS [TB_CONFIG] ( \
                [key] VARCHAR UNIQUE ON CONFLICT REPLACE NOT NULL, \
                [value] VARCHAR)";
    sqlite3_exec(db, query, 0, 0, &ErrMsg);
    print_errors();

    dao_close_db();
}

string dao_get_key(string key)
{
    string value;

    sqlite3_prepare_v2(db, "SELECT * FROM TB_CONFIG WHERE key = ?;", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_STATIC);

    if(sqlite3_step(stmt) == SQLITE_ROW) {
        for(int i = 0; i < sqlite3_column_count(stmt); i++) {
            string col = sqlite3_column_name(stmt, i);
            if(col.compare("value") == 0) {
                value = (char *)sqlite3_column_text(stmt, i);
                break;
            }
        }
    }

    sqlite3_finalize(stmt);

    return value;
}

void dao_set_key(string key, string value)
{
    sqlite3_prepare_v2(db, "INSERT INTO TB_CONFIG VALUES(?, ?);", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_STATIC);

    if(sqlite3_step(stmt) == SQLITE_ERROR) {
        cout << sqlite3_errmsg(db)<< endl;
    }

    sqlite3_finalize(stmt);
}

void dao_insert_music(string title, string artist, string album, string filepath)
{
    string temp;
    string temp2;
    int foundSlash;
    int foundDot;
    int foundHyphen;

    sqlite3_prepare_v2(db, "INSERT INTO TB_MUSIC(title, artist, album, filepath) VALUES(?, ?, ?, ?);", -1, &stmt, NULL);

    if(!title.empty()) {
        sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_STATIC);
    } else {
        foundSlash = filepath.find_last_of("/\\");
        temp = filepath.substr(foundSlash+1);

        foundDot = temp.find_last_of(".");
        temp = temp.substr(0, foundDot);

        foundHyphen = temp.find_last_of("-");
        temp = temp.substr(foundHyphen+1);
        trim(temp);
        sqlite3_bind_text(stmt, 1, temp.c_str(), -1, SQLITE_STATIC);
    }

    if(!artist.empty()) {
        sqlite3_bind_text(stmt, 2, artist.c_str(), -1, SQLITE_STATIC);
    } else {
        foundSlash = filepath.find_last_of("/\\");
        temp2 = filepath.substr(foundSlash+1);

        foundDot = temp2.find_last_of(".");
        temp2 = temp2.substr(0, foundDot);

        foundHyphen = temp2.find_last_of("-");
        temp2 = temp2.substr(0, foundHyphen);
        trim(temp2);
        sqlite3_bind_text(stmt, 2, temp2.c_str(), -1, SQLITE_STATIC);
    }


    sqlite3_bind_text(stmt, 3, album.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, filepath.c_str(), -1, SQLITE_STATIC);

    if(sqlite3_step(stmt) == SQLITE_ERROR) {
        cout << sqlite3_errmsg(db)<< endl;
    }

    sqlite3_finalize(stmt);
}

void dao_clear_all_music()
{
    sqlite3_exec(db,"DELETE FROM TB_MUSIC;", 0, 0, &ErrMsg);
    print_errors();

    sqlite3_exec(db,"VACUUM;", 0, 0, &ErrMsg);
    print_errors();

    dao_close_db();
}

void dao_insert_directory(const char* directory)
{
    dao_open_db();
    sqlite3_prepare_v2(db, "INSERT INTO TB_DIRECTORY(directory) VALUES(?);", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, directory, -1, SQLITE_STATIC);

    if(sqlite3_step(stmt) == SQLITE_ERROR) {
        cout << sqlite3_errmsg(db)<< endl;
    }

    sqlite3_finalize(stmt);
    dao_close_db();
}

void dao_delete_directory(int cod)
{
    dao_open_db();
    sqlite3_prepare_v2(db, "DELETE FROM TB_DIRECTORY WHERE cod = ?;", -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, cod);

    if(sqlite3_step(stmt) == SQLITE_ERROR) {
        cout << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    dao_close_db();
}

void dao_get_all_music(deque<Music>& listMusic)
{
    listMusic.clear();

    dao_open_db();
    sqlite3_prepare_v2(db, "SELECT * FROM TB_MUSIC ORDER BY artist, title;", -1, &stmt, NULL);
    while(sqlite3_step(stmt) == SQLITE_ROW) {
        Music m;
        for(int i = 0; i < sqlite3_column_count(stmt); i++) {
            string col = sqlite3_column_name(stmt, i);

            if(col.compare("cod") == 0) {
                m.cod = sqlite3_column_int(stmt, i);
                continue;
            }
            if(col.compare("title") == 0) {
                m.title = (char*)sqlite3_column_text(stmt, i);
                continue;
            }
            if(col.compare("artist") == 0) {
                m.artist = (char*)sqlite3_column_text(stmt, i);
                continue;
            }
            if(col.compare("album") == 0) {
                m.album = (char*)sqlite3_column_text(stmt, i);
                continue;
            }
            if(col.compare("filepath") == 0) {
                m.filepath = (char*)sqlite3_column_text(stmt, i);
                continue;
            }
        }
        listMusic.push_back(m);
    }
    sqlite3_finalize(stmt);
    dao_close_db();
}

void dao_search_music(const char* text, deque<Music>& listMusic)
{
    listMusic.clear();

    dao_open_db();

    // On Windows we need to convert from CP-1252 to UTF-8
    // TODO: Use fltk unicode functions
#if defined WIN32
    wchar_t *wText = CodePageToUnicode(65001, text);
    text = UnicodeToCodePage(1252, wText);
#endif

    string text_prepared = "%";
    text_prepared.append(text);
    text_prepared.append("%");

    switch(FLAG_SEARCH_TYPE) {
    case SEARCH_TYPE_ALL:
        sqlite3_prepare_v2(db, "SELECT * FROM TB_MUSIC WHERE title LIKE ? OR artist LIKE ? OR album LIKE ? ORDER BY artist, title;", -1, &stmt, NULL);
        sqlite3_bind_text(stmt, 1, text_prepared.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, text_prepared.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, text_prepared.c_str(), -1, SQLITE_STATIC);
        break;
    case SEARCH_TYPE_TITLE:
        sqlite3_prepare_v2(db, "SELECT * FROM TB_MUSIC WHERE title LIKE ? ORDER BY artist, title;", -1, &stmt, NULL);
        sqlite3_bind_text(stmt, 1, text_prepared.c_str(), -1, SQLITE_STATIC);
        break;
    case SEARCH_TYPE_ARTIST:
        sqlite3_prepare_v2(db, "SELECT * FROM TB_MUSIC WHERE artist LIKE ? ORDER BY artist, title;", -1, &stmt, NULL);
        sqlite3_bind_text(stmt, 1, text_prepared.c_str(), -1, SQLITE_STATIC);
        break;
    case SEARCH_TYPE_ALBUM:
        sqlite3_prepare_v2(db, "SELECT * FROM TB_MUSIC WHERE album LIKE ? ORDER BY artist, title;", -1, &stmt, NULL);
        sqlite3_bind_text(stmt, 1, text_prepared.c_str(), -1, SQLITE_STATIC);
    default:
         sqlite3_prepare_v2(db, "SELECT * FROM TB_MUSIC ORDER BY artist, title;", -1, &stmt, NULL);
    }

    while(sqlite3_step(stmt) == SQLITE_ROW) {
        Music m;
        for(int i = 0; i < sqlite3_column_count(stmt); i++) {
            string col = sqlite3_column_name(stmt, i);

            if(col.compare("cod") == 0) {
                m.cod = sqlite3_column_int(stmt, i);
                continue;
            }
            if(col.compare("title") == 0) {
                m.title = (char *)sqlite3_column_text(stmt, i);
                continue;
            }
            if(col.compare("artist") == 0) {
                m.artist = (char *)sqlite3_column_text(stmt, i);
                continue;
            }
            if(col.compare("album") == 0) {
                m.album = (char *)sqlite3_column_text(stmt, i);
                continue;
            }
            if(col.compare("filepath") == 0) {
                m.filepath = (char *)sqlite3_column_text(stmt, i);
                continue;
            }
        }
        listMusic.push_back(m);
    }
    sqlite3_finalize(stmt);
    dao_close_db();
}

void dao_get_directories(deque<COD_VALUE>& dirList)
{
    dao_open_db();
    sqlite3_prepare_v2(db, "SELECT * FROM TB_DIRECTORY", -1, &stmt, NULL);

    while(sqlite3_step(stmt) == SQLITE_ROW) {
        COD_VALUE cv;
        cv.cod = sqlite3_column_int(stmt, 0);
        cv.value = (const char *)sqlite3_column_text(stmt, 1);

        dirList.push_back(cv);
    }

    sqlite3_finalize(stmt);
    dao_close_db();
}
