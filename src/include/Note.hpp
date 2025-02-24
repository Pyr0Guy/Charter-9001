#pragma once

#include "Sprite.hpp"

class Note
{
public:
	struct NoteData
	{
		unsigned int notePosition;
		bool isSustended;
		int sustendedLen;
		int noteSustendEnd;
		int noteID;
	};

public:
	Note(NoteData& note, Vector2 pos);
	~Note();

	void Update();
	void Draw();

	void SetSustainLength(int length);
	void UpdateSustainSprites(const Vector2& startPos);

	void SetSutended(bool sus);

	NoteData GetNoteData();

	bool isMouseOverEndSus(const Vector2& startPos);

	std::vector<Sprite*> m_SustendedsSprites;
private:
	NoteData m_NoteData;
	Sprite* m_NoteSprite;

	int m_PrevSustenedLen;

	bool m_Active;
};