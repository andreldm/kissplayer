#ifndef sound_h
#define sound_h

#include "constants.h"

#if defined _WIN32
    #include <fmod.h>
#else
    #include <fmodex/fmod.h>
#endif

class Sound {
private:
    char current_file[PATH_LENGTH];
    bool loaded = false;
    bool playing = false;
    FMOD_SYSTEM* system;
    FMOD_SOUND* sound;
    FMOD_CHANNEL* channel;

public:
    int     init            (void); // initialises the sound system
    void    destroy         (void); // destroys the sound system
    void    load            (const char* filename); // loads a soundfile
    void    unload          (void); // frees the sound object
    bool    isLoaded        (void); // checks whether the sound is load or not

    void    play            (bool pause = false); // plays a sound (may start paused; no argument for unpaused)

    void    setActive       (bool sound); // set the sound on/off
    void    toggleActive    (void); // toggles sound on/off

    void    setPaused       (bool pause); // pause or unpause the sound
    void    togglePaused    (void); // toggles pause on/off

    bool    isPlaying       (void); // checks whether the sound is playing or not
    int     length          (void); // get the sound's length in miliseconds
    int     getPosition     (void); // get the current sound's position within the length in miliseconds
    void    setPosition     (int mili); // set the current sound's position within the length in miliseconds

    float   getVolume       (void); // gets the actual playing sound's volume
    void    setVolume       (float v); // sets the actual playing sound's volume
 };

#endif
