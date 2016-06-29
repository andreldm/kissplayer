#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>

#include "configuration.h"
#include "context.h"
#include "sound.h"
#include "locale.h"
#include "window_main.h"
#include "os_specific.h"
#include "dao.h"

int main(int argc, char** argv)
{
    Fl::scheme("GTK+");

    Fl::background(50, 50, 50);
    Fl::background2(90, 90, 90);
    Fl::foreground(255, 255, 255);

    fl_message_title_default(_("Warning"));

    Context context;
    context.configuration = new Configuration();
    context.dao = new Dao();
    context.sound = new Sound();
    context.osSpecific = new OsSpecific();

    // Locale::init();

    if (context.dao->init() != 0) {
        fl_alert(_("Error while initializing database"));
        return -1;
    }

    if (context.sound->init() != 0) {
        fl_alert(_("Error while sound system"));
        return -1;
    }

    WindowMain windowMain(&context);
    windowMain.init(argc, argv);
    context.osSpecific->set_app_icon(&windowMain);
    windowMain.show(0, NULL);

    context.osSpecific->init(&windowMain);

    // if (FLAG_MAXIMIZE_WINDOW) {
    //     os_specific_maximize_window();
    // }

    // if (os_specific_init() != 0) {
    //     fl_alert(_("Keyboard hooker failed!"));
    //     return -1;
    // }
    //
    // if (sound_initialize() != 0) {
    //     fl_alert(_("Error while initializing sound system!"));
    //     return -1;
    // }

    Fl::lock();

    int fl_result = Fl::run();

    context.sound->destroy();
    context.osSpecific->end();

    return fl_result;
}
