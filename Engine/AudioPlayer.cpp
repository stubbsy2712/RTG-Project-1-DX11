#include "AudioPlayer.h"

//AudioPlayer::AudioPlayer()
//{
//	IXAudio2* pXAudio2 = NULL;
//	pXAudio2->CreateSourceVoice();
//	HRESULT hr;
//	if (FAILED(hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
//		return hr;
//}
//void AudioPlayer::playAudio()
//{
//	audioBuffer->LoopBegin();
//}
//void AudioPlayer::pauseAudio()
//{
//	
//}
//void AudioPlayer::loadFile(const std::wstring& filename, ...)
//{
//	// stream index
//	DWORD streamIndex = (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM;
//
//	// create the source reader
//	//...
//
//		// select the first audio stream, and deselect all other streams
//		sourceReader->SetStreamSelection((DWORD)MF_SOURCE_READER_ALL_STREAMS, false);
//	//if (FAILED(hr))
//	//	return std::runtime_error("Critical error: Unable to disable streams!");
//
//	sourceReader->SetStreamSelection(streamIndex, true);
//	//if (FAILED(hr))
//	//	return std::runtime_error("Critical error: Unable to enable first audio stream!");
//}