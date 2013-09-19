#include "sound.h"
#include <stdio.h>
#include <FL/fl_utf8.h>

#if defined WIN32
    #include <fmod.h>
#else
    #include <fmodex/fmod.h>
#endif

#define PATH_LENGTH 4096

char current_file[PATH_LENGTH];
bool loaded = false;
bool playing = false;
FMOD_SYSTEM* _system;
FMOD_SOUND* _sound;
FMOD_CHANNEL* _channel;

int sound_initialize(void)
{
    if (FMOD_System_Create(&_system) != FMOD_OK) return -1;
    if (FMOD_System_Init(_system, 2, FMOD_INIT_NORMAL | FMOD_IGNORETAGS, 0) != FMOD_OK) return -1;
    // sets initial sound volume (mute)
    if (FMOD_Channel_SetVolume(_channel, 0.0f) != FMOD_OK) return -1;

    return 0;
}

void sound_load(const char* filename)
{
    strcpy(current_file, filename);
#if defined WIN32
    wchar_t wide_filename[PATH_LENGTH];
    fl_utf8towc(current_file, PATH_LENGTH, wide_filename, PATH_LENGTH);
    if (FMOD_System_CreateStream(_system, (char*) wide_filename, FMOD_DEFAULT | FMOD_UNICODE, 0, &_sound) != FMOD_OK) return;
#else
    if (FMOD_System_CreateStream(_system,current_file, FMOD_DEFAULT , 0, &_sound) != FMOD_OK) return;
#endif

    loaded = true;
}

void sound_unload(void)
{
    if(loaded) {
        FMOD_Channel_Stop(_channel);
        FMOD_Sound_Release(_sound);
        loaded = false;
    }
}

void sound_play(bool pause)
{
    FMOD_System_PlaySound(_system, FMOD_CHANNEL_REUSE, _sound, pause, &_channel);
    FMOD_Channel_SetMode(_channel,FMOD_LOOP_OFF);
}

void sound_toggle_active(void)
{
    sound_active(!loaded);
}

void sound_active(bool s)
{
    if (s) {
        sound_load(current_file);
        sound_play();
    } else {
        sound_unload();
    }
}

bool sound_check(void)
{
    return loaded;
}

void sound_pause(bool pause)
{
    FMOD_Channel_SetPaused(_channel, pause);
}

void sound_toggle_pause(void)
{
    if (loaded) {
        FMOD_BOOL p;
        FMOD_Channel_GetPaused(_channel,&p);
        FMOD_Channel_SetPaused (_channel,!p);
    }
}

bool sound_is_playing(void)
{
    FMOD_BOOL p;
    FMOD_Channel_GetPaused(_channel, &p);
    return (bool)!p;
}

int sound_length(void)
{
    if (loaded) {
        unsigned int mili;
        FMOD_Sound_GetLength(_sound, &mili, FMOD_TIMEUNIT_MS);
        return (int) mili;
    }
    return 0;
}

int sound_position(void)
{
    if (loaded) {
        unsigned int mili;
        FMOD_Channel_GetPosition(_channel, &mili, FMOD_TIMEUNIT_MS);
        return (int) mili;
    }
    return 0;
}

void sound_position(int mili)
{
    if (loaded) {
        FMOD_Channel_SetPosition(_channel, (unsigned int) (mili), FMOD_TIMEUNIT_MS);
    }
}

void sound_volume(float v)
{
    if (loaded && v >= 0.0f && v <= 1.0f) {
        FMOD_Channel_SetVolume(_channel,v);
    }
}

float sound_volume()
{
    float v = 0;
    if (loaded) {
        FMOD_Channel_GetVolume(_channel, &v);
    }
    return v;
}
