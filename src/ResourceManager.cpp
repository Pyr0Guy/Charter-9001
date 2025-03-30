#include "pch.hpp"
#include "include/ResourceManager.hpp"
#include "include/Audio.hpp"
#include "include/tinyxml2.h"

std::unordered_map<std::string, FMOD::Sound*> ResourceManager::m_Sounds;
std::unordered_map<std::string, FMOD::Channel*> ResourceManager::m_Channels;
std::unordered_map<std::string, std::vector<FMOD::ChannelGroup*>> ResourceManager::m_ChannelGroups;
std::unordered_map<std::string, FMOD::Studio::EventInstance*> ResourceManager::m_EventInstances;
std::unordered_map<std::string, Texture2D>	ResourceManager::m_Textures;
std::unordered_map<std::string, std::unordered_map<std::string, ResourceManager::AnimationFrames>> ResourceManager::m_Animations;

FMOD::Sound* ResourceManager::LoadSound(const std::string& name, FMOD::Sound* sound)
{
	m_Sounds[name] = sound;
	return m_Sounds[name];
}

FMOD::Sound* ResourceManager::GetSound(const std::string& name)
{
    return m_Sounds.count(name) ? m_Sounds.at(name) : nullptr;
}

FMOD::Channel* ResourceManager::LoadChannel(const std::string& name, FMOD::Channel* sound)
{
	m_Channels[name] = sound;
	return m_Channels[name];
}

FMOD::Channel* ResourceManager::GetChannel(const std::string& name)
{
    return m_Channels.count(name) ? m_Channels.at(name) : nullptr;
}

bool ResourceManager::CheckIfChannelExsists(const std::string& name)
{
    return m_Channels.count(name);
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
    return m_EventInstances.count(name);
}

void ResourceManager::LoadTexture2D(const std::string& file, const std::string& name)
{
	if (m_Textures.find(name) == m_Textures.end())
		m_Textures[name] = LoadTexture(file.c_str());
}

Texture2D ResourceManager::GetTexture(const std::string& name)
{
    return m_Textures.count(name) ? m_Textures.at(name) : Texture2D();
}

void ResourceManager::DeleteTexture(const std::string& name)
{
    //UnloadTexture already use Raylib
    if (m_Textures.find(name) != m_Textures.end())
    {
        UnloadTexture(m_Textures[name]);
        m_Textures.erase(name);
    }
}

void ResourceManager::LoadAnimation(const std::string& xmlPath, const std::string& animName) {
    // Если анимация уже загружена - пропускаем
    if (m_Animations[xmlPath].count(animName)) return;

    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(xmlPath.c_str()) != tinyxml2::XML_SUCCESS) {
        throw std::runtime_error("Failed to load animation XML: " + xmlPath);
    }

    tinyxml2::XMLElement* root = doc.FirstChildElement("TextureAtlas");
    AnimationFrames frames;

    for (tinyxml2::XMLElement* elem = root->FirstChildElement("SubTexture"); elem; elem = elem->NextSiblingElement()) {
        std::string name = elem->Attribute("name");

        // Парсим имя анимации (пример: "walk0001" -> "walk")
        size_t numPos = name.find_first_of('0');
        if (numPos == std::string::npos) continue;

        size_t startPos = name.find_last_of(' ');

        std::string baseName = name.substr(startPos + 1, numPos - startPos - 1);
        if (baseName != animName) continue;

        frames.push_back({
            static_cast<float>(elem->IntAttribute("x")),
            static_cast<float>(elem->IntAttribute("y")),
            static_cast<float>(elem->IntAttribute("width")),
            static_cast<float>(elem->IntAttribute("height"))
        });
    }

    if (!frames.empty()) {
        m_Animations[xmlPath][animName] = frames;
    }
}

const ResourceManager::AnimationFrames& ResourceManager::GetAnimation(const std::string& xmlPath, const std::string& animName)
{
    if (!HasAnimation(xmlPath, animName)) {
        throw std::runtime_error("Animation not loaded: " + animName);
    }
    return m_Animations[xmlPath][animName];
}

bool ResourceManager::HasAnimation(const std::string& xmlPath, const std::string& animName)
{
	return m_Animations.count(xmlPath) && m_Animations[xmlPath].count(animName);
}



