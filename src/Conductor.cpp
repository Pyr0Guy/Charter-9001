#include "pch.hpp"
#include "include/Conductor.hpp"
#include "include/Audio.hpp"

int Conductor::m_TopNumber = 0;
int Conductor::m_BottomNumber = 0;
int Conductor::BPM = 0;
int Conductor::SongMaxLenght = 0;
int Conductor::SongPosition = 0;
int Conductor::SongPosInBeat = 0;
int Conductor::MSPerCell = 0;

float Conductor::MSPerBeat = 0.f;

float Conductor::SongSpeed = 1.f;

std::string Conductor::m_songName = "";

bool Conductor::m_inPause = false;

void Conductor::Init(int bpm, int TopNum, int BottomNum, const std::string& SongPath, const std::string& evetName)
{
	BPM = bpm;
	m_TopNumber = TopNum;
	m_BottomNumber = BottomNum;

	if (evetName == "")
	{
		size_t startName = SongPath.find_last_of('/');
		std::string name = SongPath.substr(startName + 1, SongPath.find_last_of('.') - startName - 1);
		std::cout << name << std::endl;
		m_songName = name;
	}
	else
		m_songName = evetName;

	MSPerBeat = 60000.f / BPM;

	m_inPause = true;
	Audio::StartSong(SongPath, m_songName);

	SongMaxLenght = Audio::GetSongLength(m_songName);

	MSPerCell = MSPerBeat / 4.f;
}

void Conductor::Update()
{
	if (m_inPause == false)
	{
		SongPosition = Audio::GetSongPosition(m_songName);
		SongPosInBeat = SongPosition / MSPerBeat;
	}
}

void Conductor::SetPause(bool pause)
{
	m_inPause = pause;
	Audio::Pause(m_songName, m_inPause);
}

bool Conductor::GetPause()
{
	return m_inPause;
}

void Conductor::SetPosition(unsigned int Position)
{
	m_inPause = true;
	Audio::SetPosition(m_songName, Position);
	SongPosition = Position;
}

unsigned int Conductor::GetPosition()
{
	return Audio::GetSongPosition(m_songName);
}

std::string Conductor::GetSongName()
{
	return m_songName;
}

int Conductor::GetTopNum()
{
	return m_TopNumber;
}

int Conductor::GetBottomNum()
{
	return m_BottomNumber;
}

