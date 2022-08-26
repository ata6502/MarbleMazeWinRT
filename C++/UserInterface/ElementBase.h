// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#pragma once

enum AlignType
{
    AlignNear,
    AlignCenter,
    AlignFar,
};

struct Alignment
{
    AlignType horizontal;
    AlignType vertical;
};

class ElementBase
{
public:
    virtual void Initialize() { }
    virtual void Update([[maybe_unused]] float timeTotal, [[maybe_unused]] float timeDelta) { }
    virtual void Render() { }
    virtual void ReleaseDeviceDependentResources() { }

    void SetAlignment(AlignType horizontal, AlignType vertical);
    virtual void SetContainer(D2D1_RECT_F const& container);
    void SetVisible(bool visible);

    D2D1_RECT_F GetBounds();

    bool IsVisible() const { return m_visible; }

protected:
    ElementBase();

    virtual void CalculateSize() { }

    Alignment       m_alignment;
    D2D1_RECT_F     m_container;
    D2D1_SIZE_F     m_size;
    bool            m_visible;
};

typedef std::set<ElementBase*> ElementSet;
