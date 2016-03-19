#include "configuration.h"

#include "constants.h"
#include "util.h"

using namespace std;


Configuration* Configuration::_instance = 0;

Configuration::Configuration()
{
}

Configuration* Configuration::instance()
{
    if (!_instance)
      _instance = new Configuration();
    return _instance;
}

void Configuration::save(SaveData data, Dao* dao)
{
    dao->begin_transaction();

    // TODO: On Windows, when the window is minimized its location becomes -32000, -32000.
    //       We need to save the location before it becomes minimized.
    if(data.x >= 0) {
        dao->set_key("window_main_x", util_i2s(data.x));
    }

    if(data.y >= 0) {
        dao->set_key("window_main_y", util_i2s(data.y));
    }

    dao->set_key("window_main_width", util_i2s(data.w));
    dao->set_key("window_main_height", util_i2s(data.h));
    // dao->set_key("window_maximized", util_i2s(os_specific_is_window_maximized()));
    // dao->set_key("browser_music_width", util_i2s(browser_music->w()));
    // dao->set_key("volume_level", util_i2s(volume_controller->value()));
    dao->set_key("random_button", util_i2s(_shouldRandomize));
    dao->set_key("repeat_button", util_i2s(_shouldRepeatSong));
    // dao->set_key("search_input", lastSearch);
    // dao->set_key("search_type", util_i2s(FLAG_SEARCH_TYPE));
    // dao->set_key("music_index", util_i2s(musicIndex));
    // dao->set_key("music_index_random", util_i2s(musicIndexRandom));
    dao->set_key("color_background", util_i2s(_background));
    dao->set_key("color_selection", util_i2s(_foreground));
    dao->set_key("color_text", util_i2s(_textcolor));
    dao->set_key("lyrics", util_i2s(_shouldFetchLyrics));
    dao->set_key("scroll_title", util_i2s(_shouldScrollTitle));

    dao->commit_transaction();
}


