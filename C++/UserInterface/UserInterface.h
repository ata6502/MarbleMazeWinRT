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

class UserInterface
{
public:
    static UserInterface& GetInstance() { return m_instance; }

    static IDWriteFactory* GetDWriteFactory() { return m_instance.m_dwriteFactory.get(); }
    static ID2D1DeviceContext* GetD2DContext() { return m_instance.m_d2dContext.get(); }
    static void ReleaseDeviceDependentResources();

    void Initialize(
        _In_ ID2D1Device* d2dDevice,
        _In_ ID2D1DeviceContext* d2dContext,
        _In_ IWICImagingFactory* wicFactory,
        _In_ IDWriteFactory* dwriteFactory
    );

    void Release()
    {
        m_d2dFactory = nullptr;
        m_d2dContext = nullptr;
        m_dwriteFactory = nullptr;
        m_stateBlock = nullptr;
        m_wicFactory = nullptr;
    }

    void Update(float timeTotal, float timeDelta);
    void Render(D2D1::Matrix3x2F orientation2D);

    void RegisterElement(ElementBase* element);
    void UnregisterElement(ElementBase* element);

    void HitTest(D2D1_POINT_2F point);

private:
    UserInterface() {}
    ~UserInterface() {}

    static UserInterface m_instance;

    winrt::com_ptr<ID2D1Factory1>           m_d2dFactory;
    winrt::com_ptr<ID2D1DeviceContext>      m_d2dContext;
    winrt::com_ptr<IDWriteFactory>          m_dwriteFactory;
    winrt::com_ptr<ID2D1DrawingStateBlock>  m_stateBlock;
    winrt::com_ptr<IWICImagingFactory>      m_wicFactory;

    ElementSet m_elements;
};

