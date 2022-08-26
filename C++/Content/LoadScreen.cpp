// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#include "pch.h"
#include "LoadScreen.h"

using namespace D2D1;
using namespace winrt::Windows::UI::Core;

LoadScreen::LoadScreen()
{
    ZeroMemory(&m_imageSize, sizeof(m_imageSize));
    ZeroMemory(&m_offset, sizeof(m_offset));
    ZeroMemory(&m_totalSize, sizeof(m_totalSize));
}

void LoadScreen::Initialize(
    _In_ ID2D1Device* d2dDevice,
    _In_ ID2D1DeviceContext* d2dContext,
    _In_ IWICImagingFactory* wicFactory
)
{
    m_wicFactory.copy_from(wicFactory);
    m_d2dDevice.copy_from(d2dDevice);
    m_d2dContext.copy_from(d2dContext);

    m_imageSize = D2D1::SizeF(0.0f, 0.0f);
    m_offset = D2D1::SizeF(0.0f, 0.0f);
    m_totalSize = D2D1::SizeF(0.0f, 0.0f);

    winrt::com_ptr<ID2D1Factory> factory;
    d2dDevice->GetFactory(factory.put());

    m_d2dFactory = factory.as<ID2D1Factory1>();

    ResetDirectXResources();
}

void LoadScreen::ResetDirectXResources()
{
    winrt::com_ptr<IWICBitmapDecoder> wicBitmapDecoder;
    winrt::check_hresult(
        m_wicFactory->CreateDecoderFromFilename(
            L"Media\\Textures\\loadscreen.png",
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnDemand,
            wicBitmapDecoder.put()
        )
    );

    winrt::com_ptr<IWICBitmapFrameDecode> wicBitmapFrame;
    winrt::check_hresult(
        wicBitmapDecoder->GetFrame(0, wicBitmapFrame.put())
    );

    winrt::com_ptr<IWICFormatConverter> wicFormatConverter;
    winrt::check_hresult(
        m_wicFactory->CreateFormatConverter(wicFormatConverter.put())
    );

    winrt::check_hresult(
        wicFormatConverter->Initialize(
            wicBitmapFrame.get(),
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeCustom  // the BGRA format has no palette so this value is ignored
        )
    );

    winrt::check_hresult(
        m_d2dContext->CreateBitmapFromWicBitmap(
            wicFormatConverter.get(),
            BitmapProperties(PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
            m_bitmap.put()
        )
    );

    m_imageSize = m_bitmap->GetSize();

    winrt::check_hresult(
        m_d2dFactory->CreateDrawingStateBlock(m_stateBlock.put())
    );

    UpdateForWindowSizeChange();
}

void LoadScreen::ReleaseDeviceDependentResources()
{
    m_d2dDevice = nullptr;
    m_d2dContext = nullptr;
    m_bitmap = nullptr;
    m_d2dFactory = nullptr;
    m_stateBlock = nullptr;
    m_wicFactory = nullptr;
}

void LoadScreen::UpdateForWindowSizeChange()
{
    winrt::Windows::Foundation::Rect windowBounds = CoreWindow::GetForCurrentThread().Bounds();

    m_offset.width = (windowBounds.Width - m_imageSize.width) / 2.0f;
    m_offset.height = (windowBounds.Height - m_imageSize.height) / 2.0f;

    m_totalSize.width = m_offset.width + m_imageSize.width;
    m_totalSize.height = m_offset.height + m_imageSize.height;
}

void LoadScreen::Render(D2D1::Matrix3x2F orientation2D)
{
    m_d2dContext->SaveDrawingState(m_stateBlock.get());

    m_d2dContext->BeginDraw();
    m_d2dContext->SetTransform(orientation2D);
    m_d2dContext->DrawBitmap(
        m_bitmap.get(),
        D2D1::RectF(
            m_offset.width,
            m_offset.height,
            m_totalSize.width,
            m_totalSize.height)
    );

    // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    // is lost. It will be handled during the next call to Present.
    HRESULT hr = m_d2dContext->EndDraw();
    if (hr != D2DERR_RECREATE_TARGET)
    {
        winrt::check_hresult(hr);
    }

    m_d2dContext->RestoreDrawingState(m_stateBlock.get());
}
