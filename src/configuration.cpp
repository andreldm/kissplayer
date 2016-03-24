#include "configuration.h"

#include "constants.h"

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

void Configuration::save(ConfigData data, Dao* dao)
{
    dao->begin_transaction();

    // TODO: On Windows, when the window is minimized its location becomes -32000, -32000.
    //       We need to save the location before it becomes minimized.
    if (data.x >= 0) dao->set_key_int("window_main_x", data.x);
    if (data.y >= 0) dao->set_key_int("window_main_y", data.y);

    dao->set_key_int("window_main_width", data.w);
    dao->set_key_int("window_main_height", data.h);
    dao->set_key_int("window_maximized", data.isWindowMaximized);
    dao->set_key_int("browser_music_width", data.browserMusicWidth);
    dao->set_key_int("volume_level", data.volumeLevel);
    dao->set_key_int("random_button", _shouldRandomize);
    dao->set_key_int("repeat_button", _shouldRepeatSong);
    dao->set_key("search_input", data.lastSearch);
    dao->set_key_int("search_type", data.searchType);
    dao->set_key_int("music_index", data.musicIndex);
    dao->set_key_int("music_index_random", data.musicIndexRandom);
    dao->set_key_int("color_background", _background);
    dao->set_key_int("color_selection", _foreground);
    dao->set_key_int("color_text", _textcolor);
    dao->set_key_int("lyrics", _shouldFetchLyrics);
    dao->set_key_int("scroll_title", _shouldScrollTitle);

    dao->commit_transaction();
}

ConfigData Configuration::load(Dao* dao)
{
    int value;
    ConfigData data;
    dao->open_db();

    data.x = dao->get_key_int("window_main_x");
    data.y = dao->get_key_int("window_main_y");
    data.w = dao->get_key_int("window_main_width");
    data.h = dao->get_key_int("window_main_height");
    data.isWindowMaximized = (dao->get_key_int("window_maximized") > 0);
    // TODO: check float convertion
    data.volumeLevel = dao->get_key_int("volume_level");
    data.searchType = dao->get_key_int("search_type");
    data.lastSearch = dao->get_key("search_input");
    data.musicIndex = dao->get_key_int("music_index");
    data.musicIndexRandom = dao->get_key_int("music_index_random");
    data.browserMusicWidth = dao->get_key_int("browser_music_width");

    // _shouldMaximizeWindow, bool)

    _shouldRandomize = (dao->get_key_int("random_button") > 0);
    _shouldRepeatSong = (dao->get_key_int("repeat_button") > 0);

    value = dao->get_key_int("lyrics");
    _shouldFetchLyrics = (value == -1 ? true : value);

    value = dao->get_key_int("scroll_title");
    _shouldScrollTitle = (value == -1 ? true : value);

    value = dao->get_key_int("color_background");
    _background = (value == -1 ? DEFAULT_BACKGROUND_COLOR : value);

    value = dao->get_key_int("color_selection");
    _foreground = (value == -1 ? DEFAULT_SELECTION_COLOR : value);

    value = dao->get_key_int("color_text");
    _textcolor = (value == -1 ? DEFAULT_FOREGROUND_COLOR : value);

    dao->close_db();

    return data;
}

#define GETTER(attr, type) type Configuration::attr() { return _##attr; }
#define SETTER(attr, type) void Configuration::attr(type v) { this->_##attr = v; }
#define GETTER_SETTER(attr, type) GETTER(attr, type) SETTER(attr, type)

GETTER_SETTER(shouldMaximizeWindow, bool)
GETTER_SETTER(shouldRandomize, bool)
GETTER_SETTER(shouldRepeatSong, bool)
GETTER_SETTER(shouldFetchLyrics, bool)
GETTER_SETTER(shouldScrollTitle, bool)
GETTER_SETTER(isCancelSync, bool)
GETTER_SETTER(isListChanged, bool)
GETTER_SETTER(lastSearch, string)
GETTER_SETTER(background, Fl_Color)
GETTER_SETTER(foreground, Fl_Color)
GETTER_SETTER(textcolor, Fl_Color)

#undef GETTER
#undef SETTER
#undef GETTER_SETTER
