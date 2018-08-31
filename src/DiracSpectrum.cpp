#include "GKaSynth.hpp"
#include "GKaUtility.hpp"
#include <complex>
#include "pffft.h"

#define FFT_SIZE 16384

struct gkaDiracSpectrum : Module {

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
	gkaDiracSpectrum() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) ,_FreqPrev(1.0), _Counter(0)
	{
		_setupFFT = pffft_new_setup(FFT_SIZE, PFFFT_REAL);

		_Buffer=(float*)pffft_aligned_malloc(FFT_SIZE*sizeof(float));
		_BufferOut=(float*)pffft_aligned_malloc(FFT_SIZE*sizeof(float));
		memset(_Buffer,0,FFT_SIZE*sizeof(float));
		_Buffer[0]=5;

		pffft_transform_ordered(_setupFFT, _Buffer, _BufferOut, 0, PFFFT_BACKWARD);
		//Normalisation a revoir
		//pour N*N?
		for(int i=0;i<FFT_SIZE;++i)
		{
			_BufferOut[i]/=float(FFT_SIZE);
		}

	}
	void step() override;

	PFFFT_Setup* _setupFFT;
	float*		_Buffer;
	float*		_BufferOut;
	float		_FreqPrev;
	int 		_Counter;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void gkaDiracSpectrum::step() {

	if(params[FREQ_PARAM].value!=_FreqPrev)
	{
		float Interval=1.0/(pow(10.0,params[FREQ_PARAM].value));

		int   FreqNbEch1=(int)roundf(engineGetSampleRate()*Interval);
		int   FreqNbEch=(int)roundf((FreqNbEch1*FFT_SIZE)/engineGetSampleRate());
		if(FreqNbEch<=0) FreqNbEch=1;

		memset(_Buffer,0,FFT_SIZE*sizeof(float));
		for(int i=0;i<FFT_SIZE;i=i+FreqNbEch)
		{
			_Buffer[i]=5.0;
		}
		pffft_transform_ordered(_setupFFT, _Buffer, _BufferOut, 0, PFFFT_BACKWARD);

		//Normalisation en niveau max
		float Max=-1000;
		for(int i=0;i<FFT_SIZE;i=i+1)
		{
			Max=(_BufferOut[i]>Max)?_BufferOut[i]:Max;
		}

		Max/=5.0;
		for(int i=0;i<FFT_SIZE;i=i+1)
		{
			_BufferOut[i]=_BufferOut[i]/Max;
		}

		_FreqPrev=params[FREQ_PARAM].value;
	}


	_Counter = (_Counter + 1) % FFT_SIZE;
	outputs[OUT_OUTPUT].value = _BufferOut[_Counter];
}


struct gkaDiracSpectrumWidget : ModuleWidget {
	gkaDiracSpectrumWidget(Module *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/Module1.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));

		addParam(ParamWidget::create<RoundHugeBlackKnob>(Vec(18, 60), module, gkaDiracSpectrum::FREQ_PARAM, 0.0f, 5.0f, 1.0f));

	    addOutput(Port::create<PJ301MPort>(Vec(55, 270), Port::OUTPUT, module, gkaDiracSpectrum::OUT_OUTPUT));
	}
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelgkaDiracSpectrum = Model::create<gkaDiracSpectrum, gkaDiracSpectrumWidget>("GKaSynth", "gkaDiracSpectrum", "DiracSpectrum", OSCILLATOR_TAG);
