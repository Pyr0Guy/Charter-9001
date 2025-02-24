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
	
	Vector2 basePos = m_NoteSprite->GetPosition();
	UpdateSustainSprites(basePos);

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
	m_NoteData.sustendedLen = length;
}


void Note::UpdateSustainSprites(const Vector2& startPos) {

    for (auto& sprite : m_SustendedsSprites) {
        delete sprite;
    }
    m_SustendedsSprites.clear();


    for (int i = 0; i < m_NoteData.sustendedLen; i++) {
		Vector2 pos = {
			startPos.x,
			startPos.y + (i + 1) * Constants::GridHeight - 5.f
        };

        Sprite* sustainSprite = new Sprite(pos,Constants::ImagePath + "arrows_basic","arrow",{ 0.5, 2.00 },true, 1.f);
		std::string noteName = Constants::arrowsList[m_NoteData.noteID] + "note_long";
		sustainSprite->LoadAnimation(noteName);
		sustainSprite->Play(noteName);
		sustainSprite->SetOrigin(sustainSprite->TOP_LEFT);

		Vector2 pos2 = sustainSprite->GetPosition();
		pos2.x = pos.x + (Constants::GridWidth / 2.f) - 10.f;
		sustainSprite->SetPosition(pos2);

        m_SustendedsSprites.push_back(sustainSprite);

		if (i == m_NoteData.sustendedLen - 1)
		{
			pos.y += Constants::GridHeight;
			Sprite* sustainSprite = new Sprite(pos, Constants::ImagePath + "arrows_basic", "arrow", { 0.5, 0.5 }, true, 1.f);
			std::string noteName = Constants::arrowsList[m_NoteData.noteID] + "note_long_end";
			sustainSprite->LoadAnimation(noteName);
			sustainSprite->Play(noteName);
			sustainSprite->SetOrigin(sustainSprite->TOP_LEFT);

			Vector2 pos2 = sustainSprite->GetPosition();
			pos2.x = pos.x + (Constants::GridWidth / 2.f) - 20.f;
			sustainSprite->SetPosition(pos2);

			m_SustendedsSprites.push_back(sustainSprite);
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
