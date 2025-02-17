#include "pch.hpp"
#include "include/Cell.hpp"
#include "include/Constants.hpp"

void Cell::SetArrow()
{
	Vector2 pos = { m_x, m_y };
	m_Sprite = new Sprite(pos, Constants::ImagePath + "arrows_basic", "arrow", { 0.45f, 0.45f }, true, 1.0f, false);
	m_Sprite->SetOrigin(m_Sprite->TOP_LEFT);

	std::string lol = Constants::arrowsList[m_ID] + "note";
	m_Sprite->LoadAnimation(lol);
	m_Sprite->Play(lol);
}

Cell::Cell(int x, int y, int ID,Color c)
	: m_x(x), m_y(y), m_Color(c), m_ID(ID)
{
	m_x *= Constants::GridWidth;
	m_y *= Constants::GridHeight;

	m_x += 100;
	m_y += 100;

	m_Active = false;
	m_WithArrow = false;

	m_Sprite = nullptr;
}

void Cell::Draw()
{
	DrawRectangle(m_x, m_y, Constants::GridWidth, Constants::GridHeight, m_Color);

	if (m_WithArrow == true)
	{
		m_Sprite->Draw();
	}
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

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			m_WithArrow = !m_WithArrow;

			if (m_WithArrow == true)
				SetArrow();
			else
				m_Sprite = nullptr;
		}
	}
	else
	{
		m_Color.a = 255;
	}

	if (m_WithArrow == true)
		m_Sprite->Update();
}

void Cell::Destroy()
{
	delete m_Sprite;
}
