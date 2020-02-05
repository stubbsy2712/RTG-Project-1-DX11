#pragma once
//#include <xaudio2.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

class AudioPlayer
{
public:
	AudioPlayer();
	void playAudio();
	void pauseAudio();
	//void loadAudio(string fileName);
private:
	//XAUDIO2_BUFFER* audioBuffer;
};

