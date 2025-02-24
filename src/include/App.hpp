#pragma once
#include "Cell.hpp"
#include "Note.hpp"

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

	Cell* m_CurCell;

	std::list<Note*> m_NoteList;
	std::vector<Cell*> m_CeilVector;

	Vector2 m_DragStartPos;
	bool m_isDraging;
	Note* m_DragableNote;
};