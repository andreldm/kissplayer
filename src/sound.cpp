#include "sound.h"

#include <stdio.h>
#include <FL/fl_utf8.h>

int Sound::init(void) {
    if (FMOD_System_Create(&system) != FMOD_OK) return -1;
    if (FMOD_System_Init(system, 2, FMOD_INIT_NORMAL | FMOD_IGNORETAGS, 0) != FMOD_OK) return -1;

    return 0;
}

void Sound::destroy(void) {
    FMOD_System_Release(system);
}

void Sound::load(const char* filename) {
    unload();
    strcpy(current_file, filename);
#if defined WIN32
    wchar_t wide_filename[PATH_LENGTH];
    fl_utf8towc(current_file, PATH_LENGTH, wide_filename, PATH_LENGTH);
    if (FMOD_System_CreateStream(system, (char*) wide_filename, FMOD_DEFAULT | FMOD_UNICODE | FMOD_ACCURATETIME, 0, &sound) != FMOD_OK) return;
#else
    if (FMOD_System_CreateStream(system, current_file, FMOD_DEFAULT | FMOD_ACCURATETIME, 0, &sound) != FMOD_OK) return;
#endif

    loaded = true;
}

void Sound::unload(void) {
    if (loaded) {
        FMOD_Channel_Stop(channel);
        FMOD_Sound_Release(sound);
        loaded = false;
    }
}

void Sound::play(bool pause) {
    FMOD_System_PlaySound(system, FMOD_CHANNEL_REUSE, sound, pause, &channel);
    FMOD_Channel_SetMode(channel, FMOD_LOOP_OFF);
}

void Sound::toggleActive(void) {
    setActive(!loaded);
}

void Sound::setActive(bool s) {
    if (s) {
        load(current_file);
        play();
    } else {
        unload();
    }
}

bool Sound::isLoaded(void) {
    return loaded;
}

void Sound::setPaused(bool pause) {
    FMOD_Channel_SetPaused(channel, pause);
}

void Sound::togglePaused(void) {
    if (loaded) {
        setPaused(isPlaying());
    }
}

bool Sound::isPlaying(void) {
    FMOD_BOOL p;
    FMOD_Channel_GetPaused(channel, &p);
    return !p;
}

int Sound::length(void) {
    if (loaded) {
        unsigned int mili;
        FMOD_Sound_GetLength(sound, &mili, FMOD_TIMEUNIT_MS);
        return (int) mili;
    }

    return 0;
}

int Sound::getPosition(void) {
    if (loaded) {
        unsigned int mili;
        FMOD_Channel_GetPosition(channel, &mili, FMOD_TIMEUNIT_MS);
        return (int) mili;
    }

    return 0;
}

void Sound::setPosition(int mili) {
    if (loaded) {
        FMOD_Channel_SetPosition(channel, (unsigned int) (mili), FMOD_TIMEUNIT_MS);
    }
}

void Sound::setVolume(float v) {
    if (loaded && v >= 0.0f && v <= 1.0f) {
        FMOD_Channel_SetVolume(channel, v);
    }
}

float Sound::getVolume() {
    float v = 0;
    if (loaded) {
        FMOD_Channel_GetVolume(channel, &v);
    }

    return v;
}
