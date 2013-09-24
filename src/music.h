#ifndef music_h
#define music_h

#include <string>

class Music
{

public:
    int cod;
    std::string title;
    std::string artist;
    std::string album;
    std::string filepath;

    Music (void);
    std::string getDesc();
};

#endif
