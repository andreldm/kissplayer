#include "dao.h"

#include <FL/fl_utf8.h>
#include <string.h>
#include <sqlite3.h>
#include <iostream>
#include <stdlib.h>

#include "constants.h"
#include "util.h"
#include "os_specific.h"

using namespace std;

static sqlite3* db;
static char* ErrMsg;
static sqlite3_stmt* stmt;

void print_errors()
{
    if(ErrMsg) {
        cerr << "SQL Error: " << ErrMsg << endl;
        if(ErrMsg) {
            sqlite3_free(&ErrMsg);
        }
    }
}

Dao::Dao()
{
    db_filepath = "db.s3db";
}

void Dao::open_db()
{
    sqlite3_open(db_filepath.c_str(), &db);
}

void Dao::close_db()
{
    sqlite3_close(db);
}

void Dao::begin_transaction()
{
    open_db();

    sqlite3_exec(db,"BEGIN;", 0, 0, &ErrMsg);
    print_errors();
}

void Dao::commit_transaction()
{
    sqlite3_exec(db,"COMMIT;", 0, 0, &ErrMsg);
    print_errors();

    close_db();
}

int Dao::init()
{
    // FIXME: Avoid instantiation and delete
    OsUtils* osUtils = new OsUtils();
    if(osUtils->get_working_dir(db_filepath) != 0) {
        delete osUtils;
        return -1;
    }
    delete osUtils;

    open_db();
    const char* query;

    query = "CREATE TABLE IF NOT EXISTS [TB_MUSIC] ( \
                [cod] INTEGER PRIMARY KEY ON CONFLICT ABORT AUTOINCREMENT, \
                [title] VARCHAR(255), \
                [artist] VARCHAR(255), \
                [album] VARCHAR(255), \
                [filepath] VARCHAR UNIQUE ON CONFLICT FAIL NOT NULL, \
                [not_found] BOOLEAN NOT NULL DEFAULT (0))";
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

    close_db();
}

string Dao::get_key(string key)
{
    string value;

    sqlite3_prepare_v2(db, "SELECT * FROM TB_CONFIG WHERE key = ?;", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_STATIC);

    if(sqlite3_step(stmt) == SQLITE_ROW) {
        for(int i = 0; i < sqlite3_column_count(stmt); i++) {
            string col = sqlite3_column_name(stmt, i);
            if(col.compare("value") == 0) {
                value = (char*)sqlite3_column_text(stmt, i);
                break;
            }
        }
    }

    sqlite3_finalize(stmt);

    return value;
}

void Dao::set_key(string key, string value)
{
    sqlite3_prepare_v2(db, "INSERT INTO TB_CONFIG VALUES(?, ?);", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_STATIC);

    if(sqlite3_step(stmt) == SQLITE_ERROR) {
        cout << sqlite3_errmsg(db)<< endl;
    }

    sqlite3_finalize(stmt);
}
/**
* Tries to insert a row, in case of constraint(filepath is unique) updates the row.
*/
void Dao::insert_music(Music& music)
{
    sqlite3_prepare_v2(db, "INSERT INTO TB_MUSIC(title, artist, album, filepath) VALUES(?, ?, ?, ?);", -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, music.title.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, music.artist.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, music.album.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, music.filepath.c_str(), -1, SQLITE_STATIC);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if(result == SQLITE_ERROR) {
        cout << sqlite3_errmsg(db) << endl;
    } else if (result == SQLITE_CONSTRAINT) {
        sqlite3_prepare_v2(db, "UPDATE TB_MUSIC SET title = ?, artist = ?, album = ?, not_found = 0 WHERE filepath = ?;", -1, &stmt, NULL);
        sqlite3_bind_text(stmt, 1, music.title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, music.artist.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, music.album.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, music.filepath.c_str(), -1, SQLITE_STATIC);
        result = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if(result == SQLITE_ERROR) {
            cout << sqlite3_errmsg(db) << endl;
        }
    }
}

void Dao::mark_music_not_found()
{
    open_db();
    sqlite3_exec(db,"UPDATE TB_MUSIC SET not_found = 1;", 0, 0, &ErrMsg);
    print_errors();
    close_db();
}

void Dao::delete_music_not_found()
{
    open_db();
    sqlite3_exec(db,"DELETE FROM TB_MUSIC WHERE not_found = 1;", 0, 0, &ErrMsg);
    print_errors();

    sqlite3_exec(db,"VACUUM;", 0, 0, &ErrMsg);
    print_errors();
    close_db();
}

void Dao::insert_directory(const char* directory)
{
    open_db();
    sqlite3_prepare_v2(db, "INSERT INTO TB_DIRECTORY(directory) VALUES(?);", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, directory, -1, SQLITE_STATIC);

    if(sqlite3_step(stmt) == SQLITE_ERROR) {
        cout << sqlite3_errmsg(db)<< endl;
    }

    sqlite3_finalize(stmt);
    close_db();
}

void Dao::delete_directory(int cod)
{
    open_db();
    sqlite3_prepare_v2(db, "DELETE FROM TB_DIRECTORY WHERE cod = ?;", -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, cod);

    if(sqlite3_step(stmt) == SQLITE_ERROR) {
        cout << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
    close_db();
}

void Dao::get_all_music(deque<Music>& listMusic)
{
    listMusic.clear();

    open_db();
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
    close_db();
}

void Dao::search_music(string text, SearchType type, deque<Music>& listMusic)
{
    listMusic.clear();

    open_db();

    string text_prepared = "%";
    text_prepared.append(text);
    text_prepared.append("%");

    switch(type) {
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
        break;
    default:
        sqlite3_prepare_v2(db, "SELECT * FROM TB_MUSIC ORDER BY artist, title;", -1, &stmt, NULL);
    }

    while(sqlite3_step(stmt) == SQLITE_ROW) {
        Music m;
        for(int i = 0; i < sqlite3_column_count(stmt); i++) {
            string col = sqlite3_column_name(stmt, i);

            if(col.compare("cod") == 0)
                m.cod = sqlite3_column_int(stmt, i);
            else if(col.compare("title") == 0)
                m.title = (char*) sqlite3_column_text(stmt, i);
            else if(col.compare("artist") == 0)
                m.artist = (char*) sqlite3_column_text(stmt, i);
            else if(col.compare("album") == 0)
                m.album = (char*) sqlite3_column_text(stmt, i);
            else if(col.compare("filepath") == 0)
                m.filepath = (char*) sqlite3_column_text(stmt, i);
        }

        listMusic.push_back(m);
    }

    sqlite3_finalize(stmt);
    close_db();
}

void Dao::get_directories(deque<COD_VALUE>& dirList)
{
    open_db();
    sqlite3_prepare_v2(db, "SELECT * FROM TB_DIRECTORY", -1, &stmt, NULL);

    while(sqlite3_step(stmt) == SQLITE_ROW) {
        COD_VALUE cv;
        cv.cod = sqlite3_column_int(stmt, 0);
        cv.value = (const char*)sqlite3_column_text(stmt, 1);

        dirList.push_back(cv);
    }

    sqlite3_finalize(stmt);
    close_db();
}
