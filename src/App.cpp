#include "pch.hpp"

#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"

#include "include/App.hpp"
#include "include/Audio.hpp"
#include "include/Constants.hpp"

#include "fmod.hpp"

bool paused = false;
bool songLoad = false;

App::App(unsigned int Width, unsigned int Height, const std::string& title)
{
	InitWindow(Width, Height, title.c_str());
	SetTargetFPS(60);

	Audio::Init();
}

void App::Update()
{
	Audio::Update();


	if (IsKeyPressed('Z'))
	{
		Audio::StartSong(Constants::SoundPath + "test.wav", "Song Tutorial (Sun shining through)");
		songLoad = true;
	}

	if (IsKeyPressed('X'))
	{
		paused = !paused;
		Audio::Pause("Song Tutorial (Sun shining through)", paused);
	}

	if(songLoad == true)
		std::cout << Audio::GetSongLength("test") << std::endl;
}

void App::Draw()
{
	BeginDrawing();

		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

	EndDrawing();
}

void App::Destroy()
{
	Audio::Destroy();
	CloseWindow();
}

bool App::IsClosed()
{
	return WindowShouldClose();
}
