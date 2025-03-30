#include "pch.hpp"
#include "include/Audio.hpp"

#include "include/ResourceManager.hpp"
#include "include/Constants.hpp"

#include <fmod_studio.hpp>
#include <fmod_errors.h>

void ERRCHECK_fn(FMOD_RESULT result, const char* file, int line);
#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__)

FMOD::Studio::System*	Audio::fmodSys = nullptr;
FMOD::System*			Audio::fmodSysLow = nullptr;

FMOD::Studio::Bank*		Audio::masterBank = nullptr;

bool					Audio::m_masterBankLoaded = false;
std::vector<FMOD::Studio::EventInstance*> Audio::m_PauseVector;

void Audio::Init()
{
	ERRCHECK(FMOD::Studio::System::create(&fmodSys));
	ERRCHECK(fmodSys->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0));
	ERRCHECK(fmodSys->getCoreSystem(&fmodSysLow));
}

void Audio::StartSong(const std::string& path, const std::string& eventName)
{
	const std::string fileName = path.substr(path.find_last_of('/') + 1);
	const std::string fileNameNoExtension = fileName.substr(0 ,fileName.find('.'));
	const std::string fileExtension = fileName.substr(fileName.find('.') + 1);

	if (fileExtension != "bank" && ResourceManager::CheckIfChannelExsists(fileNameNoExtension) == false)
	{
		LoadSound(path, fileNameNoExtension);
		FMOD::Channel* newChannel;
		ERRCHECK(fmodSysLow->playSound(ResourceManager::GetSound(fileNameNoExtension), nullptr, true, &newChannel));
		ResourceManager::LoadChannel(fileNameNoExtension, newChannel);
		ResourceManager::GetChannel(fileNameNoExtension)->setPaused(false);
	}
	
	if (fileExtension == "bank" && ResourceManager::CheckIfInstanceExsists(eventName) == false)
	{
		if (eventName == "")
		{
			std::cerr << "Event Name is empty put there something" << std::endl;
			return;
		}

		LoadSongHighLevel(path, eventName);
	}
}

void Audio::Pause(const std::string& songName, bool pause)
{
	if (auto* channel = ResourceManager::GetChannel(songName))
		ERRCHECK(channel->setPaused(pause));
	else if (auto* instance = ResourceManager::GetEventInstance(songName))
	{
		//ERRCHECK(instance->setPaused(pause));

		FMOD::ChannelGroup* cg = nullptr;
		ERRCHECK(instance->getChannelGroup(&cg));
		ERRCHECK(cg->setPaused(pause));
	}
	else
		std::cout << "Audio.cpp. Pause Function: There is no " << songName << " for pause" << std::endl;
}

unsigned int Audio::GetSongPosition(const std::string& songName)
{
	unsigned int pos;

	if (auto* channel = ResourceManager::GetChannel(songName))
		ERRCHECK(channel->getPosition(&pos, FMOD_TIMEUNIT_MS));
	else if (auto* instance = ResourceManager::GetEventInstance(songName))
	{
		FMOD::ChannelGroup* cg = nullptr;
		ERRCHECK(instance->getChannelGroup(&cg));

		FMOD::Channel* chan = FindSoundChannel(cg);
		ERRCHECK(chan->getPosition(&pos, FMOD_TIMEUNIT_MS));
	}
	else
	{
		std::cout << "There is no: " << songName << std::endl;
		return 0;
	}

	return pos;
}

unsigned int Audio::GetSongLength(const std::string& songName)
{
	unsigned int pos;

	if (auto* sound = ResourceManager::GetSound(songName))
		ERRCHECK(sound->getLength(&pos, FMOD_TIMEUNIT_MS));
	else if (auto* instance = ResourceManager::GetEventInstance(songName))
	{
		FMOD::Studio::EventDescription* envDesc;
		ERRCHECK(instance->getDescription(&envDesc));
		int tmpPos;
		ERRCHECK(envDesc->getLength(&tmpPos));
		pos = static_cast<unsigned int>(tmpPos);
	}
	else
	{
		std::cout << "There is no: " << songName << std::endl;
		return 0;
	}
		//channel->getPosition(&pos, FMOD_TIMEUNIT_MS
	return pos;
}

void Audio::SetPosition(const std::string& songName, unsigned int Position)
{
	if (Position >= 0 && Position <= Audio::GetSongLength(songName))
	{
		if (auto* sound = ResourceManager::GetChannel(songName))
		{
			ERRCHECK(sound->setPosition(Position, FMOD_TIMEUNIT_MS));
		}
		else if (auto* instance = ResourceManager::GetEventInstance(songName))
		{
			//ERRCHECK(instance->setTimelinePosition(static_cast<int>(Position)));
			FMOD::ChannelGroup* cg = nullptr;
			ERRCHECK(instance->getChannelGroup(&cg));

			std::vector<FMOD::Channel*> chans = FindSoundChannels(cg);

			for (auto chan : chans)
				chan->setPosition(Position, FMOD_TIMEUNIT_MS);

		}
		else
			std::cout << "There is no: " << songName << std::endl;
	}
}

