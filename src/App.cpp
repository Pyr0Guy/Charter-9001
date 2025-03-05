#include "pch.hpp"

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

	m_Charts.reserve(2);
	m_ChartStartYPos = 100.f;

	Audio::Init();

	m_ChartNames.push_back("oldMan");
	m_ChartNames.push_back("lad");

	Conductor::Init(96, 4, 4, Constants::SoundPath + "Episode Songs.bank", "e1-s1");

	int columFuck = static_cast<int>(std::ceil(Conductor::SongMaxLenght / (Conductor::MSPerBeat / 4.0f)));

	for (size_t i = 0; i < m_ChartNames.size(); i++)
	{
		Vector2 pos = { 100 + (i * (Constants::GridWidth * Constants::MaxRows + 50)), m_ChartStartYPos };
		m_Charts.emplace_back(new ChartRegion(pos, m_ChartNames[i], columFuck));
	}

	bg = nullptr;
	m_LinePosition = m_ChartStartYPos;

	m_MainCamera.target = { 0.f, m_LinePosition };
	m_MainCamera.rotation = 0.f;
	m_MainCamera.zoom = 1.f;
	m_MainCamera.offset = { 0.f , 100.f };

	ResourceManager::LoadTexture2D(Constants::ImagePath + "arrows_basic.png", "arrow");
	LoadAnimations();
}

void App::Update()
{
	Audio::Update();
	Conductor::Update();

	m_WorldMousePos = GetScreenToWorld2D(GetMousePosition(), m_MainCamera);

	for (auto& chart : m_Charts)
		chart->Update(m_WorldMousePos);

	ConductorControll();

	//Funne
	if (IsFileDropped())
	{
		FilePathList droppedFiles = LoadDroppedFiles();

		if (droppedFiles.count > 0 &&
			(IsFileExtension(droppedFiles.paths[0], ".jpg") || IsFileExtension(droppedFiles.paths[0], ".png") || IsFileExtension(droppedFiles.paths[0], ".jpeg")))
		{
			ResourceManager::DeleteTexture("bg");
			bg = new Sprite({ 0, 0 }, droppedFiles.paths[0], "bg", { 1.f, 1.f }, false, 1.f);
			bg->SetOrigin(Origin::TOP_LEFT);
			bg->SetSize({ Constants::WindowWidth, Constants::WindowHeight });
		}

		//bg->Update();

		UnloadDroppedFiles(droppedFiles);
	}

	float cellDuration = Conductor::MSPerBeat / 4.0f;
	float cell = Conductor::SongPosition / cellDuration;
	m_LinePosition = 100.f + (cell * Constants::GridHeight);
}

void App::Draw()
{
	BeginDrawing();
		ClearBackground(WHITE);

		if(bg != nullptr)
			bg->Draw();

		if (Audio::isLoaded() == true)
		{
			BeginMode2D(m_MainCamera);

				for (auto& chart : m_Charts)
					chart->Draw();

			EndMode2D();
		}

		std::string Pos = std::to_string(Conductor::SongPosition) + "/" + std::to_string(Conductor::SongMaxLenght);

		DrawLineEx({ 100, 100 }, { 662, 100 }, 2, RED);
		DrawText(Pos.c_str(), 0, 0, 24, ORANGE);

		DrawFPS(750, 0);

	EndDrawing();
}

void App::Destroy()
{
	for (auto& chart : m_Charts)
	{
		delete chart;
		chart = nullptr;
	}

	m_Charts.clear();

	Audio::Destroy();
	CloseWindow();
}

bool App::IsClosed()
{
	return WindowShouldClose();
}


void App::LoadAnimations()
{
	for (int i = 0; i < Constants::arrowsList.size(); i++)
	{
		ResourceManager::LoadAnimation(Constants::ImagePath + "arrows_basic.xml", Constants::arrowsList[i] + "note");
		ResourceManager::LoadAnimation(Constants::ImagePath + "arrows_basic.xml", Constants::arrowsList[i] + "note_long");
		ResourceManager::LoadAnimation(Constants::ImagePath + "arrows_basic.xml", Constants::arrowsList[i] + "note_long_end");
	}
}

void App::ConductorControll()
{
	if (Audio::isLoaded() == true)
	{
		if (IsKeyPressed(KEY_SPACE))
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

		if (GetMouseWheelMove() != 0)
			Conductor::SetPosition(Conductor::SongPosition - (int)GetMouseWheelMove() * Constants::MouseWheelMult);

		if (IsKeyDown(KEY_UP))
			Conductor::SetPosition(Conductor::SongPosition - Constants::MouseWheelMult);

		if (IsKeyDown(KEY_DOWN))
			Conductor::SetPosition(Conductor::SongPosition + Constants::MouseWheelMult);

		if (Conductor::SongPosition <= -1)
			Conductor::SetPosition(0);
	}
}