#include "GKaSynth.hpp"
#include <complex>
#include <deque>
#include <numeric>
#include <fstream>
#include "pffft.h"
#include "GKaUtility.hpp"

float TableVolt[]=
{
		4.75891,
		5.00357,
		5.1576,
		5.39768,
		5.37494,
		5.59455,
		4.75907,
		4.99256,
		5.75164,
		5.43836,
		4.99286,
		5.30403,
		5.19852,
		4.7559,
		5.39102,
		5.74824,
		4.75921,
		4.99266,
		5.30411,
		5.2008,
		5.34616,
		5.42748,
		5.19897,
		4.75794,
		5.30396,
		5.20147,
		5.30407,
		5.14734,
		5.59446
};

int TablesNbSamples[]=
{
		44097,
		11024,
		33074,
		22050,
		55125,
		11022,
		44097,
		44097,
		11022,
		33074,
		22049,
		22050,
		33074,
		11022,
		44098,
		44095,
		11022,
		33074,
		22050,
		22050,
		33075,
		11024,
		44099,
		55122,
		33074,
		22050,
		55125,
		11024,
		44097
};

struct gkaSequencer : Module {

	enum ParamIds
	{
		NUM_PARAMS
	};

	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds
    {
        NUM_LIGHTS
	};
	gkaSequencer();
	virtual ~gkaSequencer();

	void step() override;

	int _NbTotalSamples;
	int _NbTotalIndex;
	int _CurrentSample;
	int _CurrentIndex;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu

};


gkaSequencer::gkaSequencer() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS),
							_NbTotalSamples(0),
							_CurrentSample(0),
							_CurrentIndex(0)

{
	_NbTotalIndex=sizeof(TablesNbSamples)/sizeof(int);
	for(int i=0;i<_NbTotalIndex;++i)
	{
		_NbTotalSamples+=TablesNbSamples[i];
	}

}

gkaSequencer::~gkaSequencer()
{
}

void gkaSequencer::step()
{
	if(_CurrentSample>(TablesNbSamples[_CurrentIndex]/3))
	{
		_CurrentIndex=(_CurrentIndex+1)%_NbTotalIndex;
		_CurrentSample=0;
	}
	++_CurrentSample;

	outputs[OUT_OUTPUT].value=TableVolt[_CurrentIndex] / 4.0;
}


struct gkaSequencerWidget : ModuleWidget
{
	gkaSequencerWidget(gkaSequencer *module);
};


gkaSequencerWidget::gkaSequencerWidget(gkaSequencer *module) : ModuleWidget(module)
{
	setPanel(SVG::load(assetPlugin(plugin, "res/Module4.svg")));

	addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));

	addOutput(Port::create<PJ301MPort>(Vec(32, 100), Port::OUTPUT, module, gkaSequencer::OUT_OUTPUT));
}



// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelgkaSequencer = Model::create<gkaSequencer, gkaSequencerWidget>("GKaSynth", "gkaSequencer", "Sequencer", SEQUENCER_TAG);
