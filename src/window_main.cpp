#include "window_main.h"
#include "images.h"
#include "os_specific.h"

/**
* The Program's main window(And due my lousy architecture, the program's core)
*/

//LOCAL WIDGETS
Fl_Button *button_play;
Fl_Button *button_stop;
Fl_Button *button_previous;
Fl_Button *button_next;
Fl_Button *button_sync;
Fl_Button *button_search;
Fl_Button *button_clear;
Fl_Button *button_settings;
Fl_Button *button_about;
Fl_Button *button_random;
Fl_Dial *dial_volume;
Fl_Select_Browser *browser_music;
Fl_Slider_Music *slider_music;
Fl_Box *box_current_time;
Fl_Input *input_search;
Fl_Choice *choice_search_type;
Fl_Text_Display *lyrics_pane;
Fl_Text_Buffer *lyrics_text_buffer;
Fl_Group *group_search;
Fl_Group *group_controls;
Fl_Tile *tile_center;

//LOCAL VARIABLES
extern Fl_Double_Window *window_main;
vector<Music> *listMusic;
vector<int> *listRandom; // we use this when randomization is activated
Fl_Window *window_loading;
string windowTitle;
int windowTitlePosition;
int musicIndex;
int musicIndexRandom;
string lastSearch;
bool doNotLoadLastSearch;

//LOCAL FUNCTIONS
void play_music();
void update_playlist(vector<Music> * l);
void timer_title_scrolling(void*);
void timer_check_music(void*);
void save_config();
void load_config();
bool hasNextMusic();
int main_handler(int, Fl_Window *);

//LOCAL CALLBACKS
void cb_close_window(Fl_Widget*, void*);
void cb_toggle_play(Fl_Widget*, void*);
void cb_previous(Fl_Widget*, void*);
void cb_stop(Fl_Widget*, void*);
void cb_next(Fl_Widget*, void*);
void cb_volume(Fl_Widget*, void*);
void cb_music_browser(Fl_Widget*, void*);
void cb_sync(Fl_Widget*, void*);
void cb_search(Fl_Widget*, void*);
void cb_search_type(Fl_Widget*, void*);
void cb_random(Fl_Widget*, void*);
void cb_settings(Fl_Widget*, void*);
void cb_about(Fl_Widget*, void*);
void cb_clear(Fl_Widget*, void*);
void cb_slider_music(Fl_Widget*, void*);

