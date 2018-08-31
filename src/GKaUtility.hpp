struct NoteUtility
{
	static void freq_to_note(const float freq,int &octave,int &note);
	static void cv_to_note(const float cv,int &octave,int &note);
	static std::string convert_note_to_str(const int note);
};

struct dspUtility
{
	static void init_window(float* &win,int size); //BlackMann-Harris window
};
