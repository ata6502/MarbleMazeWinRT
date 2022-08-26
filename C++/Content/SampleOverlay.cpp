// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#include "pch.h"
#include "SampleOverlay.h"

using namespace MarbleMaze;
using namespace D2D1;
using namespace winrt::Windows::UI::Core;

SampleOverlay::SampleOverlay(std::shared_ptr<DX::DeviceResources> const& deviceResources, std::wstring const& caption) :
    m_deviceResources(deviceResources),
    m_caption(caption),
    m_drawOverlay(true),
    m_padding(8.0f)
{
    m_textVerticalOffset = 14.0f;
    m_logoSize = D2D1::SizeF(0.0f, 0.0f);
    m_overlayWidth = 0.0f;

    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

void SampleOverlay::CreateDeviceDependentResources()
{
    winrt::check_hresult(
        m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(ColorF(ColorF::White), m_whiteBrush.put())
    );

    winrt::com_ptr<IWICBitmapDecoder> wicBitmapDecoder;
    winrt::check_hresult(
        m_deviceResources->GetWicImagingFactory()->CreateDecoderFromFilename(
            L"Media\\Textures\\windowstitle-sdk.png",
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
        m_deviceResources->GetWicImagingFactory()->CreateFormatConverter(wicFormatConverter.put())
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

    double dpiX = 96.0;
    double dpiY = 96.0;
    winrt::check_hresult(
        wicFormatConverter->GetResolution(&dpiX, &dpiY)
    );

    winrt::check_hresult(
        m_deviceResources->GetD2DDeviceContext()->CreateBitmapFromWicBitmap(
            wicFormatConverter.get(),
            BitmapProperties(
                PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                static_cast<float>(dpiX),
                static_cast<float>(dpiY)
            ),
            m_logoBitmap.put()
        )
    );

    m_logoSize = m_logoBitmap->GetSize();

    winrt::com_ptr<IDWriteTextFormat> nameTextFormat;
    winrt::check_hresult(
        m_deviceResources->GetDWriteFactory()->CreateTextFormat(
            L"Segoe UI",
            nullptr,
            DWRITE_FONT_WEIGHT_LIGHT,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            36.0f,
            L"en-US",
            nameTextFormat.put()
        )
    );

    winrt::check_hresult(
        nameTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)
    );

    winrt::check_hresult(
        nameTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
    );

    winrt::check_hresult(
        m_deviceResources->GetDWriteFactory()->CreateTextLayout(
            m_caption.c_str(),
            static_cast<uint32_t>(m_caption.size()),
            nameTextFormat.get(),
            4096.0f,
            4096.0f,
            m_textLayout.put()
        )
    );

    DWRITE_TEXT_METRICS metrics = { 0 };
    winrt::check_hresult(
        m_textLayout->GetMetrics(&metrics)
    );

    m_overlayWidth = m_padding * 3.0f + m_logoSize.width + metrics.width;

    winrt::check_hresult(
        m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(m_stateBlock.put())
    );
}

void SampleOverlay::CreateWindowSizeDependentResources()
{
    if (CoreWindow::GetForCurrentThread().Bounds().Width < m_overlayWidth)
    {
        m_drawOverlay = false;
    }
    else
    {
        m_drawOverlay = true;
    }
}

// Release all references to resources that depend on the graphics device.
// This method is invoked when the device is lost and resources are no longer usable.
void SampleOverlay::ReleaseDeviceDependentResources()
{
    m_whiteBrush = nullptr;
    m_stateBlock = nullptr;
    m_logoBitmap = nullptr;
    m_textLayout = nullptr;
}

void SampleOverlay::Render()
{
    if (m_drawOverlay)
    {
        m_deviceResources->GetD2DDeviceContext()->SaveDrawingState(m_stateBlock.get());

        m_deviceResources->GetD2DDeviceContext()->BeginDraw();
        m_deviceResources->GetD2DDeviceContext()->SetTransform(m_deviceResources->GetOrientationTransform2D());
        m_deviceResources->GetD2DDeviceContext()->DrawBitmap(
            m_logoBitmap.get(),
            D2D1::RectF(m_padding, m_padding, m_logoSize.width + m_padding, m_logoSize.height + m_padding)
        );

        m_deviceResources->GetD2DDeviceContext()->DrawTextLayout(
            Point2F(m_logoSize.width + 2.0f * m_padding, m_textVerticalOffset),
            m_textLayout.get(),
            m_whiteBrush.get()
        );

        // We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
        // is lost. It will be handled during the next call to Present.
        HRESULT hr = m_deviceResources->GetD2DDeviceContext()->EndDraw();
        if (hr != D2DERR_RECREATE_TARGET)
        {
            winrt::check_hresult(hr);
        }

        m_deviceResources->GetD2DDeviceContext()->RestoreDrawingState(m_stateBlock.get());
    }
}

float SampleOverlay::GetTitleHeightInDips()
{
    return m_logoSize.height + m_padding;
}