Fl_Double_Window* make_window_main(int argc, char **argv)
{
    // To place the window at the center of the screen
    int window_w = 420;
    int window_h = 370;
    int screen_w = Fl::w();
    int screen_h = Fl::h();
    int window_x = (screen_w/2)-(window_w/2);
    int window_y = (screen_h/2)-(window_h/2);

    window_main = new Fl_Double_Window(window_x, window_y, window_w, window_h, "KISS Player");
    window_main->size_range(window_w, window_h);
    window_main->callback((Fl_Callback*)cb_close_window);

    // SEARCH GROUP AND ITS WIDGETS
    group_search = new Fl_Group(5, 5, 410, 30);
    group_search->box(FL_UP_FRAME);
    group_search->begin();

    input_search = new Fl_Input(67, 10, 205, 22,"Search:");
    input_search->maximum_size(50);
    input_search->when(FL_WHEN_ENTER_KEY|FL_WHEN_NOT_CHANGED);
    input_search->callback(cb_search);

    choice_search_type = new Fl_Choice(275, 10, 80, 22);
    choice_search_type->add("All");
    choice_search_type->add("Title");
    choice_search_type->add("Artist");
    choice_search_type->add("Album");
    choice_search_type->value(0);
    choice_search_type->callback(cb_search_type);
    FLAG_SEARCH_TYPE = SEARCH_TYPE_ALL;

    button_clear = new Fl_Button(361, 10, 22, 22);
    button_clear->image(icon_clear);
    button_clear->callback(cb_clear);

    button_search = new Fl_Button(388, 10, 22, 22);
    button_search->image(icon_search);
    button_search->callback(cb_search);

    group_search->resizable(input_search);
    group_search->end();

    // CENTER TILE AND ITS WIDGETS
    tile_center = new Fl_Tile(5, 40, 410, 230);
    tile_center->begin();

    browser_music = new Fl_Select_Browser(5, 40, 205, 230, NULL);
    browser_music->color(DEFAULT_BACKGROUND_COLOR);
    browser_music->box(FL_DOWN_BOX);
    browser_music->type(FL_HOLD_BROWSER);
    browser_music->callback(cb_music_browser);

    lyrics_text_buffer = new Fl_Text_Buffer();

    lyrics_pane = new Fl_Text_Display (210, 40, 205, 230, NULL);
    lyrics_pane->buffer(lyrics_text_buffer);
    lyrics_pane->box(FL_DOWN_BOX);
    lyrics_pane->textfont(FL_HELVETICA);
    lyrics_pane->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0);
    lyrics_pane->scrollbar_width(15);
    lyrics_pane->color(DEFAULT_BACKGROUND_COLOR);
    lyrics_pane->callback(cb_slider_music);
    lyrics_pane->scrollbar_align(FL_ALIGN_RIGHT);

    tile_center->box(FL_DOWN_BOX);
    tile_center->end();

    // CONTROLS GROUP AND ITS WIDGETS
    group_controls = new Fl_Group(5, 275, 410, 90);
    group_controls->box(FL_UP_FRAME);
    group_controls->begin();

    slider_music = new Fl_Slider_Music(10, 280, 403, 20, "00:00");
    slider_music->callback(cb_slider_music);
    slider_music->color2(0x018BFD00);
    slider_music->align(FL_ALIGN_BOTTOM_RIGHT);

    box_current_time = new Fl_Box(10, 300, 40, 15, "00:00");

    button_play = new Fl_Button(10, 330, 25, 25);
    button_play->clear_visible_focus();
    button_play->image(icon_play);
    button_play->tooltip("Play/Pause");
    button_play->callback(cb_toggle_play);

    button_stop = new Fl_Button(45, 330, 25, 25);
    button_stop->clear_visible_focus();
    button_stop->image(icon_stop);
    button_stop->tooltip("Stop");
    button_stop->callback(cb_stop);

    button_previous = new Fl_Button(80, 330, 25, 25);
    button_previous->clear_visible_focus();
    button_previous->image(icon_previous);
    button_previous->tooltip("Previous");
    button_previous->callback(cb_previous);

    button_next = new Fl_Button(115, 330, 25, 25);
    button_next->clear_visible_focus();
    button_next->image(icon_next);
    button_next->tooltip("Next");
    button_next->callback(cb_next);

    Fl_Box *separator = new Fl_Box(FL_FLAT_BOX, 150, 330, 1, 25,"");
    separator->color(FL_DARK3);

    button_sync = new Fl_Button(160, 330, 25, 25);
    button_sync->clear_visible_focus();
    button_sync->callback(cb_sync);
    button_sync->tooltip("Synchronize Music Library");
    button_sync->image(icon_sync);

    button_random = new Fl_Button(195, 330, 25, 25);
    button_random->clear_visible_focus();
    button_random->image(icon_random_disabled);
    button_random->tooltip("Randomize");
    button_random->callback(cb_random);

    button_settings = new Fl_Button(230, 330, 25, 25);
    button_settings->clear_visible_focus();
    button_settings->image(icon_settings);
    button_settings->callback(cb_settings);
    button_settings->tooltip("Settings");

    button_about = new Fl_Button(265, 330, 25, 25);
    button_about->clear_visible_focus();
    button_about->image(icon_about);
    button_about->tooltip("About");
    button_about->callback(cb_about);

    // It won't be visible, it's just for the resize work nicely
    Fl_Button *scape_goat = new Fl_Button(330, 330, 10, 10);
    scape_goat->hide();

    dial_volume = new Fl_Dial(370, 321, 38, 38, NULL);
    dial_volume->value(0.8);
    dial_volume->callback(cb_volume);

    group_controls->resizable(scape_goat);
    group_controls->end();

    // END OF WIDGET'S SETUP

    // Check for music files on arguments
    listMusic = parseArgs(argc, argv);

    // If there is one or more on music files as argument, display it/them
    if(listMusic != NULL && listMusic->size() > 0)
    {
        for(int i = 0; i < listMusic->size(); i++)
        {
            Music m = listMusic->at(i);
            browser_music->add(m.getDesc().c_str());
        }
        doNotLoadLastSearch = true;
        randomize(&listRandom, listMusic->size());
    }

    Fl::event_dispatch(main_handler);

    Fl::add_timeout(0.5, timer_check_music);
    Fl::add_timeout(0.2, timer_title_scrolling);

    Fl::scheme("GTK+");
    window_main->resizable(tile_center);
    window_main->end();

    load_config();

    return window_main;
}

