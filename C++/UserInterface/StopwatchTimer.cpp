// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#include "pch.h"
#include "StopwatchTimer.h"

StopwatchTimer::StopwatchTimer() :
    m_active(false),
    m_elapsedTime(0.0f)
{
}

void StopwatchTimer::Initialize()
{
    TextElement::Initialize();
}

void StopwatchTimer::Update(float timeTotal, float timeDelta)
{
    if (m_active)
    {
        m_elapsedTime += timeDelta;

        WCHAR buffer[16];
        GetFormattedTime(buffer);
        SetText(buffer);
    }

    TextElement::Update(timeTotal, timeDelta);
}

void StopwatchTimer::Render()
{
    TextElement::Render();
}

void StopwatchTimer::Start()
{
    m_active = true;
}

void StopwatchTimer::Stop()
{
    m_active = false;
}

void StopwatchTimer::Reset()
{
    m_elapsedTime = 0.0f;
}

void StopwatchTimer::GetFormattedTime(WCHAR* buffer, int length) const
{
    GetFormattedTime(buffer, length, m_elapsedTime);
}

void StopwatchTimer::GetFormattedTime(WCHAR* buffer, int length, float time)
{
    int partial = (int)floor(fmodf(time * 10.0f, 10.0f));
    int seconds = (int)floor(fmodf(time, 60.0f));
    int minutes = (int)floor(time / 60.0f);
    swprintf_s(buffer, length, L"%02d:%02d.%01d", minutes, seconds, partial);
}
