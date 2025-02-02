#pragma once

class Audio
{
public:
	static void Init();

	static void StartSong(const std::string& path);

	static void Update();
	static void Destroy();
private:
	static void LoadSongLowLevel(const std::string& path, const std::string& SoundName);
};	