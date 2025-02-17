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

	for (int j = 0; j < Constants::MaxRows; j++)
	{
		for (size_t i = 0; i < columFuck; i++)
		{
			Color c = (i + j) % 2 == 0 ? BLACK : GRAY;

			m_CeilVector.emplace_back(new Cell(j, i, j, i, c));
		}
	}

	m_LinePosition = 100.f;

	m_MainCamera.target = { 0.f, m_LinePosition };
	m_MainCamera.rotation = 0.f;
	m_MainCamera.zoom = 1.f;
	m_MainCamera.offset = { 0.f , 100.f };

	m_CurCell = nullptr;

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
	{
		cell->Update({ 0.f, m_LinePosition - 100.f });

		if (cell->GetActive() == true)
			m_CurCell = cell->ReturnCell();
	}

	if (m_CurCell != nullptr)
	{
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			if (m_CurCell->m_WithArrow == false)
			{
				Note::NoteData lol = {};
				lol.noteID = m_CurCell->ReturnID();
				lol.isSustended = false;
				lol.notePosition = m_CurCell->cellColum * (Conductor::MSPerBeat / 4);
				lol.sustendedLen = 0;

				m_NoteList.emplace_back(new Note(lol, m_CurCell->GetPosition()));
				m_NoteList.back()->Update();

				m_CurCell->m_NoteRef = m_NoteList.back();
				m_CurCell->m_WithArrow = true;
			}
			else
			{
				auto it = std::find(m_NoteList.begin(), m_NoteList.end(), m_CurCell->m_NoteRef);

				m_CurCell->m_NoteRef = nullptr;
				m_CurCell->m_WithArrow = false;

				if (it != m_NoteList.end())
				{
					delete* it;
					m_NoteList.erase(it);
				}
			}
		}
	}

	float cellDuration = Conductor::MSPerBeat / 4.0f;
	float cell = Conductor::SongPosition / cellDuration;
	m_LinePosition = 100.f + (cell * Constants::GridHeight);
}

void App::Draw()
{
	BeginDrawing();

		BeginMode2D(m_MainCamera);

			ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

			for (auto& cell : m_CeilVector)
				cell->Draw();

			for (auto& note : m_NoteList)
				note->Draw();


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

	for (auto& note : m_NoteList)
		delete note;

	m_NoteList.clear();

	delete m_CurCell;

	Audio::Destroy();
	CloseWindow();
}

bool App::IsClosed()
{
	return WindowShouldClose();
}
