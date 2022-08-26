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

class StopwatchTimer : public TextElement
{
public:
    StopwatchTimer();

    virtual void Initialize();
    virtual void Update(float timeTotal, float timeDelta);
    virtual void Render();

    void Start();
    void Stop();
    void Reset();

    float GetElapsedTime() const { return m_elapsedTime; };
    void SetElapsedTime(float time) { m_elapsedTime = time; };

    void GetFormattedTime(WCHAR* buffer, int length) const;
    template <size_t _Len>
    void GetFormattedTime(WCHAR(&buffer)[_Len]) const { GetFormattedTime(buffer, _Len); }

    static void GetFormattedTime(WCHAR* buffer, int length, float time);
    template <size_t _Len>
    static void GetFormattedTime(WCHAR(&buffer)[_Len], float time) { GetFormattedTime(buffer, _Len, time); }

protected:
    bool    m_active;
    float   m_elapsedTime;
};
