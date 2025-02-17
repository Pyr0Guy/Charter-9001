#include "pch.hpp"
#include "include/Cell.hpp"
#include "include/Constants.hpp"

Cell::Cell(int x, int y, int ID, int colum, Color c)
	: m_x(x), m_y(y), m_Color(c), m_ID(ID), cellColum(colum)
{
	m_x *= Constants::GridWidth;
	m_y *= Constants::GridHeight;

	m_x += 100;
	m_y += 100;

	m_Active = false;
	m_WithArrow = false;
	m_NoteRef = nullptr;
}

void Cell::Draw()
{
	DrawRectangle(m_x, m_y, Constants::GridWidth, Constants::GridHeight, m_Color);
}

void Cell::Update(Vector2 cameraPos)
{
	Vector2 mousePos = GetMousePosition();

	if (mousePos.x + cameraPos.x >= m_x && mousePos.x + cameraPos.x <= m_x + Constants::GridWidth &&
		mousePos.y + cameraPos.y >= m_y && mousePos.y + cameraPos.y <= m_y + Constants::GridHeight)
	{
		m_Active = true;
	}
	else
	{
		m_Active = false;
	}

	if (m_Active == true)
	{
		m_Color.a = 150;
	}
	else
	{
		m_Color.a = 255;
	}

}

Cell* Cell::ReturnCell()
{
	return this;
}

int Cell::ReturnID() const
{
	return m_ID;
}

bool Cell::GetActive() const
{
	return m_Active;
}

Vector2 Cell::GetPosition() const
{
	Vector2 pos = { m_x, m_y };
	return pos;
}

void Cell::Destroy()
{
	delete m_NoteRef;
}
