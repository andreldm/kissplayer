#include "locale.h"

#include <FL/Fl.H>
#include <FL/fl_utf8.h>

#include <unistd.h>

#include "constants.h"
#include "util.h"
#include "os_specific.h"

#define KEY_LANGUAGE "lang"
#define KEY_LANGUAGE_INDEX "lang_index"

using namespace std;

static Language* languages[] = {
    new Language("System Default", ""),
    new Language("Čeština", "cs.UTF-8"),
    new Language("Deutsch", "de.UTF-8"),
    new Language("English", "en.UTF-8"),
    new Language("ελληνικά", "el.UTF-8"),
    new Language("Español", "es.UTF-8"),
    new Language("Italiano", "it.UTF-8"),
    new Language("Português (Brasil)", "pt_BR.UTF-8"),
    new Language("Türkçe", "tr.UTF-8"),
    new Language("中文 (中国)", "zh_CN.UTF-8"),
    NULL
};

void Locale::init(Dao* dao, int argc, char** argv)
{
    this->dao = dao;

#ifdef WIN32
    string language = dao->open_get_key(KEY_LANGUAGE);

    char str[50];
    sprintf(str, "%s=%s", "LC_ALL", language.c_str());
    putenv(str);
    bindtextdomain("kissplayer", ".\\locale");
    bind_textdomain_codeset("kissplayer", "UTF-8");
#elif __linux__
    bindtextdomain("kissplayer", LOCALEDIR);
    setlocale(LC_ALL, "");

    // Set font if -f argument is present
    for (int opt; (opt = getopt(argc, argv, "f:")) != EOF;) {
        if (opt == 'f') {
            Fl::set_font(FL_HELVETICA, optarg);
            break;
        }
    }
#endif

    textdomain("kissplayer");
}

void Locale::setLanguage(int index)
{
    const char* choice = languages[index]->locale_name.c_str();

    dao->open_db();
    dao->set_key(KEY_LANGUAGE, choice);
    dao->set_key_int(KEY_LANGUAGE_INDEX, index);
    dao->close_db();
}

Language** Locale::getDefinedLanguages()
{
    return languages;
}
