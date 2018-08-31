#include "GKaSynth.hpp"
#include <complex>
#include <deque>
#include <numeric>
#include "pffft.h"
#include "GKaUtility.hpp"

#define FFT_SIZE 8192
#define NB_INTEGRATION 20

struct gkaNoteDisplayer : Module {

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
	gkaNoteDisplayer() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS),_nbEchBuffer(0),_displayNote("Wait")
	{
		_setupFFT = pffft_new_setup(FFT_SIZE, PFFFT_REAL);
		_buffer=(float*)pffft_aligned_malloc(FFT_SIZE*sizeof(float));
		_fftout=(float*)pffft_aligned_malloc(FFT_SIZE*sizeof(float));
		dspUtility::init_window(_window,FFT_SIZE);
	}
	void step() override;

	float* 			_buffer;
	int   			_nbEchBuffer;
	PFFFT_Setup*	_setupFFT;
	float*			_fftout;
	float*			_window;
	std::deque<float> _listFondamental;

	std::string		_displayNote;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void gkaNoteDisplayer::step()
{

	if(_nbEchBuffer==FFT_SIZE)
	{
		pffft_transform_ordered(_setupFFT, _buffer, _fftout, 0, PFFFT_FORWARD);

		//Max Peak Search => Fundamental Note
		float fMax=-1000;
		int iMax=0;
		for(int i=0;i<FFT_SIZE/2;++i)
		{
			if(_fftout[i]>fMax)
			{
				fMax=_fftout[i];
				iMax=i;
			}
		}

		//No computing if no signal
		if(iMax>0)
		{

			//Fundamental frequency - (FFT Resolution = samplerate/ffsize)
			float fFondamental=(engineGetSampleRate()/float(FFT_SIZE))*(float(iMax));

			//Frequency meaning (useful for low freq)
			_listFondamental.push_back(fFondamental);
			if(_listFondamental.size()>NB_INTEGRATION) _listFondamental.pop_front();
			float fFondamental_Moy=0.0;
			fFondamental_Moy=std::accumulate(_listFondamental.begin(),_listFondamental.end(),fFondamental_Moy);
			fFondamental_Moy/=(float)_listFondamental.size();

			//Freq to cv for linear computing
			int Octave=0;
			int Note=0;
			NoteUtility::freq_to_note(fFondamental_Moy,Octave,Note);

			char buffer[3];
			sprintf(buffer,"%d",Octave);

			_displayNote=NoteUtility::convert_note_to_str(Note)+std::string("_")+std::string(buffer);
		}

		_nbEchBuffer=0;
	}

	_buffer[_nbEchBuffer]=inputs[IN_INPUT].value*_window[_nbEchBuffer];
	++_nbEchBuffer;
}


struct gkaNoteDisplayerWidget : ModuleWidget
{
	gkaNoteDisplayerWidget(gkaNoteDisplayer *module);

	 void step() override;

	Label* _pDisplayNote;
};


gkaNoteDisplayerWidget::gkaNoteDisplayerWidget(gkaNoteDisplayer *module) : ModuleWidget(module)
{
	setPanel(SVG::load(assetPlugin(plugin, "res/Module2.svg")));

	addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));

	addInput(Port::create<PJ301MPort>(Vec(32, 100), Port::INPUT, module, gkaNoteDisplayer::IN_INPUT));

	_pDisplayNote = new Label;
	_pDisplayNote->box.pos = Vec(10, 50);
	_pDisplayNote->box.size = Vec(100, 50);
	_pDisplayNote->fontSize=20;
	_pDisplayNote->color=nvgRGB(0xff,0xff,0xff);
	_pDisplayNote->text=module->_displayNote;
    addChild(_pDisplayNote);
}

void gkaNoteDisplayerWidget::step()
{
	gkaNoteDisplayer* myModule=dynamic_cast<gkaNoteDisplayer*>(module);
	_pDisplayNote->text=myModule->_displayNote;
	ModuleWidget::step();
}


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelgkaNoteDisplayer = Model::create<gkaNoteDisplayer, gkaNoteDisplayerWidget>("GKaSynth", "gkaNoteDisplayer", "NoteDisplayer", UTILITY_TAG);
