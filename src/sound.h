#ifndef sound_h
#define sound_h

#include <string>

#if defined WIN32
#include <fmod.h>
#else
#include <fmodex/fmod.h>
#endif

using namespace std;

/******** CLASS DEFINITION ********/

class Sound
{
private:
    static bool on; //is sound on?
    static bool possible; //is it possible to play sound?
    static string currentSound; //currently played sound
    //FMOD-specific stuff
    static FMOD_RESULT result;
    static FMOD_SYSTEM * fmodsystem;
    static FMOD_SOUND * sound;
    static FMOD_CHANNEL * channel;

public:
    static void initialise (void); //initialises sound

    //sound control
    static void setVolume (float v); //sets the actual playing sound's volume
    static float getVolume (); //gets the actual playing sound's volume
    static void load (string filename); //loads a soundfile
    static void unload (void); //frees the sound object
    static void play (bool pause = false); //plays a sound (may be started paused; no argument for unpaused)

    //getters
    static bool getSound (void); //checks whether the sound is on
    static bool isPlaying (void); //checks whether the sound is playing or not
    static int getLength (void); //get the sound's length in miliseconds
    static int getPosition (void); //get the current sound's position within the length in miliseconds

    //setters
    static void setPause (bool pause); //pause or unpause the sound
    static void setSound (bool sound); //set the sound on or off
    static void setPosition (int mili); //set the current sound's position within the length in miliseconds

    //toggles
    static void toggleSound (void); //toggles sound on and off
    static void togglePause (void); //toggle pause on/off
};
#endif
