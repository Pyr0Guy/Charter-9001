#include "pch.hpp"

#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "include/raygui/raygui.h"
#undef RAYGUI_IMPLEMENTATION 

#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "include/raygui/gui_window_file_dialog.h"

#include "include/App.hpp"
#include "include/Audio.hpp"
#include "include/Constants.hpp"
#include "include/Conductor.hpp"
#include "include/ResourceManager.hpp"

#include "fmod.hpp"

#include "include/json.hpp"

bool paused = true;
GuiWindowFileDialogState m_FileDialog;

int CeilCoundOnTheScreen = 12;

App::App(unsigned int Width, unsigned int Height, const std::string& title)
{
	InitWindow(Width, Height, title.c_str());
	SetTargetFPS(60);

	m_Charts.reserve(2);
	m_ChartStartYPos = 100.f;

	m_RendableNotes.reserve(20);

	m_PlayHitsound = false;

	Audio::Init();

	m_ChartNames.push_back("oldMan");
	m_ChartNames.push_back("lad");

	Conductor::Init(77, 4, 4, Constants::SoundPath + "Episode Songs.bank", "e1-s1");

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

	std::string dir = std::string(GetWorkingDirectory()) + "\\assets";
	m_FileDialog = InitGuiWindowFileDialog(dir.c_str());

	Audio::LoadSound("assets/sounds/click.wav", "hitsound");

	ResourceManager::LoadTexture2D(Constants::ImagePath + "arrows_basic.png", "arrow");
	LoadAnimations();
}

void App::Update()
{
	if (m_FileDialog.windowActive == true)
	{
		Conductor::SetPause(true);
		paused = true;
	}

	m_MainCamera.target = { 0.f, m_LinePosition };
	unsigned int lastTime = Conductor::SongPosition;

	for (auto& chart : m_Charts)
	{
		for (auto& note : chart->GetAllNotes())
		{
			int NotePos = note->GetNoteData().notePosition;
			int susNoteEndPos = note->GetNoteData().isSustended ? note->GetNoteData().sustendedLen : 0;
			
			int upThing = Conductor::SongPosition - (Conductor::MSPerCell * 3);
			int downThing = upThing + (Conductor::MSPerCell * CeilCoundOnTheScreen);

			bool isNoteVisible = (NotePos + susNoteEndPos >= upThing) && (NotePos <= downThing);

			if (note->isRendered == false && isNoteVisible == true)
			{
				m_RendableNotes.push_back(note);
				note->isRendered = true;
			}
			else if (isNoteVisible == false && note->isRendered == true)
			{
				note->isRendered = false;
			}
		}
	}

	Audio::Update();
	Conductor::Update();

	m_WorldMousePos = GetScreenToWorld2D(GetMousePosition(), m_MainCamera);

	if (m_FileDialog.windowActive == false)
	{
		for (auto& chart : m_Charts)
			chart->Update(m_WorldMousePos);

		ConductorControll();
	}

	//Funne
	if (IsFileDropped())
	{
		FilePathList droppedFiles = LoadDroppedFiles();

		if (droppedFiles.count > 0 &&
			(IsFileExtension(droppedFiles.paths[0], ".jpg") || IsFileExtension(droppedFiles.paths[0], ".png") || IsFileExtension(droppedFiles.paths[0], ".jpeg")))
		{
			ResourceManager::DeleteTexture("bg");
			bg = nullptr;
			bg = new Sprite({ 0, 0 }, droppedFiles.paths[0], "bg", { 1.f, 1.f }, false, 1.f);
			bg->SetOrigin(Origin::TOP_LEFT);
			bg->SetSize({ Constants::WindowWidth, Constants::WindowHeight });
		}
		//bg->Update();

		UnloadDroppedFiles(droppedFiles);
	}

	//Export Chart
	if (m_FileDialog.SelectFilePressed)
	{
		ExportFuckingChartGodHelpUsAll();

		m_FileDialog.SelectFilePressed = false;
		memset(m_FileDialog.fileNameText, 0, 1024);
		memset(m_FileDialog.fileNameTextCopy, 0, 1024);
	}
	
	if (m_PlayHitsound == true && paused == false)
	{
		for (auto& note : m_RendableNotes)
		{
			const unsigned int notePos = note->GetNoteData().notePosition;

			if (Conductor::SongPosition > notePos && lastTime <= notePos)
				Audio::PlayOneshot("hitsound");
		}
	}

	float cellDuration = Conductor::MSPerBeat / 4.0f;
	float cell = Conductor::SongPosition / cellDuration;
	m_LinePosition = 100.f + (cell * Constants::GridHeight);

	for (size_t i = 0; i < m_RendableNotes.size(); i++)
	{
		if (m_RendableNotes[i]->isDeleted == true || m_RendableNotes[i]->isRendered == false)
			m_RendableNotes.erase(m_RendableNotes.begin() + i);
	}

	for (auto& chart : m_Charts)
	{
		std::vector<Note*>& notes = chart->GetAllNotes();
		for (int i = 0; i < notes.size(); i++)
		{
			if (notes[i]->isDeleted == true)
			{
				delete notes[i];
				notes[i] = nullptr;
				notes.erase(notes.begin() + i);
			}
		}
	}

	std::cout << m_RendableNotes.size() << " " << m_Charts[0]->GetAllNotes().size() << std::endl;
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

				for (auto& note : m_RendableNotes)
					note->Draw();

			EndMode2D();
		}

		std::string Pos = std::to_string(Conductor::SongPosition) + "/" + std::to_string(Conductor::SongMaxLenght);

		DrawLineEx({ 100, 100 }, { 662, 100 }, 2, RED);
		DrawText(Pos.c_str(), 0, 0, 24, ORANGE);

		if (m_FileDialog.windowActive) 
			GuiLock();

			if (GuiButton({ Constants::WindowWidth - 100.f, 30, 100, 30 }, "Export"))
			{
				m_FileDialog.windowActive = true;
				m_FileDialog.saveFileMode = true;
			}

			GuiCheckBox({ Constants::WindowWidth - 100.f, 300, 20, 20 }, "Hitsond", &m_PlayHitsound);

			GuiUnlock();

		GuiWindowFileDialog(&m_FileDialog);

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

		if (IsKeyPressed('Q'))
			Conductor::SetPosition(Conductor::SongMaxLenght - 2000);

		if (Conductor::SongPosition >= Conductor::SongMaxLenght - 1)
			Conductor::SetPosition(Conductor::SongMaxLenght);

		if (IsKeyPressed(KEY_ENTER))
			Conductor::SetPosition(0);

		if (GetMouseWheelMove() != 0)
			Conductor::SetPosition(Conductor::SongPosition - (int)GetMouseWheelMove() * (Constants::MouseWheelMult * 2));

		if (IsKeyDown(KEY_UP))
			Conductor::SetPosition(Conductor::SongPosition - Constants::MouseWheelMult);

		if (IsKeyDown(KEY_DOWN))
			Conductor::SetPosition(Conductor::SongPosition + Constants::MouseWheelMult);

		if (Conductor::SongPosition <= -1)
			Conductor::SetPosition(0);
	}
}

