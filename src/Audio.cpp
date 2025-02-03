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
		LoadSongLowLevel(path, fileNameNoExtension);
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
		ERRCHECK(instance->setPaused(pause));
	else
		std::cout << "Audio.cpp. Pause Function: There is no " << songName << " for pause" << std::endl;
}

unsigned int Audio::GetSongPosition(const std::string& songName)
{
	unsigned int pos;

	if (auto* channel = ResourceManager::GetChannel(songName))
		ERRCHECK(channel->getPosition(&pos, FMOD_TIMEUNIT_MS));
	else if (auto* instance = ResourceManager::GetEventInstance(songName))
		ERRCHECK(instance->getTimelinePosition((int*)&pos));
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
		ERRCHECK(envDesc->getLength((int*)&pos));
	}
	else
	{
		std::cout << "There is no: " << songName << std::endl;
		return 0;
	}
		//channel->getPosition(&pos, FMOD_TIMEUNIT_MS
	return pos;
}

void Audio::Update()
{
	fmodSys->update();
}

void Audio::Destroy()
{
	masterBank->unload();
	fmodSys->release();
}

void ERRCHECK_fn(FMOD_RESULT result, const char* file, int line) {
	if (result != FMOD_OK)
		std::cout << "FMOD ERROR: Audio.cpp [Line " << line << "] " << result << "  - " << FMOD_ErrorString(result) << '\n';
}

void Audio::LoadSongLowLevel(const std::string& path, const std::string& SoundName)
{
	FMOD::Sound* sound;
	ERRCHECK(fmodSysLow->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &sound));
	ResourceManager::LoadSound(SoundName, sound);
}

void Audio::LoadSongHighLevel(const std::string& path, const std::string& eventName)
{
	if (m_masterBankLoaded == false)
	{
		ERRCHECK(fmodSys->loadBankFile((Constants::SoundPath + "Master.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));
		ERRCHECK(fmodSys->loadBankFile((Constants::SoundPath + "Master.strings.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));
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

	FMOD::Studio::EventInstance* env = nullptr;
	ERRCHECK(envDesc->createInstance(&env));
	ERRCHECK(env->start());
	ResourceManager::LoadEventInstance(eventName, env);
}