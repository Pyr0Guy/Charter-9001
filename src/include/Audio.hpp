#pragma once

#include "fmod_studio.hpp"

class Audio
{
private:
	static void LoadSongHighLevel(const std::string& path, const std::string& eventName);

	static FMOD::Channel* FindSoundChannel(FMOD::ChannelGroup* channelGroup);
	static std::vector<FMOD::Channel*> FindSoundChannels(FMOD::ChannelGroup* channelGroup);

public:
	static void Init();

	static void StartSong(const std::string& path, const std::string& eventName = "");
	static void Pause(const std::string& songName ,bool pause);
	static void DeleteSound(const std::string& soundName, bool isInBank);

	static unsigned int GetSongPosition(const std::string& songName);
	static unsigned int GetSongLength(const std::string& songName);

	static void SetPosition(const std::string& songName, unsigned int Position);

	static void Update();
	static void Destroy();

	static bool isLoaded();

	static void LoadSound(const std::string& path, const std::string& name);
	static void PlayOneshot(const std::string& name);

private:
	static bool m_masterBankLoaded;

	static FMOD::Studio::System* fmodSys;
	static FMOD::System* fmodSysLow;

	static FMOD::Studio::Bank* masterBank;

	static std::vector<FMOD::Studio::EventInstance*> m_PauseVector;
};	