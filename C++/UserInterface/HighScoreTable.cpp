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

void HighScoreTable::AddScoreToTable(HighScoreEntry& entry)
{
    for (auto iter = m_entries.begin(); iter != m_entries.end(); ++iter)
    {
        iter->wasJustAdded = false;
    }

    entry.wasJustAdded = false;

    for (auto iter = m_entries.begin(); iter != m_entries.end(); ++iter)
    {
        if (entry.elapsedTime < iter->elapsedTime)
        {
            entry.wasJustAdded = true;
            m_entries.insert(iter, entry);
            while (m_entries.size() > MAX_HIGH_SCORES)
                m_entries.pop_back();

            UpdateText();
            return;
        }
    }

    if (m_entries.size() < MAX_HIGH_SCORES)
    {
        entry.wasJustAdded = true;
        m_entries.push_back(entry);
        UpdateText();
    }
}

void HighScoreTable::UpdateText()
{
    WCHAR formattedTime[32];
    WCHAR lines[1024] = { 0, };
    WCHAR buffer[128];

    swprintf_s(lines, L"High Scores:");
    for (unsigned int i = 0; i < MAX_HIGH_SCORES; ++i)
    {
        if (i < m_entries.size())
        {
            StopwatchTimer::GetFormattedTime(formattedTime, m_entries[i].elapsedTime);
            swprintf_s(
                buffer,
                (m_entries[i].wasJustAdded ? L"\n>> %s\t%s <<" : L"\n%s\t%s"),
                m_entries[i].tag.c_str(),
                formattedTime
            );
            wcscat_s(lines, buffer);
        }
        else
        {
            wcscat_s(lines, L"\n-");
        }
    }

    SetText(lines);
}
