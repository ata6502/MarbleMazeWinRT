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

class CountdownTimer : public TextElement
{
public:
    CountdownTimer();

    virtual void Initialize();
    virtual void Update(float timeTotal, float timeDelta);
    virtual void Render();

    void StartCountdown(int seconds);

    bool IsCountdownComplete() const { return m_isCountdownComplete; }

protected:
    float   m_elapsedTime;
    int     m_secondsRemaining;
    bool    m_isCountdownComplete;
};

