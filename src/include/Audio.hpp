#pragma once

#include "fmod_studio.hpp"

struct StudioSongData
{
	FMOD::Studio::EventDescription* eventDesc;
	FMOD::Studio::EventInstance* engine;
};

class Audio
{
public:
	static void Init();

	static void StartSong(const std::string& path, const std::string& eventName = "");
	static void Pause(const std::string& songName ,bool pause);

	static void Update();
	static void Destroy();
private:
	static void LoadSongLowLevel(const std::string& path, const std::string& SoundName);
	static void LoadSongHighLevel(const std::string& path, const std::string& eventName);
};	