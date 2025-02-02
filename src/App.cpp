#include "pch.hpp"

#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"

#include "include/App.hpp"
#include "include/Audio.hpp"
#include "include/Constants.hpp"

#include "fmod.hpp"

bool paused = false;

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
		Audio::StartSong(Constants::SoundPath + "Music.bank", "Song Tutorial (Sun shining through)");

	if (IsKeyPressed('X'))
	{
		paused =! paused;
		Audio::Pause("Song Tutorial (Sun shining through)", paused);
	}
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
