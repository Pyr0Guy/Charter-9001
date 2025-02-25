#pragma once

#include "Note.hpp"

class Cell
{
public:
	Cell(int x, int y, int ID, int colum, Color c);

	void Draw();
	void Update(const Vector2& mousePos);

	Cell* ReturnCell();

	int ReturnID() const;
	bool GetActive() const;
	Vector2 GetPosition() const;

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