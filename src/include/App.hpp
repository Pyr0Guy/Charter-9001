#pragma once
#include "ChartRegion.hpp"

class App
{
private:
	typedef enum FileDialogeState
	{
		SaveExportedFiles,
		SelectAndLoadJson
	}FileDialogeState;

private:
	void LoadAnimations();
	void ConductorControll();
	void ExportFuckingChartGodHelpUsAll();
	bool CheckIfFileExsist();

	void RestartSong(int BPM, Vector2 Signature, const std::string& songPath, const std::string& eventName);
	void LoadChart(const std::string& path);

	void ResetTextbox();
	void ResetFileDialog();
public:
	App(unsigned int Width, unsigned int Height, const std::string& title);

	void Update();
	void Draw();

	void Destroy();

	bool IsClosed();

private:
	Camera2D m_MainCamera;
	float m_LinePosition;

	Vector2 m_WorldMousePos;

	//Vectors for future
	float m_ChartStartYPos;
	std::vector<ChartRegion*> m_Charts;
	std::vector<std::string> m_ChartNames;
	std::vector<Note*> m_RendableNotes;

	FileDialogeState m_FDstate;

	Sprite* bg;

	//GUI
	char m_eventName[128];
	bool m_eventBool;

	char m_filePathSong[1024];
	bool m_fileBool;

	bool m_bankFileContains;

	char m_bpmText[10];
	bool m_bpmBool;

	char m_scrollSpeed[4];
	bool m_scrollSpeedBool;

	bool m_isThreeFour;
	bool m_inTextbox;

	bool m_ShowFileAllreadyExsistsWindow;

	bool m_PlayHitsound;
};