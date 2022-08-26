// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#pragma once

#include "ElementBase.h"
#include "TextStyle.h"

class TextElement : public ElementBase
{
public:
    TextElement();

    virtual void Initialize();
    virtual void Update(float timeTotal, float timeDelta);
    virtual void Render();
    virtual void ReleaseDeviceDependentResources();

    void SetTextColor(D2D1_COLOR_F const& textColor);
    void SetTextOpacity(float textOpacity);

    void SetText(__nullterminated WCHAR* text);
    void SetText(std::wstring const& text);

    TextStyle& GetTextStyle() { return m_textStyle; }

    void FadeOut(float fadeOutTime);

protected:
    virtual void CalculateSize();

    std::wstring        m_text;
    D2D1_RECT_F         m_textExtents;

    TextStyle           m_textStyle;

    bool                m_isFadingOut;
    float               m_fadeStartingOpacity;
    float               m_fadeOutTime;
    float               m_fadeOutTimeElapsed;

    winrt::com_ptr<ID2D1SolidColorBrush>    m_textColorBrush;
    winrt::com_ptr<ID2D1SolidColorBrush>    m_shadowColorBrush;
    winrt::com_ptr<IDWriteTextLayout>       m_textLayout;

    void CreateTextLayout();
};

