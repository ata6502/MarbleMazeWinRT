// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#include "pch.h"

#include "TextButton.h"
#include "UserInterface.h"

TextButton::TextButton() :
    m_selected(false),
    m_pressed(false),
    m_padding(D2D1_SIZE_F())
{
}

void TextButton::Initialize()
{
    TextElement::Initialize();
}

void TextButton::Update(float timeTotal, float timeDelta)
{
    TextElement::Update(timeTotal, timeDelta);
}

void TextButton::Render()
{
    ID2D1DeviceContext* d2dContext = UserInterface::GetD2DContext();

    if (m_selected)
    {
        D2D1_RECT_F bounds = GetBounds();

        d2dContext->DrawRectangle(
            bounds,
            m_textColorBrush.get(),
            4.0f
        );
    }

    TextElement::Render();
}

void TextButton::SetPadding(D2D1_SIZE_F padding)
{
    m_padding = padding;
}

void TextButton::SetSelected(bool selected)
{
    m_selected = selected;
}

void TextButton::SetPressed(bool pressed)
{
    m_pressed = pressed;
}

void TextButton::CalculateSize()
{
    TextElement::CalculateSize();
    m_textExtents.left -= m_padding.width;
    m_textExtents.top -= m_padding.height;
    m_size.width += m_padding.width * 2;
    m_size.height += m_padding.height * 2;
}
