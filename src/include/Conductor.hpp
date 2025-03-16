#pragma once

class Conductor
{
public:

	static void Init(int bpm, int TopNum, int BottomNum, const std::string& SongPath, const std::string& evetName = "");
	static void Update();

	static void SetPause(bool pause);
	static bool GetPause();

	static void SetPosition(unsigned int Position);
	static unsigned int GetPosition();

	static std::string GetSongName();

	static int GetTopNum();
	static int GetBottomNum();

	static float SongSpeed;

	static int SongMaxLenght;
	static int SongPosition;
	static int SongPosInBeat;

	static int MSPerCell;

	static int BPM;

	static float MSPerBeat;
private:
	static int m_TopNumber;
	static int m_BottomNumber;

	static std::string m_songName;

	static bool m_inPause;
};