#include "signals.h"

using namespace sigc;

signal<void> SignalPlay;
signal<void> SignalStop;
signal<void> SignalNext;
signal<void> SignalPrevious;
signal<void> SignalSearch;
signal<void> SignalSearchType;
signal<void, float> SignalVolume;

signal<void> SignalCancelSync;
signal<void> SignalUpdateColors;
signal<void, int> SignalUpdateMusicPlaying;
