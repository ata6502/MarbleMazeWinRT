// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#include "pch.h"

#include "TextStyle.h"
#include "UserInterface.h"

TextStyle::TextStyle()
{
    m_fontName = L"Segoe UI";
    m_fontSize = 24.0f;
    m_fontWeight = DWRITE_FONT_WEIGHT_NORMAL;
    m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
    m_textAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
}

void TextStyle::SetFontName(std::wstring const& fontName)
{
    if (m_fontName != fontName)
    {
        m_fontName = fontName;
        m_textFormat = nullptr;
    }
}

void TextStyle::SetFontSize(float fontSize)
{
    if (m_fontSize != fontSize)
    {
        m_fontSize = fontSize;
        m_textFormat = nullptr;
    }
}

void TextStyle::SetFontWeight(DWRITE_FONT_WEIGHT fontWeight)
{
    if (m_fontWeight != fontWeight)
    {
        m_fontWeight = fontWeight;
        m_textFormat = nullptr;
    }
}

void TextStyle::SetFontStyle(DWRITE_FONT_STYLE fontStyle)
{
    if (m_fontStyle != fontStyle)
    {
        m_fontStyle = fontStyle;
        m_textFormat = nullptr;
    }
}

void TextStyle::SetTextAlignment(DWRITE_TEXT_ALIGNMENT textAlignment)
{
    if (m_textAlignment != textAlignment)
    {
        m_textAlignment = textAlignment;
        m_textFormat = nullptr;
    }
}

IDWriteTextFormat* TextStyle::GetTextFormat()
{
    if (m_textFormat == nullptr)
    {
        IDWriteFactory* dwriteFactory = UserInterface::GetDWriteFactory();

        winrt::check_hresult(
            dwriteFactory->CreateTextFormat(
                m_fontName.c_str(),
                nullptr,
                m_fontWeight,
                m_fontStyle,
                DWRITE_FONT_STRETCH_NORMAL,
                m_fontSize,
                L"en-US",
                m_textFormat.put()
            )
        );

        winrt::check_hresult(
            m_textFormat->SetTextAlignment(m_textAlignment)
        );

        winrt::check_hresult(
            m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
        );
    }

    return m_textFormat.get();
}
