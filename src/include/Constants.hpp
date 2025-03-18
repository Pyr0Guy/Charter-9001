#pragma once

class Constants
{
public:
	static constexpr unsigned int WindowWidth = 800;
	static constexpr unsigned int WindowHeight = 600;

	static constexpr unsigned int GridWidth = 64;
	static constexpr unsigned int GridHeight = 64;

	static constexpr unsigned int MaxRows = 4;

	static constexpr unsigned int MouseWheelMult = 32;

	static constexpr unsigned int FontSize = 24;
	
	static const std::string title;
	static const std::string SoundPath;
	static const std::string ImagePath;
	static const std::string MusicPath;
	static const std::string ChartPath;

	static std::array<std::string, 4> arrowsList;
};									   