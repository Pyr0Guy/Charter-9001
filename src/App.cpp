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
	m_RendableNotes.reserve(20);

	m_ChartStartYPos = 100.f;

	m_PlayHitsound = false;
	
	bg = nullptr;
	m_LinePosition = m_ChartStartYPos;
	m_bankFileContains = false;
	m_inTextbox = false;

	m_MainCamera.target = { 0.f, m_LinePosition };
	m_MainCamera.rotation = 0.f;
	m_MainCamera.zoom = 1.f;
	m_MainCamera.offset = { 0.f , 100.f };

	std::memset(m_eventName, 0, sizeof(m_eventName));
	std::memset(m_filePathSong, 0, sizeof(m_filePathSong));
	std::memset(m_bpmText, 0, sizeof(m_bpmText));
	std::memset(m_scrollSpeed, 0, sizeof(m_scrollSpeed));

	m_scrollSpeed[0] = '1';
	m_scrollSpeed[1] = '.';
	m_scrollSpeed[2] = '0';

	m_isThreeFour = false;
	m_FDstate = FileDialogeState::SaveExportedFiles;

	std::string dir = std::string(GetWorkingDirectory()) + "\\assets\\chart";//Constants::ChartPath;
	m_FileDialog = InitGuiWindowFileDialog(dir.c_str());

	Audio::Init();
	Audio::LoadSound(Constants::SoundPath + "click.wav", "hitsound");

	ResourceManager::LoadTexture2D(Constants::ImagePath + "arrows_basic.png", "arrow");
	LoadAnimations();

	RestartSong(114, { 3, 4 }, Constants::MusicPath + "Episode Songs.bank", "e0-s1");
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

			if (isNoteVisible == true && note->isRendered == false)
			{
				m_RendableNotes.push_back(note);
				note->isRendered = true;
			}
			else if (isNoteVisible == false)
			{
				note->isRendered = false;
			}
		}
	}

	if (m_fileBool == true)
	{
		const char* result = strstr(m_filePathSong, ".bank");
		if (result != nullptr)
			m_bankFileContains = true;
		else
			m_bankFileContains = false;
	}

	Audio::Update();
	Conductor::Update();

	m_WorldMousePos = GetScreenToWorld2D(GetMousePosition(), m_MainCamera);

	if (m_FileDialog.windowActive == false && m_inTextbox == false)
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
	if (m_FileDialog.SelectFilePressed && m_FDstate == FileDialogeState::SaveExportedFiles)
		ExportFuckingChartGodHelpUsAll();

	if (m_FileDialog.SelectFilePressed == true && m_FDstate == FileDialogeState::SelectAndLoadJson)
		LoadChart(std::string(m_FileDialog.dirPathText) + "\\" + std::string(m_FileDialog.fileNameText));
	
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

			if (GuiButton({ Constants::WindowWidth - 100.f, 440, 100, 30 }, "Export Chart"))
			{
				m_FileDialog.windowActive = true;
				m_FileDialog.saveFileMode = true;
				m_FDstate = FileDialogeState::SaveExportedFiles;
			}

			if (GuiButton({ Constants::WindowWidth - 100.f, 480, 100, 30 }, "Load Chart"))
			{
				m_FileDialog.windowActive = true;
				m_FileDialog.saveFileMode = false;
				m_FDstate = FileDialogeState::SelectAndLoadJson;
				//LoadChart(Constants::ChartPath + std::string(m_filePathChart));
			}

			GuiCheckBox({ Constants::WindowWidth - 100.f, 300, 20, 20 }, "Hitsond", &m_PlayHitsound);

			//I need to refactor this so fucking much - Pyr0 03.18.25
			GuiDrawText("File Path", { Constants::WindowWidth - 100.f, 55, 100, 30 }, TEXT_ALIGN_CENTER, GRAY);

			if(GuiTextBox({ Constants::WindowWidth - 100.f, 75, 100, 30 }, m_filePathSong, 1024, m_fileBool))
			{
				m_inTextbox = !m_inTextbox;
				m_fileBool = !m_fileBool;
			}

			if (m_bankFileContains == true)
			{
				GuiDrawText("Event Path", { Constants::WindowWidth - 100.f, 100, 100, 30 }, TEXT_ALIGN_CENTER, GRAY);

				if(GuiTextBox({ Constants::WindowWidth - 100.f, 120, 100, 30 }, m_eventName, 128, m_eventBool))
				{
					m_inTextbox = !m_inTextbox;
					m_eventBool = !m_eventBool;
				}
			}

			GuiDrawText("BPM", { Constants::WindowWidth - 100.f, 150, 100, 30 }, TEXT_ALIGN_CENTER, GRAY);

			if (GuiTextBox({ Constants::WindowWidth - 100.f, 170, 100, 30 }, m_bpmText, 10, m_bpmBool))
			{
				m_inTextbox = !m_inTextbox;
				m_bpmBool = !m_bpmBool;
			}

			GuiDrawText("Scroll Speed", { Constants::WindowWidth - 100.f, 210, 100, 30 }, TEXT_ALIGN_CENTER, GRAY);
			if (GuiTextBox({ Constants::WindowWidth - 100.f, 230, 100, 30 }, m_scrollSpeed, 10, m_scrollSpeedBool))
			{
				m_inTextbox = !m_inTextbox;
				m_scrollSpeedBool = !m_scrollSpeedBool;

				Conductor::ScrollSpeed = atof(m_scrollSpeed);
			}

			GuiCheckBox({ Constants::WindowWidth - 100.f, 350, 20, 20 }, "Is 3/4?", &m_isThreeFour);

			if (GuiButton({ Constants::WindowWidth - 100.f, 400, 100, 30 }, "Load Song"))
			{
				Vector2 Signature = m_isThreeFour ? Vector2({ 3, 4 }) : Vector2({ 4, 4 });
				RestartSong(atoi(m_bpmText), Signature, Constants::MusicPath + m_filePathSong, m_eventName);
			}

			GuiUnlock();

		GuiWindowFileDialog(&m_FileDialog);

		DrawFPS(750, 0);

	EndDrawing();
}

