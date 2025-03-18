#include "pch.hpp"
#include "include/Note.hpp"
#include "include/Constants.hpp"
#include "include/Conductor.hpp"
#include "include/ResourceManager.hpp"

Note::Note(const NoteData& note, Vector2 pos)
	: m_NoteData(note)
{
	m_NoteSprite = new Sprite(pos, Constants::ImagePath + "arrows_basic", "arrow", {0.45, 0.45}, true, 1.f);
	m_NoteSprite->Play(Constants::arrowsList[m_NoteData.noteID] + "note");
	m_NoteSprite->SetOrigin(Origin::TOP_LEFT);

	m_PrevSustenedLen = 0;
	m_Active = false;
	hitProcessed = false;
	isRendered = false;
	isDeleted = false;

	owner = "";
}

Note::~Note()
{
	for (auto& susSprite : m_SustendedsSprites)
	{
		delete susSprite;
		susSprite = nullptr;
	}
	m_SustendedsSprites.clear();

	delete m_NoteSprite;
	m_NoteSprite = nullptr;
}

void Note::Update()
{
	m_NoteSprite->Update();
	
	if (m_PrevSustenedLen != m_NoteData.sustendedCellLen)
	{
		Vector2 basePos = m_NoteSprite->GetPosition();
		UpdateSustainSprites(basePos);
		m_PrevSustenedLen = m_NoteData.sustendedCellLen;
	}

	for (auto& sprite : m_SustendedsSprites) {
		sprite->Update();
	}
}

void Note::Draw()
{
	for (auto& sustendedNote : m_SustendedsSprites)
		sustendedNote->Draw();

	m_NoteSprite->Draw();
}

void Note::SetSustainLength(int length)
{
	//length = std::clamp(length, 0, 10);
	m_NoteData.sustendedCellLen = length;
}


void Note::UpdateSustainSprites(const Vector2& startPos) {

    for (auto& sprite : m_SustendedsSprites) {
        delete sprite;
    }
    m_SustendedsSprites.clear();


    for (int i = 0; i < m_NoteData.sustendedCellLen; i++) {
		Vector2 pos = {
			startPos.x + (Constants::GridWidth / 2.f) - 10.f,
			startPos.y + (i + 1) * Constants::GridHeight - 6.f
        };

		m_SustendedsSprites.emplace_back(new Sprite(pos, Constants::ImagePath + "arrows_basic", "arrow", { 0.5, 2.10 }, true, 1.f));
		m_SustendedsSprites.back()->Play(Constants::arrowsList[m_NoteData.noteID] + "note_long");
		m_SustendedsSprites.back()->SetOrigin(Origin::TOP_LEFT);

		if (i == m_NoteData.sustendedCellLen - 1)
		{
			pos.y += Constants::GridHeight;
			pos.x -= 9.5f;
			m_SustendedsSprites.emplace_back(new Sprite(pos, Constants::ImagePath + "arrows_basic", "arrow", { 0.5, 0.5 }, true, 1.f));
			m_SustendedsSprites.back()->Play(Constants::arrowsList[m_NoteData.noteID] + "note_long_end");
			m_SustendedsSprites.back()->SetOrigin(Origin::TOP_LEFT);

			m_NoteData.sustendedLen = m_NoteData.sustendedCellLen * Conductor::MSPerCell;
		}
    }
}

void Note::SetSutended(bool sus)
{
	m_NoteData.isSustended = sus;
}

Note::NoteData Note::GetNoteData()
{
	return m_NoteData;
}

Vector2 Note::GetPosition()
{
	return m_NoteSprite->GetPosition();
}

bool Note::isMouseOverEndSus(const Vector2& mousePos)
{
	if (m_SustendedsSprites.empty()) return false;

	Sprite* spriteSusEnd = m_SustendedsSprites.back();
	Rectangle rect = spriteSusEnd->GetSpriteRect();

	return mousePos.x >= rect.x && mousePos.x <= rect.x + rect.width &&
		mousePos.y >= rect.y && mousePos.y <= rect.y + rect.height;
}
