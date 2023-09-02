#include "rack.hpp"


using namespace rack;

// Forward-declare the Plugin, defined in Template.cpp
extern Plugin *plugin;

// Forward-declare each Model, defined in each module source file
extern Model *modelgkaConvolver;
extern Model *modelgkaMultiplier;
extern Model *modelgkaNoteDisplayer;
extern Model *modelgkaNoteRecorder;
extern Model *modelgkaSequencer;
extern Model *modelgkaRawScope;
extern Model *modelgkaDirac;
extern Model *modelgkaDiracSpectrum;
extern Model *modelgkaPitchDisplayerTEKO;
