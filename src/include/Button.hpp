#pragma once

class Button
{
public:
	Button(const Vector2& pos, const Vector2& size, const std::string& title = "");
	Button(const Rectangle& rect, const std::string& title = "");
	virtual ~Button();

	bool isClicked(const Vector2& mousePos);
	bool isHover(const Vector2& mousePos);

	virtual void Update(const Vector2& mousePos);
	virtual void Draw();
private:
	Rectangle m_ButtonBox;
	Color m_Color;

	std::string m_Title;
	Color m_TitleColor;

	Vector2 m_TextPos;
};