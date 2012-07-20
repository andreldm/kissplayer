#include "dir_tools.h"
#include <dirent.h> //It has to be included here in order to avoid colision with FL/filename.H

/**
* OBS: This source was gaffled from somewhere and adapted to this program.
* Futher improvements are welcome.
*/

using namespace std;

string getCurrentDirectory()
{
    char cCurrentPath[FILENAME_MAX];

    if (!getcwd(cCurrentPath, sizeof(cCurrentPath) / sizeof(char)))
        return "";

    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
    return cCurrentPath;
}

vector<string> travelDirectory(string directory)
{
    //Travel thru a directory gathering all the file and directory names
    vector<string> fileList;
    DIR *dir;
    struct dirent *ent;

    //Open a directory
    if ((dir=opendir(directory.c_str())) != NULL)
    {
        while((ent=readdir(dir)) != NULL) // loop until the directory is traveled thru
        {
            //Push directory or filename to the list
            fileList.push_back(ent->d_name);
        }
        //Close up
        closedir(dir);
    }
    //Return the filelist
    return fileList;
}

void travelDirectoryRecursive(string directory, vector<string> *fullList)
{
    //Get the "root" directory's directories
    vector<string> fileList = travelDirectory(directory);

    //Loop thru the list
    for (vector<string>::iterator i=fileList.begin(); i!=fileList.end(); ++i)
    {
        //Test for . and .. directories (this and back)
        if (strcmp((*i).c_str(), ".") &&
                strcmp((*i).c_str(), ".."))
        {
            //I use stringstream here, not string = foo; string.append(bar);
            stringstream fullname;
            fullname << directory << "/" << (*i);

            //List of supported file formats
            if(strstr((*i).c_str(), ".mp3") != NULL ||
                    strstr((*i).c_str(), ".wma") != NULL ||
                    strstr((*i).c_str(), ".ogg") != NULL ||
                    strstr((*i).c_str(), ".wav") != NULL ||
                    strstr((*i).c_str(), ".flac") != NULL)
                fullList->push_back(fullname.str());

            travelDirectoryRecursive(fullname.str(), fullList);
        }
    }
}
