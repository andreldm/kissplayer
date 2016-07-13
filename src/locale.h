#ifndef locale_h
#define locale_h

#include <libintl.h>
#include <locale.h>
#include <string>

#include "dao.h"
#include "util.h"

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
private:
    Dao* dao;

public:
    void init(Dao* dao);
    void setLanguage(int index);
    Language** getDefinedLanguages();
};

#endif
