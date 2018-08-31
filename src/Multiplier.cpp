#include "GKaSynth.hpp"
#include <complex>
#include "pffft.h"

struct gkaMultiplier : Module {

	enum ParamIds
	{
		NUM_PARAMS
	};

	enum InputIds {
		IN1_INPUT,
		IN2_INPUT,
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
	gkaMultiplier() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
	{
	}
	void step() override;


	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void gkaMultiplier::step() {
	outputs[OUT_OUTPUT].value=inputs[IN1_INPUT].value*inputs[IN2_INPUT].value;
}


struct gkaMultiplierWidget : ModuleWidget {
	gkaMultiplierWidget(Module *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/Module1.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));

		addInput(Port::create<PJ301MPort>(Vec(10, 200), Port::INPUT, module, gkaMultiplier::IN1_INPUT));
	    addInput(Port::create<PJ301MPort>(Vec(10, 270), Port::INPUT, module, gkaMultiplier::IN2_INPUT));
	    addOutput(Port::create<PJ301MPort>(Vec(55, 270), Port::OUTPUT, module, gkaMultiplier::OUT_OUTPUT));
	}
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelgkaMultiplier = Model::create<gkaMultiplier, gkaMultiplierWidget>("GKaSynth", "gkaMultiplier", "Multiplier", MIXER_TAG);