void cb_close_window(Fl_Widget* widget, void*)
{
    // ignore Escape key
    if (Fl::event()==FL_SHORTCUT && Fl::event_key()==FL_Escape)
        return;

    sound_unload();
    save_config();

    window_main->~Fl_Window();
}

void cb_toggle_play(Fl_Widget* widget, void*)
{
    if(sound_check())
    {
        //It has to be done before the togglePause
        if(sound_is_playing())
        {
            window_main->label("KISS Player - Paused");
            button_play->image(icon_play);
            button_play->redraw();
#ifdef WIN32
            update_thumbnail_toolbar("play");
#endif
        }
        else
        {
            button_play->image(icon_pause);
            button_play->redraw();
#ifdef WIN32
            update_thumbnail_toolbar("pause");
#endif
        }

        sound_toggle_pause();
    }
    else if(!listMusic->empty())
    {
        musicIndex = (browser_music->value()==0)? 0 : browser_music->value()-1;
        play_music();
    }
}

void cb_stop(Fl_Widget* widget, void*)
{
    if(!sound_is_playing()) return;

    sound_active(false);
    window_main->label("KISS Player");
    button_play->image(icon_play);
    button_play->redraw();

    box_current_time->label("00:00");
    slider_music->label("00:00");
    slider_music->value(0);
}

void cb_previous(Fl_Widget* widget, void*)
{
    if(listMusic->empty()) return; //If there's no music on the list, do not continue
    if(!sound_check()) return; //If there's no music playing, do not continue

    sound_unload();

    if(FLAG_RANDOM) {
        if(musicIndexRandom >= 1) {
            musicIndex = listRandom->at(--musicIndexRandom);
            play_music();
        }
    } else if(musicIndex > 0) {
        musicIndex--;
        play_music();
    }
    cout << "musicIndex = "<< musicIndex<< endl;
    cout << "musicIndexRandom = "<< musicIndexRandom<< endl;
}

void cb_next(Fl_Widget* widget, void*)
{
    if(hasNextMusic())
    {
        if(FLAG_RANDOM) {
            musicIndex = listRandom->at(++musicIndexRandom);
        } else {
            musicIndex++;
        }
        sound_unload();
        play_music();
    }
    cout << "\n\nmusicIndex = "<< musicIndex;
    cout << "\nmusicIndexRandom = "<< musicIndexRandom;
    cout << "\nlistMusic->size() = "<< listMusic->size();
    cout << "\nFLAG_RANDOM = "<< FLAG_RANDOM;
}

void cb_music_browser(Fl_Widget* widget, void*)
{
    if(Fl::event_clicks() > 0 && Fl::event_button() == FL_LEFT_MOUSE)
    {
        if(browser_music->value() == 0) return;
        musicIndex = browser_music->value() - 1;
        play_music();
    }
}

void cb_volume(Fl_Widget* widget, void*)
{
    Fl_Dial *dial = (Fl_Dial*) widget;
    sound_volume(dial->value());
}

void cb_slider_music(Fl_Widget* widget, void*)
{
    //If there's no music playing, do not change
    //The slider's changes are handled at timer_check_music and my_handler
    if(!sound_check())
    {
        slider_music->value(0);
        return;
    }
}

void cb_search(Fl_Widget* widget, void*)
{
    lastSearch = input_search->value();
    update_playlist(searchMusics(lastSearch.c_str()));
}

