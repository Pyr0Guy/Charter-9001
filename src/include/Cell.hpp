#pragma once

#include "Sprite.hpp"

class Cell
{
private:
	void SetArrow();
public:
	Cell(int x, int y, int ID ,Color c);

	void Draw();
	void Update(Vector2 cameraPos);

	void Destroy();
private:
	int m_x, m_y;

	Color m_Color;
	bool m_Active;

	int m_ID;

	bool m_WithArrow;

	Sprite* m_Sprite;
};