#pragma once

#include "fmod_studio.hpp"

class Audio
{
private:
	static void LoadSongLowLevel(const std::string& path, const std::string& SoundName);
	static void LoadSongHighLevel(const std::string& path, const std::string& eventName);

public:
	static void Init();

	static void StartSong(const std::string& path, const std::string& eventName = "");
	static void Pause(const std::string& songName ,bool pause);

	static unsigned int GetSongPosition(const std::string& songName);
	static unsigned int GetSongLength(const std::string& songName);

	static void Update();
	static void Destroy();

private:
	static bool m_masterBankLoaded;

	static FMOD::Studio::System* fmodSys;
	static FMOD::System* fmodSysLow;

	static FMOD::Studio::Bank* masterBank;
};	