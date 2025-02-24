#pragma once

#include "types.hpp"

enum class Origin
{
	TOP_LEFT,
	TOP_CENTER,
	TOP_RIGHT,
	CENTER,
	BOTTOM_LEFT,
	BOTTOM_CENTER,
	BOTTOM_RIGHT
};

class Sprite
{
private:
	void ChangeOrigin(Origin origin);

public:
	Sprite(Vector2 pos, const std::string& filepath, const std::string& textureName, Vector2 scale, bool isAnimation, f32 animSpeed);

	void Update();
	void Draw();

	void Play(const std::string& anim);
	void PlayOneShot(const std::string& anim);

	void Pause();
	void Resume();

	std::string GetCurrentAnimation() const;
	Vector2 GetSpriteSize() const;
	Vector2 GetSpriteScale() const;
	Rectangle GetSpriteRect() const;
	s16 GetSpriteMaxFrames() const;
	int GetSpriteCurrentFrame() const;
	Vector2 GetPosition() const;
	Color GetColor() const;

	void SetColor(const Color& c);
	void SetAnimationSpeed(f32 speed);
	void SetOrigin(Origin origin);
	void SetPosition(const Vector2& pos);
	void SetScale(const Vector2& scale);
	void SetRotation(f32 rotation);

	void Move(const Vector2& move);
	void Rotate(f32 rotation);
private:

	s16 m_MaxFrames;
	f32 m_CurFrame;
	f32 m_AnimSpeed;

	std::string m_CurAnim;
	bool m_isOneShot;
	bool m_isOnPause;
	bool m_isAnimation;

	Rectangle m_CurFrameRect;

	Vector2 m_Pos;
	Vector2 m_Scale;
	f32 m_Rotation;
	Color m_Color;

	Vector2 m_Origin;
	Origin m_CurOrigin;

	Texture2D m_Texture;

	std::string m_XmlPath;
	const std::vector<Rectangle>* m_CurrentAnimation = nullptr;
};