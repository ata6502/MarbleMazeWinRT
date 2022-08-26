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

#define MAX_HIGH_SCORES 5
typedef std::vector<HighScoreEntry> HighScoreEntries;

class HighScoreTable : public TextElement
{
public:
    HighScoreTable();

    virtual void Initialize();
    virtual void Update(float timeTotal, float timeDelta);
    virtual void Render();

    void AddScoreToTable(HighScoreEntry& entry);
    HighScoreEntries GetEntries() { return m_entries; };
    void Reset();

protected:
    HighScoreEntries    m_entries;

    void UpdateText();
};

