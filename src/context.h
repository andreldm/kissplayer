#ifndef context_h
#define context_h

#include "configuration.h"
#include "dao.h"
#include "os_specific.h"
#include "sound.h"

struct Context {
    Configuration* configuration;
    OsSpecific* osSpecific;
    Dao* dao;
    Sound* sound;
};

#endif
