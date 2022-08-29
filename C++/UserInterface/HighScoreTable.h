// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#pragma once

#include "TextElement.h"

struct HighScoreEntry
{
    std::wstring tag;
    float elapsedTime;
    bool wasJustAdded;

    HighScoreEntry() :
        tag(L""),
        elapsedTime(0.f),
        wasJustAdded(false) {}
};

typedef std::vector<HighScoreEntry> HighScoreEntries;

class HighScoreTable : public TextElement
{
public:
    HighScoreTable();

    virtual void Initialize();
    virtual void Update(float timeTotal, float timeDelta);
    virtual void Render();

    void AddEntry(HighScoreEntry& entry);
    void AddNewEntry(HighScoreEntry& newEntry);
    HighScoreEntries GetEntries() { return m_entries; };
    void Reset();

private:
    const uint8_t MAX_HIGH_SCORES = 5;
    HighScoreEntries    m_entries;

    void UpdateText();
};

