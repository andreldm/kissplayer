#ifndef dao_h
#define dao_h

#define DB_NAME "db.s3db"

#include <sqlite3.h>
#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include "music.h"
#include "name_cod.h"
#include "misc.h"

using namespace std;

extern sqlite3 *db;

extern void openDB();
extern void closeDB();
extern void startDB();
extern void beginTransaction();
extern void commitTransaction();
extern void insertMusic(string title, string artist, string album, string filepath);
extern void deleteAllMusics();
extern void insertDirectory(const char *directory);
extern void deleteDirectory(int cod);
extern vector<Music> *getAllMusics();
extern vector<NameCod *> *getAllDirectories();
extern vector<Music> *searchMusics(const char *text);

//GLOBALS
extern int FLAG_SEARCH_TYPE;

#endif
