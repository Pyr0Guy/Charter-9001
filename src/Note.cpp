#include "pch.hpp"
#include "include/Note.hpp"
#include "include/Constants.hpp"
#include "include/ResourceManager.hpp"

Note::Note(NoteData& note, Vector2 pos)
	: m_NoteData(note)
{
	m_NoteSprite = new Sprite(pos, Constants::ImagePath + "arrows_basic", "arrow", {0.45, 0.45}, true, 1.f);
	std::string noteName = Constants::arrowsList[m_NoteData.noteID] + "note";
	ResourceManager::LoadAnimation(Constants::ImagePath + "arrows_basic.xml", noteName);
	m_NoteSprite->Play(noteName);
	
	//m_NoteSprite->LoadAnimation(noteName);
	//m_NoteSprite->Play(noteName);
	m_NoteSprite->SetOrigin(Origin::TOP_LEFT);

	m_PrevSustenedLen = 0;
	m_Active = false;
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
	
	if (m_PrevSustenedLen != m_NoteData.sustendedLen)
	{
		Vector2 basePos = m_NoteSprite->GetPosition();
		UpdateSustainSprites(basePos);
		m_PrevSustenedLen = m_NoteData.sustendedLen;
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
		ResourceManager::LoadAnimation(Constants::ImagePath + "arrows_basic.xml", noteName);
		//sustainSprite->LoadAnimation(noteName);
		sustainSprite->Play(noteName);
		sustainSprite->SetOrigin(Origin::TOP_LEFT);

		Vector2 pos2 = sustainSprite->GetPosition();
		pos2.x = pos.x + (Constants::GridWidth / 2.f) - 10.f;
		sustainSprite->SetPosition(pos2);

        m_SustendedsSprites.push_back(sustainSprite);

		if (i == m_NoteData.sustendedLen - 1)
		{
			pos.y += Constants::GridHeight;
			Sprite* sustainSprite = new Sprite(pos, Constants::ImagePath + "arrows_basic", "arrow", { 0.5, 0.5 }, true, 1.f);
			std::string noteName = Constants::arrowsList[m_NoteData.noteID] + "note_long_end";
			ResourceManager::LoadAnimation(Constants::ImagePath + "arrows_basic.xml", noteName);
			//sustainSprite->LoadAnimation(noteName);
			sustainSprite->Play(noteName);
			sustainSprite->SetOrigin(Origin::TOP_LEFT);

			Vector2 pos2 = sustainSprite->GetPosition();
			pos2.x = pos.x + (Constants::GridWidth / 2.f) - 19.5f;
			sustainSprite->SetPosition(pos2);

			m_SustendedsSprites.push_back(sustainSprite);

			m_NoteData.noteSustendEnd = startPos.y + (i + 1) * Constants::GridHeight - 5.f;
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

bool Note::isMouseOverEndSus(const Vector2& mousePos)
{
	if (m_SustendedsSprites.empty()) return false;

	Sprite* spriteSusEnd = m_SustendedsSprites.back();
	Rectangle rect = spriteSusEnd->GetSpriteRect();

	std::cout << "x: " << rect.x << " y: " << rect.y << " width + x: " << rect.x + rect.width << " height + y: " << rect.y + rect.height << std::endl;
	std::cout << "mouse x: " << mousePos.x << " mouse y: " << mousePos.y << std::endl;

	return mousePos.x >= rect.x && mousePos.x <= rect.x + rect.width &&
		mousePos.y >= rect.y && mousePos.y <= rect.y + rect.height;
}