void App::ExportFuckingChartGodHelpUsAll()
{
	nlohmann::json superCoolData;
	superCoolData["songName"] = Conductor::GetSongName();
	superCoolData["bpm"] = Conductor::BPM;
	superCoolData["scrollSpeed"] = Conductor::SongSpeed;
	superCoolData["songLenght"] = Conductor::SongMaxLenght;
	
	for (auto& chart : m_Charts)
	{
		std::vector<Note*> notes = chart->GetAllNotes();
		std::string owner = chart->GetOwner();
		for (Note* note : notes)
		{
			Note::NoteData data = note->GetNoteData();
			superCoolData["notes"].emplace_back(nlohmann::json::object({
				{"time", data.notePosition},
				{"noteID", data.noteID},
				{"noteTurn", owner}
			}));
		}
	}

	std::string filePath = m_FileDialog.dirPathText + std::string("/") + m_FileDialog.fileNameText;
	
	char* stringPos = strstr(m_FileDialog.fileNameText, ".json");
	if (stringPos)
	{
		size_t fileNameLen = strlen(m_FileDialog.fileNameText);
		size_t fileExtensionLen = strlen(".json");

		memmove(stringPos, stringPos + fileNameLen, fileNameLen - (stringPos - m_FileDialog.fileNameText) - fileExtensionLen + 1);
	}

	filePath += ".json";

	/*
	std::ifstream fileRead(filePath);

	if (fileRead.good()) //File Already exsists so i need to warn user, meh someday later - 14.03.25
	{

	}

	fileRead.close();
	*/

	std::ofstream fileWrite(filePath);
	if (fileWrite.is_open())
	{
		fileWrite << superCoolData.dump(4);
		fileWrite.close();
	}
}