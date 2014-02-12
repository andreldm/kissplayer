#ifndef locale_h
#define locale_h

#include <libintl.h>
#include <locale.h>
#include <string>

#define _(STRING) gettext(STRING)

class Language {
public:
    std::string description;
    std::string locale_name;

    Language(std::string s1, std::string s2)
    {
        description = s1;
        locale_name = s2;
    }
};

class Locale
{
public:
    static void init();
    static void setLanguage(int index);
    static Language** getDefinedLanguages();
};

#endif
