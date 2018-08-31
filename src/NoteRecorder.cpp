#include "GKaSynth.hpp"
#include <complex>
#include <deque>
#include <numeric>
#include <fstream>
#include "pffft.h"
#include "GKaUtility.hpp"


struct gkaNoteRecorder : Module {

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
	gkaNoteRecorder();
	virtual ~gkaNoteRecorder();

	void step() override;

	std::string		_displayNote;
	std::ofstream	_fileNote;
	int				_dureeNbEch;

	float			_voltPrev;
	int				_octavePrev;
	int				_notePrev;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu

};


gkaNoteRecorder::gkaNoteRecorder() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS),
									 _displayNote("Wait"),
									 _dureeNbEch(0),
									 _octavePrev(-10),
									 _notePrev(99)

{
	_fileNote.open ("noteRecord.txt", std::ofstream::out);
	_fileNote << "Pitch" << ";"	<< "Octave" << ";" << "Duration(s)" << ";"  << "Volts"<< ";" << "NbSamples"<< std::endl;
}

gkaNoteRecorder::~gkaNoteRecorder()
{
	_fileNote.close();
}

void gkaNoteRecorder::step()
{
	int octave;
	int note;

	NoteUtility::cv_to_note(inputs[IN_INPUT].value,octave,note);

	char buffer[3];
	sprintf(buffer,"%d",octave);
	_displayNote=NoteUtility::convert_note_to_str(note)+std::string("_")+std::string(buffer);

	//Recording
	if((octave!=_octavePrev) || (note!=_notePrev))
	{
		//to avoid glitch (around 10ms at 44100)
		if(_dureeNbEch>500)
		{
			_fileNote << NoteUtility::convert_note_to_str(_notePrev) << ";"
					  << _octavePrev << ";"
					  << ((float) _dureeNbEch / (float) engineGetSampleRate()) << ";"
					  << _voltPrev  << ";"
					  << _dureeNbEch << ";"
					  << std::endl;
		}
		_dureeNbEch=0;
	}

	_voltPrev=inputs[IN_INPUT].value;
	_octavePrev=octave;
	_notePrev=note;
	++_dureeNbEch;
}


struct gkaNoteRecorderWidget : ModuleWidget
{
	gkaNoteRecorderWidget(gkaNoteRecorder *module);

	 void step() override;

	Label* _pDisplayNote;
};


gkaNoteRecorderWidget::gkaNoteRecorderWidget(gkaNoteRecorder *module) : ModuleWidget(module)
{
	setPanel(SVG::load(assetPlugin(plugin, "res/Module3.svg")));

	addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));

	addInput(Port::create<PJ301MPort>(Vec(32, 100), Port::INPUT, module, gkaNoteRecorder::IN_INPUT));

	_pDisplayNote = new Label;
	_pDisplayNote->box.pos = Vec(10, 50);
	_pDisplayNote->box.size = Vec(100, 50);
	_pDisplayNote->fontSize=20;
	_pDisplayNote->color=nvgRGB(0xff,0xff,0xff);
	_pDisplayNote->text=module->_displayNote;
    addChild(_pDisplayNote);
}

void gkaNoteRecorderWidget::step()
{
	gkaNoteRecorder* myModule=dynamic_cast<gkaNoteRecorder*>(module);
	_pDisplayNote->text=myModule->_displayNote;
	ModuleWidget::step();
}


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelgkaNoteRecorder = Model::create<gkaNoteRecorder, gkaNoteRecorderWidget>("GKaSynth", "gkaNoteRecorder", "NoteRecorder", UTILITY_TAG);