void App::Destroy()
{
	for (auto& chart : m_Charts)
	{
		for (auto& note : chart->GetAllNotes())
		{
			delete note;
			note = nullptr;
		}

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
	superCoolData["songPath"] = Conductor::GetSongPath();
	superCoolData["isBank"] = Conductor::IsBank();
	superCoolData["songName"] = Conductor::GetSongName();
	superCoolData["bpm"] = Conductor::BPM;
	superCoolData["scrollSpeed"] = Conductor::ScrollSpeed;
	superCoolData["songLenght"] = Conductor::SongMaxLenght;
	superCoolData["is3/4"] = Conductor::GetTopNum() == 3 ? true : false;
	
	for (auto& chart : m_Charts)
	{
		std::vector<Note*> notes = chart->GetAllNotes();
		for (Note* note : notes)
		{
			Note::NoteData data = note->GetNoteData();

			if (note->GetNoteData().isSustended == true)
			{
				superCoolData["notes"].emplace_back(nlohmann::json::object({
					{"time", data.notePosition},
					{"noteID", data.noteID},
					{"noteTurn", note->owner},
					{"isSusteded", true},
					{"susLen", data.sustendedLen},
					{"susLenCell", data.sustendedCellLen}
				}));
			}
			else
			{
				superCoolData["notes"].emplace_back(nlohmann::json::object({
					{"time", data.notePosition},
					{"noteID", data.noteID},
					{"noteTurn", note->owner},
					{"isSusteded", false },
				}));
			}
		}
	}
	
	char* stringPos = strstr(m_FileDialog.fileNameText, ".json");
	if (stringPos)
	{
		size_t fileNameLen = strlen(m_FileDialog.fileNameText);
		size_t fileExtensionLen = strlen(".json");

		memmove(stringPos, stringPos + fileNameLen, fileNameLen - (stringPos - m_FileDialog.fileNameText) - fileExtensionLen + 1);
	}

	std::string filePath = m_FileDialog.dirPathText + std::string("\\") + m_FileDialog.fileNameText;
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

	ResetFileDialog();
}

void App::RestartSong(int BPM, Vector2 Signature, const std::string& songPath, const std::string& eventName = "")
{
	for (auto& chart : m_Charts)
	{
		for (auto& note : chart->GetAllNotes())
		{
			delete note;
			note = nullptr;
		}

		delete chart;
		chart = nullptr;
	}

	m_Charts.clear();
	m_ChartNames.clear();
	m_RendableNotes.clear();

	m_ChartNames.push_back("oldMan");
	m_ChartNames.push_back("lad");

	Conductor::Init(BPM, Signature.x, Signature.y, songPath, eventName);

	int columFuck = static_cast<int>(std::ceil(Conductor::SongMaxLenght / (Conductor::MSPerBeat / 4.0f)));

	for (size_t i = 0; i < m_ChartNames.size(); i++)
	{
		Vector2 pos = { 100 + (i * (Constants::GridWidth * Constants::MaxRows + 50)), m_ChartStartYPos };
		m_Charts.emplace_back(new ChartRegion(pos, m_ChartNames[i], columFuck));
	}

	Conductor::SetPosition(0);
	Conductor::SetPause(true);
	paused = true;
	ResetTextbox();
}

void App::LoadChart(const std::string& path)
{
	std::fstream chartFile(path);
	if (!chartFile.is_open()) {
		std::cerr << "Cant load File" << std::endl;
		return;
	}

	nlohmann::json superCoolChart;
	try {
		chartFile >> superCoolChart;
	}
	catch (const nlohmann::json::parse_error& e) {
		std::cerr << "Error cant parse this shit: " << e.what() << std::endl;
		return;
	}

	chartFile.close();

	{
		bool isThereFour = superCoolChart["is3/4"];
		int ScrollSpeed = superCoolChart["scrollSpeed"];
		int BPM = superCoolChart["bpm"];
		int SongLen = superCoolChart["songLenght"];
		std::string SongPath = superCoolChart["songPath"];
		bool isSongInBank = superCoolChart["isBank"];
		std::string SongName = isSongInBank ? superCoolChart["songName"] : "";

		Vector2 Signature = isThereFour ? Vector2({ 3, 4 }) : Vector2({ 4, 4 });
		RestartSong(BPM, Signature, SongPath, SongName);
	}

	auto notes = superCoolChart["notes"];

	for (const auto& note : notes)
	{
		Note::NoteData data;

		data.isSustended = note["isSusteded"];
		data.sustendedLen = data.isSustended == true ? note["susLen"] : 0;
		data.sustendedCellLen = data.isSustended == true ? note["susLenCell"] : 0;
		data.noteID = note["noteID"];
		data.notePosition = note["time"];
		std::string noteTurn = note["noteTurn"];

		for (const auto& chart : m_Charts)
		{
			if (chart->GetOwner() == noteTurn)
				chart->AddNote(data, noteTurn);
		}
	}

	ResetFileDialog();
}

void App::ResetTextbox()
{
	m_bpmBool = false;
	m_eventBool = false;
	m_fileBool = false;
	m_scrollSpeedBool = false;
}

void App::ResetFileDialog()
{
	m_FileDialog.SelectFilePressed = false;
	m_FileDialog.saveFileMode = false;
	memset(m_FileDialog.fileNameText, 0, 1024);
	memset(m_FileDialog.fileNameTextCopy, 0, 1024);
}
