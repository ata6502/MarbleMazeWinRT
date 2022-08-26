// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#pragma once

class TextStyle
{
public:
    TextStyle();

    void SetFontName(std::wstring const& fontName);
    void SetFontSize(float fontSize);
    void SetFontWeight(DWRITE_FONT_WEIGHT fontWeight);
    void SetFontStyle(DWRITE_FONT_STYLE fontStyle);
    void SetTextAlignment(DWRITE_TEXT_ALIGNMENT textAlignment);

    IDWriteTextFormat* GetTextFormat();

    bool HasTextFormatChanged() { return (m_textFormat == nullptr); }

private:
    std::wstring            m_fontName;
    float                   m_fontSize;
    DWRITE_FONT_WEIGHT      m_fontWeight;
    DWRITE_FONT_STYLE       m_fontStyle;
    DWRITE_TEXT_ALIGNMENT   m_textAlignment;

    winrt::com_ptr<IDWriteTextFormat> m_textFormat;
};

