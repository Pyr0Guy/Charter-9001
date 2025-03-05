#include "pch.hpp"
#include "include/Button.hpp"
#include "include/Constants.hpp"

Button::Button(const Vector2& pos, const Vector2& size, const std::string& title)
	: m_Color(GRAY), m_Title(title), m_TitleColor(WHITE)
{
	m_ButtonBox.x = pos.x;
	m_ButtonBox.y = pos.y;
	m_ButtonBox.width = pos.x + size.x;
	m_ButtonBox.height = pos.y + size.y;

	m_TextPos.x = (m_ButtonBox.width / 2.f);
	m_TextPos.y = (m_ButtonBox.height / 2.f);

	m_TextPos.x = m_ButtonBox.x + (m_ButtonBox.width / 2.f) - (MeasureText(title.c_str(), Constants::FontSize) / 2.f);
	m_TextPos.y = m_ButtonBox.y + (m_ButtonBox.height / 2.f) - (Constants::FontSize / 2.f);
}

Button::Button(const Rectangle& rect, const std::string& title)
	: m_ButtonBox(rect), m_Color(GRAY), m_Title(title), m_TitleColor(WHITE)
{
	m_TextPos.x = m_ButtonBox.x + (m_ButtonBox.width / 2.f) - (MeasureText(title.c_str(), Constants::FontSize) / 2.f);
	m_TextPos.y = m_ButtonBox.y + (m_ButtonBox.height / 2.f) - (Constants::FontSize / 2.f);
}

Button::~Button()
{
}

bool Button::isClicked(const Vector2& mousePos)
{
	return isHover(mousePos) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

bool Button::isHover(const Vector2& mousePos)
{
	return mousePos.x >= m_ButtonBox.x && mousePos.x <= m_ButtonBox.x + m_ButtonBox.width &&
		mousePos.y >= m_ButtonBox.y && mousePos.y <= m_ButtonBox.y + m_ButtonBox.height;
}

void Button::Update(const Vector2& mousePos)
{
	m_Color = GRAY;
	m_TitleColor.a = 255;

	if (isHover(mousePos))
	{
		m_Color.a = 150;
		m_TitleColor.a = 150;
	}

	if (isClicked(mousePos))
	{
		m_Color = BLACK;
	}
}

void Button::Draw()
{
	DrawRectangleRec(m_ButtonBox, m_Color);
	DrawText(m_Title.c_str(), m_TextPos.x, m_TextPos.y, Constants::FontSize, m_TitleColor);
}
