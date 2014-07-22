#include "locale.h"

#include <FL/fl_utf8.h>

#include "dao.h"
#include "util.h"
#include "os_specific.h"

#define KEY_LANGUAGE "lang"
#define KEY_LANGUAGE_INDEX "lang_index"

using namespace std;

static Language* languages[] = {
    new Language("System Default", ""),
    new Language("English", "en.UTF-8"),
    new Language("Espa�ol", "es.UTF-8"),
    new Language("Italiano", "it.UTF-8"),
    new Language("Portugu�s (Brasil)", "pt_BR.UTF-8"),
    NULL
};

void Locale::init()
{
#ifdef WIN32
    dao_open_db();
    string language = dao_get_key(KEY_LANGUAGE);
    dao_close_db();

    language = "LC_ALL=" + language;
    putenv(language.c_str());
    bindtextdomain("kissplayer", ".\\locale");
#elif __linux__
    bindtextdomain("kissplayer", LOCALEDIR);
#endif

    textdomain("kissplayer");
}

void Locale::setLanguage(int index)
{
    const char* choice = languages[index]->locale_name.c_str();

    dao_open_db();
    dao_set_key(KEY_LANGUAGE, choice);
    dao_set_key(KEY_LANGUAGE_INDEX, util_i2s(index));
    dao_close_db;
}

Language** Locale::getDefinedLanguages()
{
    return languages;
}
