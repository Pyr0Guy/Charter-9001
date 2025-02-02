#pragma once

namespace FMOD {
	class Sound;
	class Channel;

	namespace Studio {
		class EventInstance;
	}
}

struct StudioSongData;

class ResourceManager
{
private:
	static std::unordered_map<std::string, FMOD::Sound*> m_Sounds;
	static std::unordered_map<std::string, FMOD::Channel*> m_Channels;
	static std::unordered_map<std::string, StudioSongData*> m_StudioSongData;
	static std::unordered_map<std::string, FMOD::Studio::EventInstance*> m_EventInstances;

public:
	static FMOD::Sound* LoadSound(const std::string& name, FMOD::Sound* sound);
	static FMOD::Sound* GetSound(const std::string& name);

	static FMOD::Channel* LoadChannel(const std::string& name, FMOD::Channel* sound);
	static FMOD::Channel* GetChannel(const std::string& name);
	static bool CheckIfChannelExsists(const std::string& name);

	static void LoadStudioSongData(const std::string& name, StudioSongData* lol);
	static StudioSongData* GetStudioSongData(const std::string& name);

	static void LoadEventInstance(const std::string& name, FMOD::Studio::EventInstance* env);
	static FMOD::Studio::EventInstance* GetEventInstance(const std::string& name);
};