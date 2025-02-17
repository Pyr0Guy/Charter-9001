#include "pch.hpp"

#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"
#include "raymath.h"

#include "include/Cell.hpp"
#include "include/App.hpp"
#include "include/Audio.hpp"
#include "include/Constants.hpp"
#include "include/Conductor.hpp"
#include "include/ResourceManager.hpp"

#include "fmod.hpp"

bool paused = true;

App::App(unsigned int Width, unsigned int Height, const std::string& title)
{
	InitWindow(Width, Height, title.c_str());
	SetTargetFPS(60);

	Audio::Init();

	Conductor::Init(96, 4, 4, Constants::SoundPath + "Episode Songs.bank", "e1-s1");

	int columFuck = static_cast<int>(std::ceil(Conductor::SongMaxLenght / (Conductor::MSPerBeat / 4.0f)));

	for (int j = 0; j < Constants::MaxColumns; j++)
	{
		for (size_t i = 0; i < columFuck; i++)
		{
			Color c = (i + j) % 2 == 0 ? BLACK : GRAY;

			m_CeilVector.emplace_back(new Cell(j, i, j, c));
		}
	}

	m_LinePosition = 100.f;

	m_MainCamera.target = { 0.f, m_LinePosition };
	m_MainCamera.rotation = 0.f;
	m_MainCamera.zoom = 1.f;
	m_MainCamera.offset = { 0.f , 100.f };

	ResourceManager::LoadTexture2D(Constants::ImagePath + "arrows_basic.png", "arrow");
}

void App::Update()
{
	Audio::Update();
	Conductor::Update();

	if(IsKeyPressed(KEY_SPACE))
	{
		paused = !paused;
		Conductor::SetPause(paused);
	}

	m_MainCamera.target = { 0.f, m_LinePosition };

	if (IsKeyPressed('Q'))
		Conductor::SetPosition(Conductor::SongMaxLenght - 2000);

	if (Conductor::SongPosition >= Conductor::SongMaxLenght - 1)
		Conductor::SetPosition(Conductor::SongMaxLenght);

	if (IsKeyPressed(KEY_ENTER))
		Conductor::SetPosition(0);

	for (auto& cell : m_CeilVector)
		cell->Update({0.f, m_LinePosition - 100.f});

	float cellDuration = Conductor::MSPerBeat / 4.0f;
	float cell = Conductor::SongPosition / cellDuration;
	m_LinePosition = 100.f + (cell * Constants::GridHeight);
}

void App::Draw()
{
	BeginDrawing();

		BeginMode2D(m_MainCamera);

			ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

			for (int j = 0; j < m_CeilVector.size(); j++)
			{
				m_CeilVector[j]->Draw();
			}


		EndMode2D();

		std::string Pos = std::to_string(Conductor::SongPosition) + "/" + std::to_string(Conductor::SongMaxLenght);

		DrawLineEx({ 50, 100 }, { 500, 100 }, 2, RED);
		DrawText(Pos.c_str(), 0, 0, 24, ORANGE);

	EndDrawing();
}

void App::Destroy()
{
	for (auto& cell : m_CeilVector)
		cell->Destroy();

	Audio::Destroy();
	CloseWindow();
}

bool App::IsClosed()
{
	return WindowShouldClose();
}
