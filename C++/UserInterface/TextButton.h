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

class TextButton : public TextElement
{
public:
    TextButton();

    virtual void Initialize();
    virtual void Update(float timeTotal, float timeDelta);
    virtual void Render();

    void SetPadding(D2D1_SIZE_F padding);
    void SetSelected(bool selected);

    bool GetSelected() const { return m_selected; }

    void SetPressed(bool pressed);
    bool IsPressed() const { return m_pressed; }

protected:
    virtual void CalculateSize();

    D2D1_SIZE_F     m_padding;
    bool            m_selected;
    bool            m_pressed;
};
