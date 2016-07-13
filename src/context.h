#ifndef context_h
#define context_h

#include "configuration.h"
#include "dao.h"
#include "locale.h"
#include "os_specific.h"
#include "sound.h"

class Context {
public:
    Configuration*  configuration;
    Dao*            dao;
    Locale*         locale;
    OsSpecific*     osSpecific;
    Sound*          sound;

    Context() {
        configuration = new Configuration();
        dao = new Dao();
        locale = new Locale();
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
