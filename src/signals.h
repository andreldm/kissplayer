#ifndef signals_h
#define signals_h

#include <sigc++/sigc++.h>
#include <FL/Fl_Widget.H>

extern sigc::signal<void> SignalPlay;
extern sigc::signal<void> SignalStop;
extern sigc::signal<void> SignalNext;
extern sigc::signal<void> SignalPrevious;
extern sigc::signal<void> SignalSearch;
extern sigc::signal<void> SignalSearchType;
extern sigc::signal<void> SignalResetWindowTitle;
extern sigc::signal<void, float> SignalVolume;

extern sigc::signal<void> SignalSync;
extern sigc::signal<void> SignalCancelSync;
extern sigc::signal<void> SignalUpdateColors;
extern sigc::signal<void, int> SignalUpdateMusicPlaying;

#endif
