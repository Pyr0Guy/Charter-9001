#include "pch.hpp"
#include "include/Audio.hpp"

#include "include/ResourceManager.hpp"

#include <fmod_studio.hpp>
#include <fmod_errors.h>

void ERRCHECK_fn(FMOD_RESULT result, const char* file, int line);
#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__)

FMOD::Studio::System* fmodSys = nullptr;
FMOD::System* fmodSysLow = nullptr;

FMOD::Studio::Bank* masterBank = nullptr;

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

	if (!ResourceManager::CheckIfChannelExsists(fileNameNoExtension))
	{
		if (fileExtension != "bank")
		{
			LoadSongLowLevel(path, fileNameNoExtension);
			FMOD::Channel* newChannel;
			ERRCHECK(fmodSysLow->playSound(ResourceManager::GetSound(fileNameNoExtension), nullptr, true, &newChannel));
			ResourceManager::LoadChannel(fileNameNoExtension, newChannel);
			ResourceManager::GetChannel(fileNameNoExtension)->setPaused(false);
		}
		else
		{
			if (eventName == "")
			{
				std::cerr << "Event Name is empty put there something" << std::endl;
				return;
			}

			LoadSongHighLevel(path, eventName);

			ERRCHECK(fmodSys->loadBankFile("assets/music/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));
			ERRCHECK(fmodSys->loadBankFile("assets/music/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));
			ERRCHECK(fmodSys->loadBankFile(path.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));

			//StudioSongData lol;
			//ResourceManager::LoadStudioSongData(eventName, &lol);

			//std::string event = "event:/" + eventName;
			//fmodSys->getEvent(event.c_str(), &ResourceManager::GetStudioSongData(eventName)->eventDesc);
			//ResourceManager::GetStudioSongData(eventName)->eventDesc->createInstance(&ResourceManager::GetStudioSongData(eventName)->engine);
			//ResourceManager::GetStudioSongData(eventName)->engine->start();

			FMOD::Studio::EventDescription* envDesc = nullptr;
			std::string event = "event:/" + eventName;
			
			fmodSys->getEvent(event.c_str(), &envDesc);

			FMOD::Studio::EventInstance* env = nullptr;
			envDesc->createInstance(&env);
			env->start();
			ResourceManager::LoadEventInstance(eventName, env);
		}
	}
}

void Audio::Pause(const std::string& songName, bool pause)
{
	ERRCHECK(ResourceManager::GetEventInstance(songName)->setPaused(pause));
}

void Audio::Update()
{
	ERRCHECK(fmodSys->update());
}

void Audio::Destroy()
{
	ERRCHECK(fmodSys->release());
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
}