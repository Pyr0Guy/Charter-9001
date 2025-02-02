#include "pch.hpp"

#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"

#include "include/App.hpp"
#include "include/Audio.hpp"
#include "include/Constants.hpp"
#include "include/ResourceManager.hpp"

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

	if (IsKeyPressed('X'))
		Audio::StartSong(Constants::SoundPath + "test.wav");

	if (IsKeyPressed('Z'))
	{
		paused = !paused;
		ResourceManager::GetChannel("test")->setPaused(paused);
	}

	if (ResourceManager::CheckIfChannelExsists("test"))
	{
		unsigned int pos;
		ResourceManager::GetChannel("test")->getPosition(&pos, FMOD_TIMEUNIT_MS);
		std::cout << pos << std::endl;
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
