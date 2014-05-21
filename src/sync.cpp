#include "sync.h"

#include <deque>

#include <taglib/fileref.h>

#include <FL/fl_ask.H>

#include "dao.h"
#include "locale.h"
#include "os_specific.h"
#include "util.h"
#include "window_loading.h"
#include "window_main.h"

using namespace std;

/**
* Seeks directories for music files and adds them to the DB.
*/
void sync_execute(bool do_not_warn)
{
    deque<COD_VALUE> listDir;
    dao_get_directories(listDir);

    if(!do_not_warn && listDir.size() == 0) {
        fl_beep();
        fl_message_title(_("Warning"));
        fl_message(_("Please, add at least one directory on the Settings Window."));
        listDir.clear();
        return;
    }

    window_main_stop();
    window_loading_show();
    window_loading_set_dir_max(listDir.size());

    dao_mark_music_not_found();

    dao_begin_transaction();

    for(int i = 0; i < listDir.size(); i++) {
        Fl::check();
#ifdef WIN32
        deque<wstring> listFiles;
        wchar_t dir[PATH_LENGTH];
        fl_utf8towc(listDir.at(i).value.c_str(), listDir.at(i).value.size(), dir, PATH_LENGTH);
#else
        deque<string> listFiles;
        const char* dir = listDir.at(i).value.c_str();
#endif

        os_specific_scanfolder(dir, listFiles);
        window_loading_set_file_max(listFiles.size());

        for(int j = 0; j < listFiles.size(); j++) {
            //cout<<"Dir: "<<i+1<<"/"<<listDir.size()<<" - File: "<<j+1<<"/"<<listFiles.size()<< endl;
            if(FLAG_CANCEL_SYNC) break;

#ifdef WIN32
            const wchar_t* filepath = listFiles.at(j).c_str();
#else
            const char* filepath = listFiles.at(j).c_str();
#endif

            Music m;
            TagLib::FileRef* f = new TagLib::FileRef(filepath);
            if(!f->isNull()) {
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

            dao_insert_music(m);
            window_loading_set_file_value(j + 1);
            Fl::check();
        }
        listFiles.clear();
        if(FLAG_CANCEL_SYNC) break;

        window_loading_set_dir_value(i + 1);
    }

    window_loading_close();

    dao_commit_transaction();

    dao_delete_music_not_found();

    FLAG_CANCEL_SYNC = false;

    window_main_search();
}
