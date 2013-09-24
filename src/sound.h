#ifndef sound_h
#define sound_h

int     sound_initialize    (void); //initialises sound system
void    sound_load          (const char* filename); // loads a soundfile
void    sound_unload        (void); // frees the sound object
bool    sound_is_loaded     (void); // checks whether the sound is load or not

void    sound_play          (bool pause = false); // plays a sound (may start paused; no argument for unpaused)

void    sound_active        (bool sound); // set the sound on/off
void    sound_toggle_active (void); // toggles sound on/off

void    sound_pause         (bool pause); // pause or unpause the sound
void    sound_toggle_pause  (void); // toggles pause on/off

bool    sound_is_playing    (void); // checks whether the sound is playing or not
int     sound_length        (void); // get the sound's length in miliseconds
int     sound_position      (void); // get the current sound's position within the length in miliseconds
void    sound_position      (int mili); // set the current sound's position within the length in miliseconds

void    sound_volume        (float v); // sets the actual playing sound's volume
float   sound_volume        (void); // gets the actual playing sound's volume

#endif
