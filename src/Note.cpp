#include "pch.hpp"
#include "include/Note.hpp"
#include "include/Constants.hpp"

Note::Note(NoteData& note, Vector2 pos)
	: m_NoteData(note)
{
	m_NoteSprite = new Sprite(pos, Constants::ImagePath + "arrows_basic", "arrow", {0.45, 0.45}, true, 1.f);
	std::string noteName = Constants::arrowsList[m_NoteData.noteID] + "note";
	m_NoteSprite->LoadAnimation(noteName);
	m_NoteSprite->Play(noteName);
	m_NoteSprite->SetOrigin(m_NoteSprite->TOP_LEFT);
}

Note::~Note()
{
	delete m_NoteSprite;
}

void Note::Update()
{
	m_NoteSprite->Update();
}

void Note::Draw()
{
	m_NoteSprite->Draw();
}

Note::NoteData Note::GetNoteData()
{
	return m_NoteData;
}
