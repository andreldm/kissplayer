#include "music.h"

#include "util.h"

using namespace std;

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
string Music::getDesc(void)
{
    string temp = "";
    if (artist.empty() || title.empty()) {
        resolveNames();
    }

    temp.append(artist);
    temp.append(" - ");
    temp.append(title);

    return temp;
}

/**
* If the artist or title are missing, take them from the filename.
* The filename pattern must be: <artist> - <title>.<ext>
*/
void Music::resolveNames(void)
{
    int foundSlash;
    int foundDot;
    int foundHyphen;

    if (artist.empty()) {
        foundSlash = filepath.find_last_of("/\\");
        artist = filepath.substr(foundSlash+1);

        foundDot = artist.find_last_of(".");
        artist = artist.substr(0, foundDot);

        foundHyphen = artist.find_last_of("-");
        artist = artist.substr(0, foundHyphen);
        util_trim(artist);
    }

    if (title.empty()) {
        foundSlash = filepath.find_last_of("/\\");
        title = filepath.substr(foundSlash+1);

        foundDot = title.find_last_of(".");
        title = title.substr(0, foundDot);

        foundHyphen = title.find_last_of("-");
        title = title.substr(foundHyphen+1);
        util_trim(title);
    }
}
