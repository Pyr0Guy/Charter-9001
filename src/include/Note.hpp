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
	std::vector<Sprite*> m_SustendedsSprites;
private:
	NoteData m_NoteData;
	Sprite* m_NoteSprite;
};