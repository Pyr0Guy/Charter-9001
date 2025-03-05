#pragma once
#include "ChartRegion.hpp"
#include "Button.hpp"

class App
{
private:
	void LoadAnimations();
	
	void ConductorControll();
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

	Sprite* bg;

	//Buttons
};