void cb_search_type(Fl_Widget* widget, void*)
{
    const char * choice = choice_search_type->text();
    if(strcmp(choice, "All") == 0) FLAG_SEARCH_TYPE = SEARCH_TYPE_ALL;
    else if(strcmp(choice, "Title") == 0) FLAG_SEARCH_TYPE = SEARCH_TYPE_TITLE;
    else if(strcmp(choice, "Artist") == 0) FLAG_SEARCH_TYPE = SEARCH_TYPE_ARTIST;
    else if(strcmp(choice, "Album") == 0) FLAG_SEARCH_TYPE = SEARCH_TYPE_ALBUM;

    input_search->take_focus();
    input_search->do_callback();
}

void cb_clear(Fl_Widget* widget, void*)
{
    input_search->value(NULL);
    input_search->do_callback();
}

void cb_random(Fl_Widget* widget, void*)
{
    FLAG_RANDOM = !FLAG_RANDOM;

    if(FLAG_RANDOM)
        button_random->image(icon_random_enabled);
    else
        button_random->image(icon_random_disabled);

    button_random->redraw();
}

void cb_settings(Fl_Widget* widget, void*)
{
    window_settings = make_window_settings();
    window_settings->show();
}

void cb_about(Fl_Widget* widget, void*)
{
    window_about = make_window_about();
    window_about->show();
}

void cb_sync(Fl_Widget* widget, void*)
{
    button_stop->do_callback();
    misc_sync_library();
    update_playlist(getAllMusics());
}

int main_handler(int e, Fl_Window *w)
{
    //AS THE SLIDER IS RELEASED, WE SET THE SOUND'S POSITION
    if(e == FL_RELEASE && Fl::belowmouse() == slider_music && !Fl::event_buttons())
    {
        box_current_time->label(formatTime(slider_music->value())); //IN ORDER TO UPDATE WITHOUT DELAY

        // We need to store the volume, because when we play it again, the FMOD will reset this value
        float volume = sound_volume();

        //If the user is holding the slider and the music reaches its end
        //we need to play the sound, otherwise we get a strange sound loop.
        //Remove these two lines and see it for yourself :)
        //OBS: This isPlaying evalution seems crazy, but it's related to FMOD_BOOL :(
        if(sound_is_playing())
            sound_play();

        sound_position(slider_music->value());
        sound_volume(volume);
    }

    if(e == FL_KEYDOWN)
    {
        // Increase Volume
        if(Fl::event_original_key() == FL_Volume_Down)
        {
            float v = dial_volume->value() - 0.05f;
            if (v < 0) v = 0;
            dial_volume->value(v);
            sound_volume(v);
            return 0;
        }

        // Decrease Volume
        else if(Fl::event_original_key() == FL_Volume_Up)
        {
            float v = dial_volume->value() + 0.05f;
            if (v > 1) v = 1;
            dial_volume->value(v);
            sound_volume(v);
            return 0;
        }
    }

    if(e == FL_KEYUP)
    {
        // Go 5s foward
        if(Fl::event_original_key() == FL_Right)
        {
            if(input_search == Fl::focus())
            {
                return Fl::handle_(e, w);
            }
            Fl::focus(slider_music);

            int pos = sound_position();
            if(pos < sound_length() + 5000)
                sound_position(pos + 5000);

            return 1;
        }

        // Go 5s backward
        else if(Fl::event_original_key() == FL_Left)
        {
            if(input_search == Fl::focus())
            {
                return Fl::handle_(e, w);
            }
            Fl::focus(slider_music);

            int pos = sound_position();
            if(sound_length() - 5000 > 0)
                sound_position(pos - 5000);

            return 1;
        }

        return 0;
    }

	 //MOUSEWHEEL HANDLER FOR VOLUME DIAL
	if(e == FL_MOUSEWHEEL &&
		Fl::belowmouse() != NULL &&
		Fl::belowmouse() != browser_music &&
		Fl::belowmouse() != lyrics_pane &&
		Fl::belowmouse()->parent() != browser_music &&
		Fl_Window::current() == window_main)
	{
		float valor = 0.1 * Fl::event_dy();
		valor = dial_volume->value()-valor;
		if(valor < 0) valor = 0;
		if(valor > 1) valor = 1;
		dial_volume->value(valor);
		sound_volume(valor);
		return 0;
	}

	return Fl::handle_(e, w);
}

