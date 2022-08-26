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

UserInterface UserInterface::m_instance;

void UserInterface::Initialize(
    _In_ ID2D1Device* d2dDevice,
    _In_ ID2D1DeviceContext* d2dContext,
    _In_ IWICImagingFactory* wicFactory,
    _In_ IDWriteFactory* dwriteFactory
)
{
    m_wicFactory.copy_from(wicFactory);
    m_dwriteFactory.copy_from(dwriteFactory);
    m_d2dContext.copy_from(d2dContext);

    winrt::com_ptr<ID2D1Factory> factory;
    d2dDevice->GetFactory(factory.put());

    m_d2dFactory = factory.as<ID2D1Factory1>();

    winrt::check_hresult(
        m_d2dFactory->CreateDrawingStateBlock(m_stateBlock.put())
    );
}

void UserInterface::ReleaseDeviceDependentResources()
{
    m_instance.Release();

    auto elements = m_instance.m_elements;
    for (auto iterator = elements.begin(); iterator != elements.end(); iterator++)
    {
        (*iterator)->ReleaseDeviceDependentResources();
    }
}

void UserInterface::Update(float timeTotal, float timeDelta)
{
    for (auto iter = m_elements.begin(); iter != m_elements.end(); ++iter)
    {
        (*iter)->Update(timeTotal, timeDelta);
    }
}

void UserInterface::Render(D2D1::Matrix3x2F orientation2D)
{
    m_d2dContext->SaveDrawingState(m_stateBlock.get());
    m_d2dContext->BeginDraw();
    m_d2dContext->SetTransform(orientation2D);

    m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    for (auto iter = m_elements.begin(); iter != m_elements.end(); ++iter)
    {
        if ((*iter)->IsVisible())
            (*iter)->Render();
    }

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        winrt::check_hresult(hr);
    }

    m_d2dContext->RestoreDrawingState(m_stateBlock.get());
}

void UserInterface::RegisterElement(ElementBase* element)
{
    m_elements.insert(element);
}

void UserInterface::UnregisterElement(ElementBase* element)
{
    auto iter = m_elements.find(element);
    if (iter != m_elements.end())
    {
        m_elements.erase(iter);
    }
}

inline bool PointInRect(D2D1_POINT_2F point, D2D1_RECT_F rect)
{
    if ((point.x < rect.left) ||
        (point.x > rect.right) ||
        (point.y < rect.top) ||
        (point.y > rect.bottom))
    {
        return false;
    }

    return true;
}

void UserInterface::HitTest(D2D1_POINT_2F point)
{
    for (auto iter = m_elements.begin(); iter != m_elements.end(); ++iter)
    {
        if (!(*iter)->IsVisible())
            continue;

        TextButton* textButton = dynamic_cast<TextButton*>(*iter);
        if (textButton != nullptr)
        {
            D2D1_RECT_F bounds = (*iter)->GetBounds();
            textButton->SetPressed(PointInRect(point, bounds));
        }
    }
}
