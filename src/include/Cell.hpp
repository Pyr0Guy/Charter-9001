#pragma once

#include "Note.hpp"

class Cell
{
public:
	Cell(int x, int y, int ID, int colum, Color c);

	void Draw();
	void Update(Vector2 cameraPos);

	Cell* ReturnCell();

	int ReturnID() const;
	bool GetActive() const;
	Vector2 GetPosition() const;

	void Destroy();

	Vector2 GetNextCellPosition(int direction) const;

	Note* m_NoteRef;
	bool m_WithArrow;
	int cellColum;
private:
	int m_x, m_y;

	Color m_Color;
	bool m_Active;

	int m_ID;
};