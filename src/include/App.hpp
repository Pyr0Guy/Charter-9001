#pragma once
#include "Cell.hpp"

class App
{
public:
	App(unsigned int Width, unsigned int Height, const std::string& title);

	void Update();
	void Draw();

	void Destroy();

	bool IsClosed();

private:
	Camera2D m_MainCamera;
	float m_LinePosition;

	std::vector<Cell*> m_CeilVector;
};