void Configuration::load(Dao* dao)
{
    dao->open_db();
    //
    // // SET WINDOW POSITION
    // int x = util_s2i(dao_get_key("window_main_x"));
    // if(x != -1) window->position(x, window->y());
    //
    // int y = util_s2i(dao_get_key("window_main_y"));
    // if(y != -1) window->position(window->x(), y);
    //
    // // SET WINDOW SIZE
    // int width = util_s2i(dao_get_key("window_main_width"));
    // if(width != -1) window->size(width, window->h());
    //
    // int height = util_s2i(dao_get_key("window_main_height"));
    // if(height != -1) window->size(window->w(), height);
    //
    // // SET WINDOW MAXIMIZED STATE
    // int maximized = util_s2i(dao_get_key("window_maximized"));
    // if(maximized == 1) {
    //     FLAG_MAXIMIZE_WINDOW = true;
    // }
    //
    // // SET VOLUME
    // float volume = util_s2i(dao_get_key("volume_level"));
    // if(volume != -1) {
    //     volume_controller->value(volume);
    //     cb_volume(volume_controller, NULL);
    // }
    //
    // // TOGGLE RANDOMIZE
    // int random = util_s2i(dao_get_key("random_button"));
    // if(random != -1) {
    //     // It should be inverted, because...
    //     FLAG_RANDOM = !random;
    //     // ...here we toggle the Random Button
    //     cb_random(NULL, 0);
    // }
    //
    // int repeat = util_s2i(dao_get_key("repeat_button"));
    // if(repeat != -1) {
    //     FLAG_REPEAT = !repeat;
    //     cb_repeat(NULL, 0);
    // }
    //
    // // SET SEARCH TYPE
    // int search_type = util_s2i(dao_get_key("search_type"));
    // if(search_type != -1) {
    //     FLAG_SEARCH_TYPE = search_type;
    //     choice_search_type->value(FLAG_SEARCH_TYPE);
    // }
    //
    // // SET SEARCH STRING
    // if(!skipLastSearch) {
    //     string search_input = dao_get_key("search_input");
    //     if(search_input.empty()) {
    //         dao_get_all_music(listMusic);
    //         update_playlist();
    //     } else {
    //         input_search->value(search_input.c_str());
    //         cb_search(NULL, 0);
    //     }
    // }
    //
    // // Open DB again, update_playlist and cb_search close it
    // dao_open_db();
    //
    // // SET LYRICS FLAG
    // int lyrics = util_s2i(dao_get_key("lyrics"));
    // if(lyrics != -1) {
    //     FLAG_LYRICS = lyrics;
    // } else {
    //     FLAG_LYRICS = true;
    // }
    //
    // // SET SCROLL TITLE FLAG
    // int scroll_title = util_s2i(dao_get_key("scroll_title"));
    // if(scroll_title != -1) {
    //     FLAG_SCROLL_TITLE = scroll_title;
    // } else {
    //     FLAG_SCROLL_TITLE = true;
    // }
    //
    // // SET MUSIC INDEX
    // int mi = util_s2i(dao_get_key("music_index"));
    // if(mi != -1) musicIndex = mi;
    //
    // int mir = util_s2i(dao_get_key("music_index_random"));
    // if(mir != -1) musicIndexRandom = mir;
    //
    // browser_music->value(musicIndex+1);
    //
    // // SET MUSIC BROWSER WIDTH
    // int browser_music_width = util_s2i(dao_get_key("browser_music_width"));
    //
    // if(browser_music_width > 0) {
    //     browser_music->size(browser_music_width, browser_music->h());
    //     int x = tile_center->x() + browser_music_width;
    //     int y = tile_center->y();
    //     int w = tile_center->w() - browser_music_width;
    //     int h = lyrics_pane->h();
    //     lyrics_pane->resize(x, y, w, h);
    // }
    //
    // // SET WIDGETS COLORS
    int color_background = util_s2i(dao->get_key("color_background"));
    int color_selection = util_s2i(dao->get_key("color_selection"));
    int color_text = util_s2i(dao->get_key("color_text"));

    background(color_background == -1 ? DEFAULT_BACKGROUND_COLOR : color_background);
    foreground(color_selection == -1 ? DEFAULT_SELECTION_COLOR : color_selection);
    textcolor(color_text == -1 ? DEFAULT_FOREGROUND_COLOR : color_text);

    dao->close_db();
}

bool Configuration::shouldMaximizeWindow() { return _shouldMaximizeWindow; }
void Configuration::shouldMaximizeWindow(bool v) { this->_shouldMaximizeWindow = v; }

bool Configuration::shouldRandomize() { return _shouldRandomize; }
void Configuration::shouldRandomize(bool v) { this->_shouldRandomize = v; }

bool Configuration::shouldRepeatSong() { return _shouldRepeatSong; }
void Configuration::shouldRepeatSong(bool v) { this->_shouldRepeatSong = v; }

bool Configuration::shouldFetchLyrics() { return _shouldFetchLyrics; }
void Configuration::shouldFetchLyrics(bool v) { this->_shouldFetchLyrics = v; }

bool Configuration::shouldScrollTitle() { return _shouldScrollTitle; }
void Configuration::shouldScrollTitle(bool v) { this->_shouldScrollTitle = v; }

bool Configuration::isCancelSync() { return _isCancelSync; }
void Configuration::isCancelSync(bool v) { this->_isCancelSync = v; }

bool Configuration::isListChanged() { return _isListChanged; }
void Configuration::isListChanged(bool v) { this->_isListChanged = v; }

Fl_Color Configuration::background() { return _background; }
void Configuration::background(Fl_Color v) { this->_background = v; }

Fl_Color Configuration::foreground() { return _foreground; }
void Configuration::foreground(Fl_Color v) { this->_foreground = v; }

Fl_Color Configuration::textcolor() { return _textcolor; }
void Configuration::textcolor(Fl_Color v) { this->_textcolor = v; }
