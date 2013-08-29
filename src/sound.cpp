#include "sound.h"
/**
* OBS: This source was gaffled from somewhere and adapted to this program.
* It's a wrapper for FMOD.
* Futher improvements are welcome.
*/

bool Sound::loaded = false;
bool Sound::playing = false;
string Sound::currentFile;

FMOD_SYSTEM * Sound::system;
FMOD_SOUND * Sound::sound;
FMOD_CHANNEL * Sound::channel;

void Sound::initialise (void)
{
    if (FMOD_System_Create(&system) != FMOD_OK) return;
    if (FMOD_System_Init(system, 2, FMOD_INIT_NORMAL | FMOD_IGNORETAGS, 0) != FMOD_OK) return;
    // sets initial sound volume (mute)
    if (FMOD_Channel_SetVolume(channel, 0.0f) != FMOD_OK) return;
}

void Sound::load (string filename)
{
    currentFile = filename;
    if (FMOD_System_CreateStream(system,currentFile.c_str(), FMOD_DEFAULT, 0, &sound) != FMOD_OK) return;
    loaded = true;
}

//frees the sound object
void Sound::unload (void)
{
    if(loaded) {
        FMOD_Channel_Stop(channel);
        FMOD_Sound_Release(sound);
        loaded = false;
    }
}

//plays a sound (pause = false by default)
void Sound::play (bool pause)
{
    FMOD_System_PlaySound(system, FMOD_CHANNEL_REUSE, sound, pause, &channel);
    FMOD_Channel_SetMode(channel,FMOD_LOOP_OFF);
}

//toggles sound on and off
void Sound::toggleSound (void)
{
    setSound(!loaded);
}

//turn sound on or off
void Sound::setSound (bool s)
{
    if (s) {
        load(currentFile);
        play();
    } else {
        unload();
    }
}

//tells whether the sound is on or off
bool Sound::getSound (void)
{
    return loaded;
}

void Sound::setPause (bool pause)
{
    FMOD_Channel_SetPaused (channel, pause);
}

void Sound::togglePause (void)
{
    if (loaded)
    {
        FMOD_BOOL p;
        FMOD_Channel_GetPaused(channel,&p);
        FMOD_Channel_SetPaused (channel,!p);
    }
}

bool Sound::isPlaying(void)
{
    FMOD_BOOL p;
    FMOD_Channel_GetPaused(channel, &p);
    return (bool)!p;
}

// get sound length in miliseconds
int Sound::getLength (void)
{
    if (loaded) {
        unsigned int mili;
        FMOD_Sound_GetLength(sound, &mili, FMOD_TIMEUNIT_MS);
        return (int) mili;
    }
    return 0;
}

// get current sound position in miliseconds
int Sound::getPosition (void)
{
    if (loaded) {
        unsigned int mili;
        FMOD_Channel_GetPosition(channel, &mili, FMOD_TIMEUNIT_MS);
        return (int) mili;
    }
    return 0;
}

// set current sound position in miliseconds
void Sound::setPosition (int mili)
{
    if (loaded)
    {
        FMOD_Channel_SetPosition(channel, (unsigned int) (mili), FMOD_TIMEUNIT_MS);
    }
}

void Sound::setVolume (float v)
{
    if (loaded && v >= 0.0f && v <= 1.0f) {
        FMOD_Channel_SetVolume(channel,v);
    }
}

float Sound::getVolume ()
{
    float v = 0;
    if (loaded) {
        FMOD_Channel_GetVolume(channel, &v);
    }
    return v;
}
