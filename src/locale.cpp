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
    new Language("Čeština", "cs.UTF-8"),
    new Language("Deutsch", "de.UTF-8"),
    new Language("English", "en.UTF-8"),
    new Language("ελληνικά", "el.UTF-8"),
    new Language("Español", "es.UTF-8"),
    new Language("Italiano", "it.UTF-8"),
    new Language("Português (Brasil)", "pt_BR.UTF-8"),
    new Language("Türkçe", "tr.UTF-8"),
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
    bind_textdomain_codeset("kissplayer", "UTF-8");
#elif __linux__
    bindtextdomain("kissplayer", LOCALEDIR);
    setlocale(LC_ALL, "");
#endif

    textdomain("kissplayer");
}

void Locale::setLanguage(int index)
{
    const char* choice = languages[index]->locale_name.c_str();

    dao_open_db();
    dao_set_key(KEY_LANGUAGE, choice);
    dao_set_key(KEY_LANGUAGE_INDEX, util_i2s(index));
    dao_close_db();
}

Language** Locale::getDefinedLanguages()
{
    return languages;
}
