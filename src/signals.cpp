#include "signals.h"

sigc::signal<void> SignalPlay;
sigc::signal<void> SignalStop;
sigc::signal<void> SignalNext;
sigc::signal<void> SignalPrevious;
sigc::signal<void> SignalSearch;
sigc::signal<void> SignalSearchType;
sigc::signal<void> SignalResetWindowTitle;
sigc::signal<void, float> SignalVolume;

sigc::signal<void> SignalSync;
sigc::signal<void> SignalCancelSync;
sigc::signal<void> SignalUpdateColors;
sigc::signal<void, int> SignalUpdateMusicPlaying;
