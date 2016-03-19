#include "playlist.h"

#include <taglib/fileref.h>

#include "configuration.h"
#include "signals.h"
#include "util.h"

using namespace std;

Playlist::Playlist(Dao* dao, Sound* sound, KSP_Browser* browser_music)
{
    this->dao = dao;
    this->browser_music = browser_music;
    this->sound = sound;
}

/**
* Parses the arguments to check if there are music files to play.
* Returns indicating if a music has been successfully parsed.
*/
bool Playlist::parse_args(int argc, char** argv)
{
    bool parsed = false;

    for(int i = 1; i < argc; i++) {
        string arg(argv[i]);

        if(util_is_ext_supported(arg)) {
            Music m;
            TagLib::FileRef* f = new TagLib::FileRef(arg.c_str());
            if(!f->isNull()) {
                parsed = true;
                m.title = f->tag()->title().toCString(true);
                m.artist = f->tag()->artist().toCString(true);
                m.album = f->tag()->album().toCString(true);
                m.filepath = arg;
                m.resolveNames();
                listMusic.push_back(m);
                browser_music->add(m.getDesc().c_str());
            }
            delete(f);
        }
    }

    util_randomize(listRandom, listMusic.size());

    return parsed;
}

bool Playlist::search(std::string text, SearchType type)
{
    dao->search_music(text, type, listMusic);
    update_browser();
}

void Playlist::update_browser()
{
    browser_music->clear();
    browser_music->clear_highlighted();

    for(int i = 0; i < listMusic.size(); i++) {
        Music m = listMusic.at(i);
        browser_music->add(m.getDesc().c_str());
        if(m.cod != 0 && m.cod == musicPlayingCod) {
            browser_music->set_highlighted(i + 1);
            browser_music->value(i + 1);
        }
    }

    musicIndexRandom = -1;
    util_randomize(listRandom, listMusic.size());
}

bool Playlist::play(float volume, bool playAtBrowser)
{
    lastVolume = volume;

    if(playAtBrowser && browser_music->value() == 0) {
        return false;
    }

    if(playAtBrowser || !sound->isLoaded()) {
        musicIndex = (browser_music->value() == 0) ? 0 : browser_music->value()-1;
    }

    Music music = listMusic.at(musicIndex);
    musicPlayingCod = music.cod;
    const char* filepath = music.filepath.c_str();

    // TODO: First we need to check if the file exists
    /*while(fl_access(filepath, 0)) { // 0 = F_OK
        if(!hasNext()) {
            stop(); // FIXME: Update WindowMain
            return false;
        }

        if(Configuration::instance()->shouldRandomize()) musicIndex = listRandom.at(++musicIndexRandom);
        else musicIndex++;

        filepath = listMusic.at(musicIndex).filepath.c_str();
        browser_music->value(musicIndex + 1);
        browser_music->redraw();
    }*/

    sound->load(filepath);
    sound->play();
    sound->setVolume(volume);

    SignalUpdateMusicPlaying.emit(musicIndex);

    return true;
}

bool Playlist::stop()
{
    if(!sound->isPlaying()) {
        return false;
    }

    musicPlayingCod = 0;
    sound->setActive(false);

    return true;
}

bool Playlist::hasNext()
{
    // There's no music on the list or no music playing
    if(listMusic.empty() || !sound->isLoaded()) return false;

    if(Configuration::instance()->shouldRepeatSong()) return true;

    // '-1' is the beginning of the playlist
    if(Configuration::instance()->shouldRandomize()) {
        if(musicIndexRandom == -1 || musicIndexRandom + 2 <= listRandom.size()) {
            return true;
        }
    } else if(musicIndex + 2 <= listMusic.size()) {
        return true;
    }

    return false;
}

bool Playlist::hasPrevious()
{
    // There's no music on the list or no music playing
    if(listMusic.empty() || !sound->isLoaded()) return false;

    if(Configuration::instance()->shouldRepeatSong()) return true;

    return Configuration::instance()->shouldRandomize() ?
        musicIndexRandom > 0 :
        musicIndex > 0;
}

void Playlist::increment_music_index()
{
    if(Configuration::instance()->shouldRandomize()) {
        if(musicIndexRandom + 2 > listMusic.size()) {
            if(!Configuration::instance()->shouldRepeatSong()) return;
            musicIndexRandom = -1;
            util_randomize(listRandom, listMusic.size());
        }

        musicIndex = listRandom.at(++musicIndexRandom);
    } else {
        if(musicIndex + 2 > listMusic.size()) {
            if(!Configuration::instance()->shouldRepeatSong()) return;
            musicIndex = -1;
        }

        musicIndex++;
    }
}

void Playlist::decrement_music_index()
{
    if(Configuration::instance()->shouldRandomize()) {
        if(musicIndexRandom <= 0) {
            if(!Configuration::instance()->shouldRepeatSong()) return;
            musicIndexRandom = listMusic.size() -1;
            util_randomize(listRandom, listMusic.size());
        }

        musicIndex = listRandom.at(--musicIndexRandom);
    } else {
        if(musicIndex <= 0) {
            if(!Configuration::instance()->shouldRepeatSong()) return;
            musicIndex = listMusic.size();
        }

        musicIndex--;
    }
}

bool Playlist::next()
{
    if(hasNext()) {
        increment_music_index();
        play(lastVolume);
    }
}

bool Playlist::previous()
{
    if(hasPrevious()) {
        decrement_music_index();
        play(lastVolume);
    }
}

Music Playlist::getCurrentMusic()
{
    return listMusic.at(musicIndex);
}

bool Playlist::isEmpty()
{
    return listMusic.empty();
}
