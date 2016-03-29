#include "sound.h"

#include <stdio.h>
#include <FL/fl_utf8.h>

int Sound::init(void) {
    if (FMOD::System_Create(&system) != FMOD_OK) return -1;
    if (system->init(2, FMOD_INIT_NORMAL | FMOD_IGNORETAGS, 0) != FMOD_OK) return -1;

    return 0;
}

void Sound::destroy(void) {
    system->release();
}

void Sound::load(const char* filename) {
    unload();
    strcpy(current_file, filename);
#if defined WIN32
    wchar_t wide_filename[PATH_LENGTH];
    fl_utf8towc(current_file, PATH_LENGTH, wide_filename, PATH_LENGTH);
    if (system->createStream((char*) wide_filename, FMOD_DEFAULT | FMOD_UNICODE | FMOD_ACCURATETIME, 0, &sound) != FMOD_OK) return;
#else
    if (system->createStream(current_file, FMOD_DEFAULT | FMOD_ACCURATETIME, 0, &sound) != FMOD_OK) return;
#endif

    loaded = true;
}

void Sound::unload(void) {
    if (loaded) {
        channel->stop();
        sound->release();
        loaded = false;
    }
}

void Sound::play(bool pause) {
    system->playSound(FMOD_CHANNEL_REUSE, sound, pause, &channel);
    channel->setMode(FMOD_LOOP_OFF);
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
    channel->setPaused(pause);
}

void Sound::togglePaused(void) {
    if (loaded) {
        setPaused(isPlaying());
    }
}

bool Sound::isPlaying(void) {
    bool p;
    channel->getPaused(&p);
    return !p;
}

int Sound::length(void) {
    if (loaded) {
        unsigned int mili;
        sound->getLength(&mili, FMOD_TIMEUNIT_MS);
        return (int) mili;
    }

    return 0;
}

int Sound::getPosition(void) {
    if (loaded) {
        unsigned int mili;
        channel->getPosition(&mili, FMOD_TIMEUNIT_MS);
        return (int) mili;
    }

    return 0;
}

void Sound::setPosition(int mili) {
    if (loaded) {
        channel->setPosition((unsigned int) (mili), FMOD_TIMEUNIT_MS);
    }
}

void Sound::setVolume(float v) {
    if (loaded && v >= 0.0f && v <= 1.0f) {
        channel->setVolume(v);
    }
}

float Sound::getVolume() {
    float v = 0;
    if (loaded) {
        channel->getVolume(&v);
    }

    return v;
}
