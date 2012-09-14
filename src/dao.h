#ifndef dao_h
#define dao_h

#include <sqlite3.h>
#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include "music.h"
#include "name_cod.h"
#include "misc.h"

#ifdef WIN32
#include "win/windows_specific.h"
#endif

#ifdef __linux__
#include "linux/linux_specific.h"
#endif

using namespace std;

void openDB();
void closeDB();
void startDB();
void beginTransaction();
void commitTransaction();
void setKey(string key, string value);
string getKey(string key);
void insertMusic(string title, string artist, string album, string filepath);
void deleteAllMusics();
void insertDirectory(const char *directory);
void deleteDirectory(int cod);
vector<Music> *getAllMusics();
vector<NameCod *> *getAllDirectories();
vector<Music> *searchMusics(const char *text);

//GLOBALS
extern int FLAG_SEARCH_TYPE;

#endif
