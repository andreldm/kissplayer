#ifndef sync_h
#define sync_h

#include "configuration.h"
#include "dao.h"
#include "window_loading.h"
#include "window_main.h"

class Sync
{
private:
    Configuration* config;
    Dao* dao;
    WindowMain* windowMain;
    WindowLoading* windowLoading;

public:
    Sync(Configuration*, WindowMain*);
    void    execute    (bool do_not_warn = false);
};

#endif
