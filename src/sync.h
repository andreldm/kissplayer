#ifndef sync_h
#define sync_h

#include "context.h"
#include "window_loading.h"

class Sync
{
private:
    Context* context;
    WindowLoading* windowLoading;

public:
    Sync(Context* context);
    void    execute    (bool do_not_warn = false);
};

#endif
