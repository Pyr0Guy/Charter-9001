#pragma once

#include "types.hpp"

class Sprite
{
private:
	typedef std::unordered_map<std::string, std::vector<Rectangle>> AnimationList;

	void LoadAllAnimations(std::string XMLPath);
	void ChangeOrigin(int origin);

public:
	typedef enum ORIGIN
	{
		TOP_LEFT,
		TOP_CENTER,
		TOP_RIGHT,
		CENTER,
		BOTTOM_LEFT,
		BOTTOM_CENTER,
		BOTTOM_RIGHT
	} ORIGIN;

public:
	Sprite(Vector2 pos, const std::string& filepath, const std::string& textureName, Vector2 scale, bool isAnimation, f32 animSpeed, bool loadAll = false);
	~Sprite();

	void Update();
	void Draw();

	void Play(const std::string& anim);
	void PlayOneShot(const std::string& anim);

	void Pause();
	void Resume();

	std::string GetCurrentAnimation() const;
	std::vector<std::string> GetAllAnimations() const;
	Vector2 GetSpriteSize() const;
	Vector2 GetSpriteScale() const;
	s16 GetSpriteMaxFrames() const;
	int GetSpriteCurrentFrame() const;
	Vector2 GetPosition() const;

	void SetAnimationSpeed(f32 speed);
	void SetOrigin(int origin);
	void SetPosition(Vector2 pos);

	void Move(Vector2 move);

	void LoadAnimation(const std::string& anim);

	bool IsAnimationExsists(const std::string& name) const;
private:

	s16 m_MaxFrames;
	f32 m_CurFrame;
	f32 m_AnimSpeed;

	std::string m_CurAnim;
	bool m_isOneShot;
	bool m_isOnPause;
	bool m_isAnimation;

	AnimationList m_AnimList;
	Rectangle m_CurFrameRect;

	Vector2 m_Pos;
	Vector2 m_Scale;
	f32 m_Rotation;

	Vector2 m_Origin;
	s8 m_CurOrigin;

	Texture2D m_Texture;

	std::string m_ThatFuckingStringThatIHate;
};