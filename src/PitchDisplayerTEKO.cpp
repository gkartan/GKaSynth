#include "GKaSynth.hpp"
#include <complex>
#include <deque>
#include <numeric>
#include "pffft.h"
#include "GKaUtility.hpp"

#define NB_INTEGRATION 20

struct gkaPitchDisplayerTEKO : Module {

	enum ParamIds
	{
		NUM_PARAMS
	};

	enum InputIds {
		IN_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds
    {
        NUM_LIGHTS
	};
	gkaPitchDisplayerTEKO() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS),_Index(0),_displayNote("Wait")
	{
		memset(_Value,0,4*sizeof(float));
		memset(_ValueDerivated,0,4*sizeof(float));
	}
	void step() override;


	float			_Value[4];
	float			_ValueDerivated[4];
	int				_Index;
	std::string		_displayNote;
	std::deque<double> _listFondamental;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


//Method with Teager-Kaiser Energy operator
//output with a delay of 2 value
// cf http://folk.uio.no/eivindkv/ek-thesis-2003-05-12-final-2.pdf
void gkaPitchDisplayerTEKO::step()
{
	_Value[3]=_Value[2];
	_Value[2]=_Value[1];
	_Value[1]=_Value[0];
	_Value[0]=inputs[IN_INPUT].value;

	_ValueDerivated[3]=_ValueDerivated[2];
	_ValueDerivated[2]=_ValueDerivated[1];
	_ValueDerivated[1]=_ValueDerivated[0];
	_ValueDerivated[0]=_Value[1]-_Value[0];

	double fTeagerEnergy=(_Value[2]*_Value[2])-(_Value[1]*_Value[3]);
	double fTeagerEnergySigDerivated=(_ValueDerivated[2]*_ValueDerivated[2])-(_ValueDerivated[1]*_ValueDerivated[3]);

	if(fTeagerEnergy>0)
	{
		double fTeagerOperator=1-fTeagerEnergySigDerivated/(2.0*fTeagerEnergy);
		if(fTeagerOperator>=-1.0 && fTeagerOperator<=1.0)
		{
			double fFondamental=engineGetSampleRate()*acos(fTeagerOperator)/(2.0*M_PI);
			_listFondamental.push_back(fFondamental);
			if(_listFondamental.size()>NB_INTEGRATION) _listFondamental.pop_front();
			double fFondamental_Moy=std::accumulate(_listFondamental.begin(),_listFondamental.end(),0);
			fFondamental_Moy/=(float)_listFondamental.size();

			int Octave=0;
			int Note=0;
			NoteUtility::freq_to_note(fFondamental_Moy,Octave,Note);

			char buffer[3];
			sprintf(buffer,"%d",Octave);
			_displayNote=NoteUtility::convert_note_to_str(Note)+std::string("_")+std::string(buffer);
		}
	}
}


struct gkaPitchDisplayerTEKOWidget : ModuleWidget
{
	gkaPitchDisplayerTEKOWidget(gkaPitchDisplayerTEKO *module);

	 void step() override;

	Label* _pDisplayNote;
};


gkaPitchDisplayerTEKOWidget::gkaPitchDisplayerTEKOWidget(gkaPitchDisplayerTEKO *module) : ModuleWidget(module)
{
	setPanel(SVG::load(assetPlugin(plugin, "res/Module2.svg")));

	addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));

	addInput(Port::create<PJ301MPort>(Vec(32, 100), Port::INPUT, module, gkaPitchDisplayerTEKO::IN_INPUT));

	_pDisplayNote = new Label;
	_pDisplayNote->box.pos = Vec(10, 50);
	_pDisplayNote->box.size = Vec(100, 50);
	_pDisplayNote->fontSize=20;
	_pDisplayNote->color=nvgRGB(0xff,0xff,0xff);
	_pDisplayNote->text=module->_displayNote;
    addChild(_pDisplayNote);
}

void gkaPitchDisplayerTEKOWidget::step()
{
	gkaPitchDisplayerTEKO* myModule=dynamic_cast<gkaPitchDisplayerTEKO*>(module);
	_pDisplayNote->text=myModule->_displayNote;
	ModuleWidget::step();
}


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelgkaPitchDisplayerTEKO = Model::create<gkaPitchDisplayerTEKO,gkaPitchDisplayerTEKOWidget>("GKaSynth", "gkaPitchDisplayerTEKO", "PitchDisplayerTEKO", UTILITY_TAG);
