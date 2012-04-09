#include "music.h"

Music::Music (void)
{
    cod = 0;
    title = "";
    artist = "";
    album = "";
    filepath = "";
}

/**
* Get Music's Description as in this format: "Artist - Title"
*/
string Music::getDesc()
{
    string temp = "";
    if(artist.empty())
    {
        size_t foundSlash;
        size_t foundDot;
        foundSlash = filepath.find_last_of("/\\");
        temp = filepath.substr(foundSlash+1);

        foundDot = temp.find_last_of(".");
        temp = temp.substr(0,foundDot);
    }
    else
    {
        temp.append(artist);
        temp.append(" - ");
        temp.append(title);
    }

    return temp;
}
