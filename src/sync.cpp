#include "sync.h"

#include <deque>

#include <taglib/fileref.h>

#include <FL/Fl.H>
#include <FL/fl_ask.H>

#include "locale.h"
#include "os_specific.h"
#include "signals.h"
#include "util.h"
#include "window_loading.h"

using namespace std;

Sync::Sync(Context* context)
{
    this->context = context;
}

/**
* Seeks directories for music files and adds them to the DB.
*/
void Sync::execute(bool do_not_warn)
{
    deque<COD_VALUE> listDir;
    context->dao->get_directories(listDir);

    if (!do_not_warn && listDir.empty()) {
        fl_beep();
        fl_message(_("Please, add at least one directory on the Settings Window."));
        listDir.clear();
        return;
    }

    SignalStop.emit();
    WindowLoading* windowLoading = new WindowLoading(context);
    windowLoading->show();
    windowLoading->set_dir_max(listDir.size());

    context->dao->mark_music_not_found();

    context->dao->begin_transaction();

    for (uint i = 0; i < listDir.size(); i++) {
        Fl::check();
#ifdef WIN32
        deque<wstring> listFiles;
        wchar_t dir[PATH_LENGTH];
        fl_utf8towc(listDir.at(i).value.c_str(), listDir.at(i).value.size(), dir, PATH_LENGTH);
#else
        deque<string> listFiles;
        const char* dir = listDir.at(i).value.c_str();
#endif

        context->osSpecific->scanfolder(dir, listFiles);
        windowLoading->set_file_max(listFiles.size());

        for (uint j = 0; j < listFiles.size(); j++) {
            if (context->configuration->isCancelSync()) break;

#ifdef WIN32
            const wchar_t* filepath = listFiles.at(j).c_str();
#else
            const char* filepath = listFiles.at(j).c_str();
#endif

            Music m;
            TagLib::FileRef* f = new TagLib::FileRef(filepath);
            if (!f->isNull()) {
                m.title = f->tag()->title().toCString(true);
                m.artist = f->tag()->artist().toCString(true);
                m.album = f->tag()->album().toCString(true);
            }
            delete(f);

#ifdef WIN32
            char path[PATH_LENGTH];
            fl_utf8fromwc(path, PATH_LENGTH, filepath, lstrlenW(filepath));
#else
            const char* path = filepath;
#endif
            m.filepath = path;
            m.resolveNames();

            context->dao->insert_music(m);
            windowLoading->set_file_value(j + 1);
            Fl::check();
        }
        listFiles.clear();
        if (context->configuration->isCancelSync()) break;

        windowLoading->set_dir_value(i + 1);
    }

    windowLoading->close();
    delete windowLoading;

    context->dao->commit_transaction();

    context->dao->delete_music_not_found();

    context->configuration->isCancelSync(false);

    SignalSearch.emit();
}
