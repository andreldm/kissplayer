#ifndef name_cod_h
#define name_cod_h

#include <string>

using namespace std;

/**
* A facilite to store a primary key and some text.
*/
class NameCod
{

public:
    int cod;
    string name;

    NameCod (int, string);
    NameCod (void);
};
#endif
