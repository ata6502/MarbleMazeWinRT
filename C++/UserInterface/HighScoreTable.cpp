// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#include "pch.h"

#include "HighScoreTable.h"
#include "StopwatchTimer.h"

#include <sstream> // ostringstream

HighScoreTable::HighScoreTable()
{
}

void HighScoreTable::Initialize()
{
    TextElement::Initialize();
    UpdateText();
}

void HighScoreTable::Reset()
{
    m_entries.clear();
    UpdateText();
}

void HighScoreTable::Update(float timeTotal, float timeDelta)
{
    TextElement::Update(timeTotal, timeDelta);
}

void HighScoreTable::Render()
{
    TextElement::Render();
}

void HighScoreTable::AddEntry(HighScoreEntry& entry)
{
    entry.wasJustAdded = false;
    m_entries.push_back(entry);
    UpdateText();
}

void HighScoreTable::AddNewEntry(HighScoreEntry& newEntry)
{
    for (auto it = m_entries.begin(); it != m_entries.end(); ++it)
        it->wasJustAdded = false;

    newEntry.wasJustAdded = true;

    for (auto it = m_entries.begin(); it != m_entries.end(); ++it)
    {
        if (newEntry.elapsedTime < it->elapsedTime)
        {
            m_entries.insert(it, newEntry);
            if (m_entries.size() > MAX_HIGH_SCORES)
                m_entries.pop_back();
            UpdateText();
            return;
        }
    }

    if (m_entries.size() < MAX_HIGH_SCORES)
    {
        m_entries.push_back(newEntry);
        UpdateText();
    }
}

void HighScoreTable::UpdateText()
{
    std::wostringstream buffer;
    buffer << L"High Scores:\n";

    WCHAR formattedTime[32];
    for (uint8_t i = 0; i < MAX_HIGH_SCORES; ++i)
    {
        if (i < m_entries.size())
        {
            bool isNew = m_entries[i].wasJustAdded;

            StopwatchTimer::GetFormattedTime(formattedTime, m_entries[i].elapsedTime);
            buffer << 
                (isNew ? ">> " : "") <<
                m_entries[i].tag << 
                "\t" << 
                formattedTime << 
                (isNew ? "<< " : "") <<
                std::endl;
        }
        else
        {
            buffer << L"-\n";
        }
    }

    SetText(buffer.str());
}
