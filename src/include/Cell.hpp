#pragma once

#include "Note.hpp"

class Cell
{
public:
	Cell(int x, int y, int ID, int colum, float CellMS, Color c);

	void Draw();
	void Update(const Vector2& mousePos);

	Cell* ReturnCell();

	int ReturnID() const;
	bool GetActive() const;
	Vector2 GetPosition() const;
	float GetCellMS() const;
	int GetID() const;

	Vector2 GetNextCellPosition(int direction) const;

	Note* m_NoteRef;
	bool m_WithArrow;
	int cellColum;
private:
	int m_x, m_y;

	Color m_Color;
	bool m_Active;

	int m_ID;
	float m_CellMS;
};