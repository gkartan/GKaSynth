#include "GKaSynth.hpp"


Plugin *plugin;


void init(Plugin *p) {
	plugin = p;
	p->slug = TOSTRING(SLUG);
	p->version = TOSTRING(VERSION);

	// Add all Models defined throughout the plugin
	p->addModel(modelgkaConvolver);
	p->addModel(modelgkaMultiplier);
	p->addModel(modelgkaNoteDisplayer);
	p->addModel(modelgkaNoteRecorder);
	p->addModel(modelgkaRawScope);
	p->addModel(modelgkaSequencer);
	p->addModel(modelgkaDirac);
	p->addModel(modelgkaDiracSpectrum);
	p->addModel(modelgkaPitchDisplayerTEKO);

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