void play_music()
{
    browser_music->value(musicIndex+1);
    browser_music->redraw();

    sound_load(listMusic->at(musicIndex).filepath.c_str());
    sound_play();
    sound_volume(dial_volume->value());

    windowTitle = "KISS Player :: ";
    windowTitle.append(browser_music->text(musicIndex+1));
    windowTitlePosition = 0;

    button_play->image(icon_pause);
    button_play->redraw();
#ifdef WIN32
    update_thumbnail_toolbar("pause");
#endif

    box_current_time->label("00:00");
    slider_music->copy_label(formatTime(sound_length()));
    slider_music->maximum(sound_length());
    slider_music->value(0);
    if(FLAG_LYRICS) {
        fetch_lyrics(lyrics_text_buffer, listMusic->at(musicIndex).artist, listMusic->at(musicIndex).title);
        lyrics_pane->scroll(0,0);
    }
    else {
        lyrics_text_buffer->text("");
    }
}

void timer_title_scrolling(void*)
{
    Fl::repeat_timeout(0.2, timer_title_scrolling);
    if (!sound_check() || !sound_is_playing()) {
        return;
    }

    if (windowTitlePosition == windowTitle.size()) {
        windowTitlePosition = 0;
    } else {
        windowTitlePosition++;
    }

    string title = windowTitle.substr(windowTitlePosition, windowTitle.size());
    title.append(" - ");
    title.append(windowTitle.substr(0, windowTitlePosition));

    window_main->copy_label(title.c_str());
}

void timer_check_music(void*)
{
    Fl::repeat_timeout(0.25, timer_check_music);

    //If there's no music playing, do not continue
    if(!sound_check()) {
        return;
    }

    /* If the music reached its end, so advance to the next one.
       Note that we check if any mouse button is pressed, so if the user
       is dragging the slider it won't advance the music.
       OBS: In rare cases, the music stucks at the maximum - 1 ms. */
    if(slider_music->value() >= (slider_music->maximum() - 1) &&
       Fl::pushed() != slider_music) {
        if(hasNextMusic()) {
            cb_next(NULL, 0);
        } else {
            cb_stop(NULL, 0);
        }
        return;
    }

    box_current_time->copy_label(formatTime(sound_position()));
    box_current_time->redraw();

    if(Fl::pushed() != slider_music) {
        slider_music->value(sound_position());
    }
}

void save_config()
{
    beginTransaction();

    // TODO: On Windows, when the window is minimized its location becomes -32000, -32000.
    //       We need to save the location before it becomes minimized.
    if(window_main->x() >= 0) {
        setKey("window_main_x", intToString(window_main->x()));
    }

    if(window_main->y() >= 0) {
        setKey("window_main_y", intToString(window_main->y()));
    }

    setKey("window_main_width", intToString(window_main->w()));
    setKey("window_main_height", intToString(window_main->h()));

    setKey("window_maximized", intToString(isWindowMaximized(window_main)));

    setKey("browser_music_width", intToString(browser_music->w()));

    setKey("volume_level", floatToString(dial_volume->value()));

    setKey("random_button", intToString(FLAG_RANDOM));

    setKey("search_input", lastSearch);

    setKey("search_type", intToString(FLAG_SEARCH_TYPE));

    setKey("music_index", intToString(musicIndex));

    setKey("music_index_random", intToString(musicIndexRandom));

    setKey("color_background", intToString(browser_music->color()));

    setKey("color_selection", intToString(browser_music->color2()));

    setKey("color_text", intToString(browser_music->textcolor()));

    setKey("lyrics", intToString(FLAG_LYRICS));

    commitTransaction();
}

