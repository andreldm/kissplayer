#ifndef music_h
#define music_h

#include <string>

using namespace std;

class Music
{

public:
    int cod;
    string title;
    string artist;
    string album;
    string filepath;

    Music (void);
    string getDesc();
};
#endif
