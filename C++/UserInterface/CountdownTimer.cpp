// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#include "pch.h"
#include "CountdownTimer.h"

CountdownTimer::CountdownTimer() :
    m_elapsedTime(0.0f),
    m_secondsRemaining(0),
    m_isCountdownComplete(false)
{
}

void CountdownTimer::Initialize()
{
    TextElement::Initialize();
}

void CountdownTimer::Update(float timeTotal, float timeDelta)
{
    if (m_secondsRemaining >= 0)
    {
        m_elapsedTime += timeDelta;
        if (m_elapsedTime >= 1.0f)
        {
            m_elapsedTime -= 1.0f;

            if (--m_secondsRemaining > 0)
            {
                WCHAR buffer[4];
                swprintf_s(buffer, L"%2d", m_secondsRemaining);
                SetText(buffer);
                SetTextOpacity(1.0f);
                FadeOut(1.0f);
            }
            else if (m_secondsRemaining == 0)
            {
                SetText(L"Go!");
                SetTextOpacity(1.0f);
                FadeOut(1.0f);
            }
            else
            {
                m_isCountdownComplete = true;
            }

            SetVisible(true);
        }
    }

    TextElement::Update(timeTotal, timeDelta);
}

void CountdownTimer::Render()
{
    TextElement::Render();
}

void CountdownTimer::StartCountdown(int seconds)
{
    m_secondsRemaining = seconds;
    m_elapsedTime = 0.0f;
    m_isCountdownComplete = false;

    WCHAR buffer[4];
    swprintf_s(buffer, L"%2d", m_secondsRemaining);
    SetText(buffer);
    SetTextOpacity(1.0f);
    FadeOut(1.0f);
}
