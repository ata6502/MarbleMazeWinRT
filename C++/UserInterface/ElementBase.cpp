// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#include "pch.h"
#include "ElementBase.h"

ElementBase::ElementBase() :
    m_visible(false)
{
    m_alignment.horizontal = AlignCenter;
    m_alignment.vertical = AlignCenter;

    ZeroMemory(&m_container, sizeof(m_container));
    ZeroMemory(&m_size, sizeof(m_size));
}

void ElementBase::SetAlignment(AlignType horizontal, AlignType vertical)
{
    m_alignment.horizontal = horizontal;
    m_alignment.vertical = vertical;
}

void ElementBase::SetContainer(D2D1_RECT_F const& container)
{
    m_container = container;
}

void ElementBase::SetVisible(bool visible)
{
    m_visible = visible;
}

D2D1_RECT_F ElementBase::GetBounds()
{
    CalculateSize();

    D2D1_RECT_F bounds = D2D1::RectF();

    switch (m_alignment.horizontal)
    {
    case AlignNear:
        bounds.left = m_container.left;
        bounds.right = bounds.left + m_size.width;
        break;

    case AlignCenter:
        bounds.left = m_container.left + (m_container.right - m_container.left - m_size.width) / 2.0f;
        bounds.right = bounds.left + m_size.width;
        break;

    case AlignFar:
        bounds.right = m_container.right;
        bounds.left = bounds.right - m_size.width;
        break;
    }

    switch (m_alignment.vertical)
    {
    case AlignNear:
        bounds.top = m_container.top;
        bounds.bottom = bounds.top + m_size.height;
        break;

    case AlignCenter:
        bounds.top = m_container.top + (m_container.bottom - m_container.top - m_size.height) / 2.0f;
        bounds.bottom = bounds.top + m_size.height;
        break;

    case AlignFar:
        bounds.bottom = m_container.bottom;
        bounds.top = bounds.bottom - m_size.height;
        break;
    }

    return bounds;
}
