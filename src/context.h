#ifndef context_h
#define context_h

#include "configuration.h"
#include "dao.h"
#include "os_specific.h"
#include "sound.h"

class Context {
public:
    Configuration*  configuration;
    Dao*            dao;
    OsSpecific*     osSpecific;
    Sound*          sound;

    Context() {
        configuration = new Configuration();
        dao = new Dao();
        sound = new Sound();
        osSpecific = new OsSpecific();
    }

    ~Context() {
        sound->destroy();
        osSpecific->end();

        delete configuration;
        delete dao;
        delete sound;
        delete osSpecific;
    }
};

#endif