void load_config()
{
    openDB();

    // SET WINDOW POSITION
    int x = stringToInt(getKey("window_main_x"));
    if(x != -1) window_main->position(x, window_main->y());

    int y = stringToInt(getKey("window_main_y"));
    if(y != -1) window_main->position(window_main->x(), y);

    // SET WINDOW SIZE
    int width = stringToInt(getKey("window_main_width"));
    if(width != -1) window_main->size(width, window_main->h());

    int height = stringToInt(getKey("window_main_height"));
    if(height != -1) window_main->size(window_main->w(), height);

    // SET WINDOW MAXIMIZED STATE
    int maximized = stringToInt(getKey("window_maximized"));
    if(maximized == 1) {
        shouldMaximizeWindow = true;
    }

    // SET VOLUME
    float volume = stringToFloat(getKey("volume_level"));
    if(volume != -1) {
        dial_volume->value(volume);
        sound_volume(dial_volume->value());
    }

    // TOGGLE RANDOMIZE
    int random = stringToInt(getKey("random_button"));
    if(random != -1) {
        // It should be inverted, because...
        FLAG_RANDOM = !random;
        // ...here toggle the Random Button
        cb_random(NULL, 0);
    }

    // SET SEARCH TYPE
    int search_type = stringToInt(getKey("search_type"));
    if(search_type != -1) {
        FLAG_SEARCH_TYPE = search_type;
        choice_search_type->value(FLAG_SEARCH_TYPE);
    }

    // SET SEARCH STRING
    string search_input = getKey("search_input");
    if(!search_input.empty()) {
        input_search->value(search_input.c_str());
        if(!doNotLoadLastSearch)
            cb_search(NULL, 0);
    }
    // If the search string is empty, load all the musics
    else if(!doNotLoadLastSearch) {
        update_playlist(getAllMusics());
    }

    // SET LYRICS FLAG
    int lyrics = stringToInt(getKey("lyrics"));
    if(lyrics != -1) {
        FLAG_LYRICS = lyrics;
    }
    else {
        FLAG_LYRICS = true;
    }

    // SET MUSIC INDEX
    int mi = stringToInt(getKey("music_index"));
    if(mi != -1) musicIndex = mi;

    int mir = stringToInt(getKey("music_index_random"));
    if(mir != -1) musicIndexRandom = mir;

    browser_music->value(musicIndex+1);

    // SET MUSIC BROWSER WIDTH
    int browser_music_width = stringToInt(getKey("browser_music_width"));

    if(browser_music_width > 0) {
        browser_music->size(browser_music_width, browser_music->h());
        lyrics_pane->resize(tile_center->x() + browser_music_width,
                            tile_center->y(),
                            tile_center->w() - browser_music_width,
                            lyrics_pane->h());
    }

    // SET WIDGETS COLORS
    int color_background = stringToInt(getKey("color_background"));
    int color_selection = stringToInt(getKey("color_selection"));
    int color_text = stringToInt(getKey("color_text"));

    if(color_background == -1)
        color_background = DEFAULT_BACKGROUND_COLOR;
    if(color_selection == -1)
        color_selection = FL_SELECTION_COLOR;
    if(color_text == -1)
        color_text = FL_FOREGROUND_COLOR;

    browser_music->color(color_background);
    browser_music->color2(color_selection);
    lyrics_pane->color(color_background);
    lyrics_pane->color2(color_selection);
    input_search->color2(color_selection);
    choice_search_type->color2(color_selection);
    browser_music->textcolor(color_text);
    lyrics_pane->textcolor(color_text);

    browser_music->redraw();
    lyrics_pane->redraw();
    input_search->redraw();
    choice_search_type->redraw();

    closeDB();
}

bool hasNextMusic()
{
    // There's no music on the list or no music playing
    if(listMusic->empty() || !sound_check()) {
        return false;
    }

    // '-1' is the beginning of the playlist
    if(FLAG_RANDOM) {
        if(musicIndexRandom == -1 || musicIndexRandom + 2 <= listRandom->size())
            return true;
    } else if(musicIndex + 2 <= listMusic->size()) {
        return true;
    }

    return false;
}

void update_playlist(vector<Music> * l)
{
    if(listMusic != 0) {
        listMusic->clear();
        delete listMusic;
    }

    listMusic = l;
    browser_music->clear();

    for(int i = 0; i < listMusic->size(); i++) {
        Music m = listMusic->at(i);
        browser_music->add(m.getDesc().c_str());
    }

    musicIndexRandom = -1;
    randomize(&listRandom, listMusic->size());
}
