#include "GKaSynth.hpp"
#include <complex>
#include "pffft.h"

struct gkaDirac : Module {

	enum ParamIds
	{
		FREQ_PARAM,
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
	gkaDirac() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)  ,_FreqPrev(0), _FreqNbEch(0), _Counter(0)
	{
	}
	void step() override;

	float	_FreqPrev;
	int		_FreqNbEch;
	int 	_Counter;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void gkaDirac::step() {

	if(params[FREQ_PARAM].value!=_FreqPrev)
	{
		float Interval=1.0/(pow(10.0,params[FREQ_PARAM].value))*10.0;
		_FreqNbEch=(int)roundf(engineGetSampleRate()*Interval);
		_FreqPrev=params[FREQ_PARAM].value;
	}

	if(_Counter>=_FreqNbEch)
	{
		outputs[OUT_OUTPUT].value=5;
		_Counter=0;
	} else {
		outputs[OUT_OUTPUT].value=0;
		++_Counter;
	}
}


struct gkaDiracWidget : ModuleWidget {
	gkaDiracWidget(Module *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/Module1.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));

		addParam(ParamWidget::create<RoundHugeBlackKnob>(Vec(18, 60), module, gkaDirac::FREQ_PARAM, 0.0f, 5.0f, 1.0f));

	    addOutput(Port::create<PJ301MPort>(Vec(55, 270), Port::OUTPUT, module, gkaDirac::OUT_OUTPUT));
	}
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelgkaDirac = Model::create<gkaDirac, gkaDiracWidget>("GKaSynth", "gkaDirac", "Dirac", OSCILLATOR_TAG);
