#include "GKaSynth.hpp"
#include <complex>
#include <deque>
#include <numeric>
#include "GKaUtility.hpp"

std::string NoteGregorianStr[]={
	 "C",
	 "C#",
	 "D",
	 "D#",
	 "E",
	 "F",
	 "F#",
	 "G",
	 "G#",
	 "A",
	 "A#",
	 "B"
};




std::string NoteUtility::convert_note_to_str(const int note)
{
	return NoteGregorianStr[note];
}

void NoteUtility::freq_to_note(const float freq,int &octave,int &note)
{
	//by convention minimum is -8 for octave (due to the log computing)
	//no up limit
	if(freq<1.021977)
	{
		octave=-8;
		note=0;
	} else
	{
		float cv=log2f(freq / 261.626f);
		cv_to_note(cv,octave,note);
	}
}

//note : 0 to 11 - octave in gregorian convention (vcvrack convention)
void NoteUtility::cv_to_note(const float cv,int &octave,int &note)
{
	int octave_cv=(int)floor(cv);
	octave=4+octave_cv;
	note=(int)roundf(float(cv-octave_cv)*12.0);
	if(note==12)
	{
		++octave;
		note=0;
	}
}



//BlackMann-Harris tapering function
void dspUtility::init_window(float* &win,int size)
{

	win = (float*)malloc(size * sizeof(float));

    float a0 = 0.355768;
    float a1 = 0.487396;
    float a2 = 0.144232;
    float a3 = 0.012604;

	double phi = 2*M_PI/(double)(size-1);

	for (int i = 0; i < size; ++i)
	{
		win[i] = (float)(a0 - a1*cos( phi*(double)i )+ a2*cos( 2*phi*(double)i )- a3 * cos( 3*phi*(double)i ));
	}
}
