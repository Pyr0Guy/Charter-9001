#include "pch.hpp"
#include "include/ChartRegion.hpp"
#include "include/Constants.hpp"
#include "include/Conductor.hpp"

ChartRegion::ChartRegion(const Vector2& pos, const std::string& regionOwner, unsigned int howManyColums)
	: m_whichRegion(regionOwner), m_ColumNum(howManyColums), m_pos(pos)
{
	m_allCell.reserve(howManyColums);

	for (int j = 0; j < Constants::MaxRows; j++)
	{
		for (size_t i = 0; i < howManyColums; i++)
		{
			Color c = (i + j) % 2 == 0 ? BLACK : GRAY;

			m_allCell.emplace_back(new Cell(m_pos.x + (j * Constants::GridWidth), m_pos.y + (i * Constants::GridHeight), j, i, c));
		}
	}

	m_CurCell = nullptr;
	m_DragableNote = nullptr;

	m_Active = false;
	m_DragEndNote = false;
	m_isDraging = false;
}

ChartRegion::~ChartRegion()
{
	for (auto& cell : m_allCell)
	{
		delete cell;
		cell = nullptr;
	}

	m_allCell.clear();

	for (auto& note : m_AllNotes)
	{
		delete note;
		note = nullptr;
	}

	m_AllNotes.clear();
}

void ChartRegion::Update(const Vector2& mousePos)
{
	m_CurCell = nullptr;

	for (auto& cell : m_allCell)
	{
		cell->Update(mousePos);

		if (cell->GetActive() == true)
			m_CurCell = cell->ReturnCell();
	}

	NoteHandling(mousePos);

	if (m_isDraging == true && GetMousePosition().y >= Constants::WindowHeight - 100)
	{
		Conductor::SetPosition(Conductor::GetPosition() + 20);
	}
	
	if (m_isDraging == true && GetMousePosition().y <= 100 && m_isDraging == true)
	{
		Conductor::SetPosition(Conductor::GetPosition() - 20);
	}

}

void ChartRegion::Draw()
{
	for (auto& cell : m_allCell)
		cell->Draw();

	for (auto& note : m_AllNotes)
		note->Draw();
}

void ChartRegion::NoteHandling(const Vector2& mousePos)
{
	float deltaY = mousePos.y - m_DragStartPos.y;

	if (m_CurCell != nullptr)
	{
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			for (auto& note : m_AllNotes)
			{
				if (note->isMouseOverEndSus(mousePos))
				{
					m_DragableNote = note;
					m_DragEndNote = true;
					m_isDraging = true;
					m_DragStartPos = mousePos;
					break;
				}
			}

			if (m_DragableNote == nullptr && m_CurCell->m_WithArrow == true)
			{
				m_DragableNote = m_CurCell->m_NoteRef;
				m_DragEndNote = false;
				m_isDraging = true;
				m_DragStartPos = mousePos;
			}

		}

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && m_isDraging == true)
		{
			Vector2 currentMousePos = mousePos;
			float deltaY;

			if (m_DragEndNote == false)
				deltaY = currentMousePos.y - m_DragStartPos.y;
			else
				deltaY = currentMousePos.y - (Constants::GridHeight * 2); //This is buggy idk how to fix this

			int cells = static_cast<int>(deltaY / Constants::GridHeight);

			cells = std::max(0, cells);

			if (cells > 0)
				m_DragableNote->SetSutended(true);
			else
				m_DragableNote->SetSutended(false);

			if (m_DragableNote != nullptr) {
				m_DragableNote->SetSustainLength(cells);
			}
		}

		if (m_isDraging && IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
		{
			m_isDraging = false;
			m_DragableNote = nullptr;
			m_DragEndNote = false;
		}

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			if (m_CurCell->m_WithArrow == false)
			{
				Note::NoteData lol = {};
				lol.noteID = m_CurCell->ReturnID();
				lol.isSustended = false;
				lol.notePosition = m_CurCell->cellColum * (Conductor::MSPerBeat / 4);
				lol.sustendedLen = 0;

				m_AllNotes.emplace_back(new Note(lol, m_CurCell->GetPosition()));
				m_AllNotes.back()->Update();

				m_CurCell->m_NoteRef = m_AllNotes.back();
				m_CurCell->m_WithArrow = true;
			}
			else
			{
				auto it = std::find(m_AllNotes.begin(), m_AllNotes.end(), m_CurCell->m_NoteRef);

				m_CurCell->m_NoteRef = nullptr;
				m_CurCell->m_WithArrow = false;

				if (it != m_AllNotes.end())
				{
					delete* it;
					m_AllNotes.erase(it);
				}
			}
		}
	}

	for (auto& note : m_AllNotes)
		note->Update();
}