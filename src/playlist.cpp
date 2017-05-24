#include "playlist.h"

#include <sstream>
#include <taglib/fileref.h>

#include "signals.h"
#include "util.h"

#if defined WIN32
    #include <time.h>
    #include <windows.h>
#else
    #include <stdlib.h>
    #include <curl/curl.h>
    static CURL* curl = curl_easy_init();
#endif

using namespace std;

Playlist::Playlist(Context* context, KSP_Browser* browser_music)
{
    this->context = context;
    this->browser_music = browser_music;
    this->musicPlayingCod = 0;
}

/**
* Parses the arguments to check if there are music files to play.
* Returns indicating if a music has been successfully parsed.
*/
bool Playlist::parse_args(int argc, char** argv)
{
    bool parsed = false;

    for (int i = 1; i < argc; i++) {
        string arg(argv[i]);

        if (util_is_ext_supported(arg)) {
            Music m;
            TagLib::FileRef* f = new TagLib::FileRef(arg.c_str());
            if (!f->isNull()) {
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

    if (parsed) {
        musicIndex = -1;
        musicIndexRandom = 0;

        if (context->configuration->shouldRandomize()) {
            musicIndex = listRandom.at(musicIndexRandom++);
        } else {
            musicIndex++;
        }

        SignalPlay.emit();
    }

    return parsed;
}

void Playlist::search(std::string text, SearchType type)
{
    context->dao->search_music(text, type, listMusic);
    update_browser();
}

void Playlist::update_browser()
{
    browser_music->clear();
    browser_music->clear_highlighted();

    for (unsigned int i = 0; i < listMusic.size(); i++) {
        Music m = listMusic.at(i);
        browser_music->add(m.getDesc().c_str());
        if (m.cod != 0 && m.cod == musicPlayingCod) {
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

    if (playAtBrowser && browser_music->value() == 0) {
        return false;
    }

    if (playAtBrowser || !context->sound->isLoaded()) {
        musicIndex = (browser_music->value() == 0) ? 0 : browser_music->value()-1;
    }

    Music music = listMusic.at(musicIndex);
    musicPlayingCod = music.cod;
    const char* filepath = music.filepath.c_str();

    // TODO: First we need to check if the file exists
    /*while (fl_access(filepath, 0)) { // 0 = F_OK
        if (!hasNext()) {
            stop(); // FIXME: Update WindowMain
            return false;
        }

        if (context->configuration->shouldRandomize()) musicIndex = listRandom.at(++musicIndexRandom);
        else musicIndex++;

        filepath = listMusic.at(musicIndex).filepath.c_str();
        browser_music->value(musicIndex + 1);
        browser_music->redraw();
    }*/

    context->sound->load(filepath);
    context->sound->play();
    context->sound->setVolume(volume);

    SignalUpdateMusicPlaying.emit(musicIndex);

    return true;
}

bool Playlist::stop()
{
    if (!context->sound->isPlaying()) {
        return false;
    }

    musicPlayingCod = 0;
    context->sound->setActive(false);

    return true;
}

bool Playlist::hasNext()
{
    // There's no music on the list or no music playing
    if (listMusic.empty() || !context->sound->isLoaded()) return false;

    if (context->configuration->shouldRepeatSong()) return true;

    // '-1' is the beginning of the playlist
    if (context->configuration->shouldRandomize()) {
        if (musicIndexRandom == -1 || musicIndexRandom + 2 <= (int) listRandom.size()) {
            return true;
        }
    } else if (musicIndex + 2 <= (int) listMusic.size()) {
        return true;
    }

    return false;
}

bool Playlist::hasPrevious()
{
    // There's no music on the list or no music playing
    if (listMusic.empty() || !context->sound->isLoaded()) return false;

    if ( context->configuration->shouldRepeatSong()) return true;

    return context->configuration->shouldRandomize() ?
        musicIndexRandom > 0 :
        musicIndex > 0;
}

void Playlist::increment_music_index()
{
    if (context->configuration->shouldRandomize()) {
        if (musicIndexRandom + 2 > (int) listMusic.size()) {
            if (!context->configuration->shouldRepeatSong()) return;
            musicIndexRandom = -1;
            util_randomize(listRandom, listMusic.size());
        }

        musicIndex = listRandom.at(++musicIndexRandom);
    } else {
        if (musicIndex + 2 > (int) listMusic.size()) {
            if (!context->configuration->shouldRepeatSong()) return;
            musicIndex = -1;
        }

        musicIndex++;
    }
}

void Playlist::decrement_music_index()
{
    if (context->configuration->shouldRandomize()) {
        if (musicIndexRandom <= 0) {
            if (!context->configuration->shouldRepeatSong()) return;
            musicIndexRandom = listMusic.size() -1;
            util_randomize(listRandom, listMusic.size());
        }

        musicIndex = listRandom.at(--musicIndexRandom);
    } else {
        if (musicIndex <= 0) {
            if (!context->configuration->shouldRepeatSong()) return;
            musicIndex = listMusic.size();
        }

        musicIndex--;
    }
}

void Playlist::next()
{
    if (hasNext()) {
        increment_music_index();
        play(lastVolume);
    }
}

void Playlist::previous()
{
    if (hasPrevious()) {
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

void Playlist::parse_dnd(string urls)
{
    istringstream lines(urls);
    string url;
    bool list_changed = false;

    while (getline(lines, url)) {
        if (util_is_ext_supported(url)) {
#ifdef WIN32
            wchar_t filepath[PATH_LENGTH];
            fl_utf8towc(url.c_str(), url.size(), filepath, PATH_LENGTH);
#else
            char* url_unescaped = curl_easy_unescape(curl , url.c_str(), 0, NULL);
            url = url_unescaped;
            util_replace_all(url, "file://", "");
            curl_free(url_unescaped);
            const char* filepath = url.c_str();
#endif
            Music m;
            TagLib::FileRef* f = new TagLib::FileRef(filepath);
            if (!f->isNull()) {
                m.title = f->tag()->title().toCString(true);
                m.artist = f->tag()->artist().toCString(true);
                m.album = f->tag()->album().toCString(true);
                m.filepath = url;
                m.resolveNames();

                if (!list_changed) {
                    listMusic.clear();
                    list_changed = true;
                }
                listMusic.push_back(m);
            }
            delete(f);
        }
    }

    if (list_changed) update_browser();
}
