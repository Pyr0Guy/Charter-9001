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
	m_DragableNote = nullptr;
	
	m_isDraging = false;

	ResourceManager::LoadTexture2D(Constants::ImagePath + "arrows_basic.png", "arrow");
}

void App::Update()
{
	Audio::Update();
	Conductor::Update();

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

		if(GetMouseWheelMove() != 0)
			Conductor::SetPosition(Conductor::SongPosition - (int)GetMouseWheelMove() * Constants::MouseWheelMult);

		if(IsKeyDown(KEY_UP))
			Conductor::SetPosition(Conductor::SongPosition - Constants::MouseWheelMult);

		if (IsKeyDown(KEY_DOWN))
			Conductor::SetPosition(Conductor::SongPosition + Constants::MouseWheelMult);

		if (Conductor::SongPosition <= -1)
			Conductor::SetPosition(0);
	}

	for (auto& cell : m_CeilVector)
	{
		cell->Update({ 0.f, m_LinePosition - 100.f });

		if (cell->GetActive() == true)
			m_CurCell = cell->ReturnCell();
	}

	Vector2 worldMousePos = GetScreenToWorld2D(GetMousePosition(), m_MainCamera);
	float deltaY = worldMousePos.y - m_DragStartPos.y;

	if (m_CurCell != nullptr)
	{
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			for (auto& note : m_NoteList)
			{
				if (note->isMouseOverEndSus(worldMousePos))
				{
					m_DragableNote = note;
					m_DragEndNote = true;
					m_isDraging = true;
					m_DragStartPos = worldMousePos;
					break;
				}
			}

			if (m_DragableNote == nullptr && m_CurCell->m_WithArrow == true)
			{
				m_DragableNote = m_CurCell->m_NoteRef;
				m_DragEndNote = false;
				m_isDraging = true;
				m_DragStartPos = worldMousePos;
			}

		}

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && m_isDraging == true)
		{
			Vector2 currentMousePos = worldMousePos;
			float deltaY;

			if (m_DragEndNote == false)
				deltaY = currentMousePos.y - m_DragStartPos.y;
			else
				deltaY = currentMousePos.y - (Constants::GridHeight * 2); //IDK how it work

			int cells = static_cast<int>(deltaY / Constants::GridHeight);

			cells = std::max(0, cells);
			
			if(cells > 0)
				m_DragableNote->SetSutended(true);
			else
				m_DragableNote->SetSutended(false);

			if (m_DragableNote != nullptr) {
				m_DragableNote->SetSustainLength(cells);
			}
		}

		if (m_isDraging && IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) 
		{
			m_isDraging = false;
			m_DragableNote = nullptr;
			m_DragEndNote = false;
		}

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

	for (auto& note : m_NoteList)
		note->Update();

	if (m_isDraging == true && GetMousePosition().y >= Constants::WindowHeight - 100)
	{
		Conductor::SetPosition(Conductor::GetPosition() + 20);
	}

	if (m_isDraging == true && GetMousePosition().y <= 100 && m_isDraging == true)
	{
		Conductor::SetPosition(Conductor::GetPosition() - 20);
	}

	float cellDuration = Conductor::MSPerBeat / 4.0f;
	float cell = Conductor::SongPosition / cellDuration;
	m_LinePosition = 100.f + (cell * Constants::GridHeight);
}

void App::Draw()
{
	BeginDrawing();

	if (Audio::isLoaded() == true)
	{
		BeginMode2D(m_MainCamera);

			ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

			for (auto& cell : m_CeilVector)
				cell->Draw();

			for (auto& note : m_NoteList)
				note->Draw();


		EndMode2D();
	}

		std::string Pos = std::to_string(Conductor::SongPosition) + "/" + std::to_string(Conductor::SongMaxLenght);

		DrawLineEx({ 50, 100 }, { 500, 100 }, 2, RED);
		DrawText(Pos.c_str(), 0, 0, 24, ORANGE);

		DrawFPS(750, 0);

	EndDrawing();
}

void App::Destroy()
{
	for (auto& cell : m_CeilVector)
	{
		cell->Destroy();
		cell = nullptr;
	}

	m_CeilVector.clear();

	for (auto& note : m_NoteList)
	{
		delete note;
		note = nullptr;
	}

	m_NoteList.clear();

	delete m_CurCell;
	m_CurCell = nullptr;

	Audio::Destroy();
	CloseWindow();
}

bool App::IsClosed()
{
	return WindowShouldClose();
}