void Audio::Update()
{
	fmodSys->update();

	if (!m_PauseVector.empty())
	{
		FMOD::Studio::EventInstance* instance = m_PauseVector.back();
		FMOD::Studio::EventDescription* envDesc = nullptr;

		if (instance->getDescription(&envDesc) == FMOD_OK && envDesc)
		{
			FMOD_STUDIO_LOADING_STATE state = FMOD_STUDIO_LOADING_STATE_ERROR;

			if (envDesc->getSampleLoadingState(&state) == FMOD_OK &&
				state == FMOD_STUDIO_LOADING_STATE_LOADED)
			{
				char path[512];
				int retrieved;
				envDesc->getPath(path, sizeof(path), &retrieved);
				std::string realName = path;
				realName = realName.substr(realName.find_first_of('/') + 1);
				Audio::Pause(realName, true);
				m_PauseVector.pop_back();

			}
		}
	}
}

void Audio::Destroy()
{
	masterBank->unload();
	fmodSys->release();
}

bool Audio::isLoaded()
{
	return m_PauseVector.empty();
}

void Audio::LoadSound(const std::string& path, const std::string& name)
{
	FMOD::Sound* sound = nullptr;
	ERRCHECK(fmodSysLow->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &sound));
	ResourceManager::LoadSound(name, sound);
}

void Audio::PlayOneshot(const std::string& name)
{
	fmodSysLow->playSound(ResourceManager::GetSound(name), nullptr, false, nullptr);
}

void ERRCHECK_fn(FMOD_RESULT result, const char* file, int line) {
	if (result != FMOD_OK)
		std::cout << "FMOD ERROR: Audio.cpp [Line " << line << "] " << result << "  - " << FMOD_ErrorString(result) << '\n';
}

void Audio::LoadSongHighLevel(const std::string& path, const std::string& eventName)
{
	if (m_masterBankLoaded == false)
	{
		ERRCHECK(fmodSys->loadBankFile((Constants::MusicPath + "Master.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));
		ERRCHECK(fmodSys->loadBankFile((Constants::MusicPath + "Master.strings.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));
		m_masterBankLoaded = true;
	}

	ERRCHECK(fmodSys->loadBankFile(path.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));

	//StudioSongData lol;
	//ResourceManager::LoadStudioSongData(eventName, &lol);

	//std::string event = "event:/" + eventName;
	//fmodSys->getEvent(event.c_str(), &ResourceManager::GetStudioSongData(eventName)->eventDesc);
	//ResourceManager::GetStudioSongData(eventName)->eventDesc->createInstance(&ResourceManager::GetStudioSongData(eventName)->engine);
	//ResourceManager::GetStudioSongData(eventName)->engine->start();

	FMOD::Studio::EventDescription* envDesc = nullptr;
	std::string event = "event:/" + eventName;

	ERRCHECK(fmodSys->getEvent(event.c_str(), &envDesc));

	if (envDesc)
	{
		FMOD::Studio::EventInstance* eventInstance = nullptr;

		ERRCHECK(envDesc->createInstance(&eventInstance));
		ERRCHECK(eventInstance->setCallback(nullptr));
		ERRCHECK(eventInstance->start());

		m_PauseVector.push_back(eventInstance);

		ResourceManager::LoadEventInstance(eventName, eventInstance);
	}
}


FMOD::Channel* Audio::FindSoundChannel(FMOD::ChannelGroup* channelGroup)
{
	if (!channelGroup)
	{
		return nullptr;
	}

	FMOD::Channel* chan = nullptr;
	FMOD::Sound* sound = nullptr;

	int numChannels = 0;
	if (channelGroup->getNumChannels(&numChannels) == FMOD_OK && numChannels > 0)
	{
		for (int i = 0; i < numChannels; ++i)
		{
			if (channelGroup->getChannel(i, &chan) == FMOD_OK)
			{
				if (chan->getCurrentSound(&sound) == FMOD_OK && sound)
				{
					return chan;
				}
			}
		}
	}

	int numGroups = 0;
	if (channelGroup->getNumGroups(&numGroups) == FMOD_OK && numGroups > 0)
	{
		FMOD::ChannelGroup* child = nullptr;
		
		for (int i = 0; i < numGroups; ++i)
		{
			if (channelGroup->getGroup(i, &child) == FMOD_OK && child)
			{
				chan = FindSoundChannel(child);
				if (chan)
				{
					return chan;
				}
			}
		}
	}

	return nullptr;
}

std::vector<FMOD::Channel*> Audio::FindSoundChannels(FMOD::ChannelGroup* channelGroup)
{
	if (!channelGroup)
	{
		return {};
	}

	std::vector<FMOD::Channel*> channelVector;

	FMOD::Channel* chan = nullptr;
	FMOD::Sound* sound = nullptr;

	int numChannels = 0;
	if (channelGroup->getNumChannels(&numChannels) == FMOD_OK && numChannels > 0)
	{
		for (int i = 0; i < numChannels; ++i)
		{
			if (channelGroup->getChannel(i, &chan) == FMOD_OK)
			{
				if (chan->getCurrentSound(&sound) == FMOD_OK && sound)
				{
					channelVector.push_back(chan);
				}
			}
		}
	}

	int numGroups = 0;
	if (channelGroup->getNumGroups(&numGroups) == FMOD_OK && numGroups > 0)
	{
		FMOD::ChannelGroup* child = nullptr;

		for (int i = 0; i < numGroups; ++i)
		{
			if (channelGroup->getGroup(i, &child) == FMOD_OK && child)
			{
				chan = FindSoundChannel(child);
				if (chan)
				{
					channelVector.push_back(chan);
				}
			}
		}
	}

	//std::cout << channelVector.size() << std::endl;
	return channelVector;
}
