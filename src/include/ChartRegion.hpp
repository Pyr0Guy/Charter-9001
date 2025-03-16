#pragma once

#include "Cell.hpp"

class ChartRegion
{
private:
	void NoteHandling(const Vector2& mousePos);

	typedef struct Line
	{
		Rectangle points;
		Color c;
	}Line;

public:
	ChartRegion(const Vector2& pos, const std::string& regionOwner, unsigned int howManyColums);
	~ChartRegion();

	void Update(const Vector2& mousePos);
	void Draw();

	std::vector<Note*>& GetAllNotes();
	std::string GetOwner() const;

	
private:
	Vector2 m_pos;

	std::string m_whichRegion;
	std::vector<Cell*> m_allCell;
	std::vector<Note*> m_AllNotes;
	std::vector<Line> m_LinesVector;

	Vector2 m_DragStartPos;
	bool m_isDraging;
	Note* m_DragableNote;
	bool m_DragEndNote;

	Cell* m_CurCell;

	int m_ColumNum;

	bool m_Active;
};