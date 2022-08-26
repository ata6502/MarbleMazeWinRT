// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#include "pch.h"

#include "TextElement.h"
#include "UserInterface.h"

using namespace D2D1;

TextElement::TextElement() :
    m_isFadingOut(false),
    m_fadeOutTime(0.f),
    m_fadeOutTimeElapsed(0.f),
    m_fadeStartingOpacity(0.f)
{
    ZeroMemory(&m_textExtents, sizeof(m_textExtents));
}

void TextElement::Initialize()
{
    ID2D1DeviceContext* d2dContext = UserInterface::GetD2DContext();

    winrt::check_hresult(
        d2dContext->CreateSolidColorBrush(
            ColorF(ColorF::White),
            m_textColorBrush.put()
        )
    );

    winrt::check_hresult(
        d2dContext->CreateSolidColorBrush(
            ColorF(ColorF::Black),
            m_shadowColorBrush.put()
        )
    );
}

void TextElement::Update([[maybe_unused]] float timeTotal, float timeDelta)
{
    if (m_isFadingOut)
    {
        m_fadeOutTimeElapsed += timeDelta;

        float delta = std::min(1.0f, m_fadeOutTimeElapsed / m_fadeOutTime);
        SetTextOpacity((1.0f - delta) * m_fadeStartingOpacity);

        if (m_fadeOutTimeElapsed >= m_fadeOutTime)
        {
            m_isFadingOut = false;
            SetVisible(false);
        }
    }
}

void TextElement::Render()
{
    ID2D1DeviceContext* d2dContext = UserInterface::GetD2DContext();

    D2D1_RECT_F bounds = GetBounds();
    D2D1_POINT_2F origin = Point2F(
        bounds.left - m_textExtents.left,
        bounds.top - m_textExtents.top
    );

    m_shadowColorBrush->SetOpacity(m_textColorBrush->GetOpacity() * 0.5f);

    d2dContext->DrawTextLayout(
        Point2F(origin.x + 4.0f, origin.y + 4.0f),
        m_textLayout.get(),
        m_shadowColorBrush.get(),
        D2D1_DRAW_TEXT_OPTIONS_NO_SNAP
    );

    d2dContext->DrawTextLayout(
        origin,
        m_textLayout.get(),
        m_textColorBrush.get(),
        D2D1_DRAW_TEXT_OPTIONS_NO_SNAP
    );
}

void TextElement::ReleaseDeviceDependentResources()
{
    m_textColorBrush = nullptr;
    m_shadowColorBrush = nullptr;
}

void TextElement::SetTextColor(D2D1_COLOR_F const& textColor)
{
    m_textColorBrush->SetColor(textColor);
}

void TextElement::SetTextOpacity(float textOpacity)
{
    m_textColorBrush->SetOpacity(textOpacity);
}

void TextElement::SetText(__nullterminated WCHAR* text)
{
    SetText(std::wstring(text));
}

void TextElement::SetText(std::wstring const& text)
{
    if (m_text != text)
    {
        m_text = text;
        m_textLayout = nullptr;
    }
}

void TextElement::FadeOut(float fadeOutTime)
{
    m_fadeStartingOpacity = m_textColorBrush->GetOpacity();
    m_fadeOutTime = fadeOutTime;
    m_fadeOutTimeElapsed = 0.0f;
    m_isFadingOut = true;
}

void TextElement::CalculateSize()
{
    CreateTextLayout();

    DWRITE_TEXT_METRICS metrics;
    ZeroMemory(&metrics, sizeof(metrics));
    DWRITE_OVERHANG_METRICS overhangMetrics;
    ZeroMemory(&overhangMetrics, sizeof(overhangMetrics));

    winrt::check_hresult(
        m_textLayout->GetMetrics(&metrics)
    );
    winrt::check_hresult(
        m_textLayout->GetOverhangMetrics(&overhangMetrics)
    );

    m_textExtents = RectF(
        -overhangMetrics.left,
        -overhangMetrics.top,
        overhangMetrics.right + metrics.layoutWidth,
        overhangMetrics.bottom + metrics.layoutHeight
    );

    m_size = SizeF(
        m_textExtents.right - m_textExtents.left,
        m_textExtents.bottom - m_textExtents.top
    );
}

void TextElement::CreateTextLayout()
{
    if ((m_textLayout == nullptr) || m_textStyle.HasTextFormatChanged())
    {
        m_textLayout = nullptr;

        IDWriteFactory* dwriteFactory = UserInterface::GetDWriteFactory();

        winrt::check_hresult(
            dwriteFactory->CreateTextLayout(
                m_text.c_str(),
                static_cast<uint32_t>(m_text.size()),
                m_textStyle.GetTextFormat(),
                m_container.right - m_container.left,
                m_container.bottom - m_container.top,
                m_textLayout.put()
            )
        );
    }
}
