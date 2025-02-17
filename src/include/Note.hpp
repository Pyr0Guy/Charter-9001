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

	NoteData GetNoteData();
private:
	NoteData m_NoteData;
	Sprite* m_NoteSprite;
};