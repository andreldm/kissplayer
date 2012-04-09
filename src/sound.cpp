#include "sound.h"

/**
* OBS: This source was gaffled from somewhere and adapted to this program.
* It's like a wrapper for FMOD.
* Futher improvements are welcome.
*/

bool Sound::on = false; //is sound on?
bool Sound::possible = true; //is it possible to play sound?
string Sound::currentSound; //currently played sound
//FMOD-specific stuff
FMOD_RESULT Sound::result;
FMOD_SYSTEM * Sound::fmodsystem;
FMOD_SOUND * Sound::sound;
FMOD_CHANNEL * Sound::channel;

//initialises sound
void Sound::initialise (void)
{
    //create the sound system. If fails, sound is set to impossible
    result = FMOD_System_Create(&fmodsystem);
    if (result != FMOD_OK) possible = false;
    //if initialise the sound system. If fails, sound is set to impossible
    if (possible) result = FMOD_System_Init(fmodsystem,2, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK) possible = false;
    //sets initial sound volume (mute)
    if (possible) FMOD_Channel_SetVolume(channel,0.0f);
}

//sets the actual playing sound's volume
void Sound::setVolume (float v)
{
    if (possible && on && v >= 0.0f && v <= 1.0f)
    {
        FMOD_Channel_SetVolume(channel,v);
    }
}

//gets the actual playing sound's volume
float Sound::getVolume ()
{
    float v = 0;
    if (possible && on && v >= 0.0f && v <= 1.0f)
    {
        FMOD_Channel_GetVolume(channel,&v);
    }
    return v;
}

//loads a soundfile
void Sound::load (string filename)
{
    currentSound = filename;
    if (possible)
    {
        on = true;
        result = FMOD_Sound_Release(sound);
        result = FMOD_System_CreateStream(fmodsystem,currentSound.c_str(), FMOD_SOFTWARE, 0, &sound);
        if (result != FMOD_OK) possible = false;
    }
}

//frees the sound object
void Sound::unload (void)
{
    if (possible)
    {
        on = false;
        result = FMOD_Sound_Release(sound);
    }
}

//plays a sound (no argument to leave pause as default)
void Sound::play (bool pause)
{
    if (possible && on)
    {
        result = FMOD_System_PlaySound(fmodsystem,FMOD_CHANNEL_FREE, sound, pause, &channel);
        FMOD_Channel_SetMode(channel,FMOD_LOOP_OFF);
    }
}

//toggles sound on and off
void Sound::toggleSound (void)
{
    on = !on;
    if (on == true)
    {
        load(currentSound);
        play();
    }
    if (on == false)
    {
        unload();
    }
}

//pause or unpause the sound
void Sound::setPause (bool pause)
{
    FMOD_Channel_SetPaused (channel, pause);
}

//turn sound on or off
void Sound::setSound (bool s)
{
    on = s;
    if (on == true)
    {
        load(currentSound);
        play();
    }
    if (on == false)
    {
        unload();
    }
}

//toggle pause on and off
void Sound::togglePause (void)
{
    if (possible && on)
    {
        FMOD_BOOL p;
        FMOD_Channel_GetPaused(channel,&p);
        FMOD_Channel_SetPaused (channel,!p);
    }
}

//tells whether the sound is playing or not
bool Sound::isPlaying(void)
{
    FMOD_BOOL p;
    FMOD_Channel_GetPaused(channel, &p);
    return (bool)!p; // Dunno, but seems like the FMOD_BOOL is the inverse of C's bool
}

//tells whether the sound is on or off
bool Sound::getSound (void)
{
    return on;
}

//get the sound's length in miliseconds
int Sound::getLength (void)
{
    if (possible && on)
    {
        unsigned int mili;
        FMOD_Sound_GetLength(sound, &mili, FMOD_TIMEUNIT_MS);
        return (int) (mili);
    }
    else return 0;
}

//get the current sound's position within the length in miliseconds
int Sound::getPosition (void)
{
    if (possible && on)
    {
        unsigned int mili;
        FMOD_Channel_GetPosition(channel, &mili, FMOD_TIMEUNIT_MS);
        return (int) (mili);
    }
    else return 0;
}

//set the current sound's position within the length in miliseconds
void Sound::setPosition (int mili)
{
    if (possible && on)
    {
        FMOD_Channel_SetPosition(channel, (unsigned int) (mili), FMOD_TIMEUNIT_MS);
    }
}
