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
    new Language("Español", "es.UTF-8"),
    new Language("Português (Brasil)", "pt_BR.UTF-8"),
    NULL
};

void Locale::init()
{
    dao_open_db();
    string language = dao_get_key(KEY_LANGUAGE);
    dao_close_db();

#ifdef WIN32
    language = "LC_ALL=" + language;
    putenv(language.c_str());
    bindtextdomain("kissplayer", ".\\locale");
#elif __linux__
    setlocale(LC_ALL, language.c_str());
    string dirname;
    os_specific_get_working_dir(dirname);
    dirname.append("locale");
    bindtextdomain("kissplayer", dirname.c_str());
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
