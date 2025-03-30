#pragma once

#include "Sprite.hpp"

class Note
{
public:
	struct NoteData
	{
		float notePosition;
		bool isSustended;
		int sustendedCellLen;
		int sustendedLen;
		int noteID;
	};

public:
	Note(const NoteData& note, Vector2 pos);
	~Note();

	void Update();
	void Draw();

	void SetSustainLength(int length);
	void UpdateSustainSprites(const Vector2& startPos);

	void SetSutended(bool sus);

	NoteData GetNoteData();
	Vector2 GetPosition();

	bool isMouseOverEndSus(const Vector2& startPos);

	std::vector<Sprite*> m_SustendedsSprites;
	std::string owner;

	bool hitProcessed;
	bool isRendered;
	bool isDeleted;
private:
	NoteData m_NoteData;
	Sprite* m_NoteSprite;

	int m_PrevSustenedLen;

	bool m_Active;
};