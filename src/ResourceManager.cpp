#include "pch.hpp"
#include "include/ResourceManager.hpp"
#include "include/Audio.hpp"

std::unordered_map<std::string, FMOD::Sound*> ResourceManager::m_Sounds;
std::unordered_map<std::string, FMOD::Channel*> ResourceManager::m_Channels;
std::unordered_map<std::string, FMOD::Studio::EventInstance*> ResourceManager::m_EventInstances;
std::unordered_map<std::string, Texture2D>	ResourceManager::m_Textures;

FMOD::Sound* ResourceManager::LoadSound(const std::string& name, FMOD::Sound* sound)
{
	m_Sounds[name] = sound;
	return m_Sounds[name];
}

FMOD::Sound* ResourceManager::GetSound(const std::string& name)
{
	return m_Sounds[name];
}

FMOD::Channel* ResourceManager::LoadChannel(const std::string& name, FMOD::Channel* sound)
{
	m_Channels[name] = sound;
	return m_Channels[name];
}

FMOD::Channel* ResourceManager::GetChannel(const std::string& name)
{
	return m_Channels[name];
}

bool ResourceManager::CheckIfChannelExsists(const std::string& name)
{
	return m_Channels.find(name) != m_Channels.end();
}

void ResourceManager::LoadEventInstance(const std::string& name, FMOD::Studio::EventInstance* env)
{
	m_EventInstances[name] = env;
}

FMOD::Studio::EventInstance* ResourceManager::GetEventInstance(const std::string& name)
{
	return m_EventInstances[name];
}

bool ResourceManager::CheckIfInstanceExsists(const std::string& name)
{
	return m_EventInstances.find(name) != m_EventInstances.end();
}

void ResourceManager::LoadTexture2D(const std::string& file, const std::string& name)
{
	if (m_Textures.find(name) == m_Textures.end())
		m_Textures[name] = LoadTexture(file.c_str());
}

Texture2D ResourceManager::GetTexture(const std::string& name)
{
	return m_Textures[name];
}