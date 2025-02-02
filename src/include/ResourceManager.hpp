#pragma once

namespace FMOD {
	class Sound;
	class Channel;
}

class ResourceManager
{
private:
	static std::unordered_map<std::string, FMOD::Sound*> m_Sounds;
	static std::unordered_map<std::string, FMOD::Channel*> m_Channels;

public:
	static FMOD::Sound* LoadSound(const std::string& name, FMOD::Sound* sound);
	static FMOD::Sound* GetSound(const std::string& name);

	static FMOD::Channel* LoadChannel(const std::string& name, FMOD::Channel* sound);
	static FMOD::Channel* GetChannel(const std::string& name);
	static bool CheckIfChannelExsists(const std::string& name);
};