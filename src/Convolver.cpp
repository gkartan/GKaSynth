#include "GKaSynth.hpp"
#include <complex>
#include "pffft.h"

#define FFT_SIZE 2048
struct gkaConvolver : Module {

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
	gkaConvolver() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS),_nbEchBuffer(0)
	{
		_setupFFT = pffft_new_setup(FFT_SIZE, PFFFT_REAL);
		_buffer[0]=(float*)pffft_aligned_malloc(FFT_SIZE*sizeof(float));
		_buffer[1]=(float*)pffft_aligned_malloc(FFT_SIZE*sizeof(float));
		_bufferOut=(float*)pffft_aligned_malloc(FFT_SIZE*sizeof(float));
		_fft[0]=(float*)pffft_aligned_malloc(FFT_SIZE*sizeof(float));
		_fft[1]=(float*)pffft_aligned_malloc(FFT_SIZE*sizeof(float));
		_fftout=(float*)pffft_aligned_malloc(FFT_SIZE*sizeof(float));
		memset(_bufferOut,0,FFT_SIZE*sizeof(float));
	}
	void step() override;

	float* _buffer[2];
	int   _nbEchBuffer;

	float* _bufferOut;
	PFFFT_Setup* _setupFFT;

	float*	_fft[2];
	float*	_fftout;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void gkaConvolver::step() {

	if(_nbEchBuffer==FFT_SIZE)
	{
		//Calcul FFT Buffer1
		pffft_transform_ordered(_setupFFT, _buffer[0], _fft[0], 0, PFFFT_FORWARD);

		//Calcul FFT Buffer2
		pffft_transform_ordered(_setupFFT, _buffer[1], _fft[1], 0, PFFFT_FORWARD);

		//Convolution
		for(int i=0;i<FFT_SIZE;++i)
		{
			_fftout[i]=_fft[0][i]*_fft[1][i];
		}

		//Calcul FFT Inverse
		pffft_transform_ordered(_setupFFT, _fftout, _bufferOut, 0, PFFFT_BACKWARD);

		//Normalisation a revoir
		//pour N*N?
		for(int i=0;i<FFT_SIZE;++i)
		{
			_bufferOut[i]/=(FFT_SIZE*FFT_SIZE);
		}

		//WOLA


		_nbEchBuffer=0;
	}

	_buffer[0][_nbEchBuffer]=inputs[IN1_INPUT].value;
	_buffer[1][_nbEchBuffer]=inputs[IN2_INPUT].value;
	outputs[OUT_OUTPUT].value=_bufferOut[_nbEchBuffer++];
}


struct gkaConvolverWidget : ModuleWidget {
	gkaConvolverWidget(Module *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/Module1.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 30, 365)));

		addInput(Port::create<PJ301MPort>(Vec(10, 200), Port::INPUT, module, gkaConvolver::IN1_INPUT));
	    addInput(Port::create<PJ301MPort>(Vec(10, 270), Port::INPUT, module, gkaConvolver::IN2_INPUT));
	    addOutput(Port::create<PJ301MPort>(Vec(55, 270), Port::OUTPUT, module, gkaConvolver::OUT_OUTPUT));
	}
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelgkaConvolver = Model::create<gkaConvolver, gkaConvolverWidget>("GKaSynth", "gkaConvolver", "Convolver", MIXER_TAG);
