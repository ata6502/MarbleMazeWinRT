﻿// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#pragma once

namespace DX
{
    // Provides an interface for an application that owns DeviceResources to be notified of the device being lost or created.
    interface IDeviceNotify
    {
        virtual void OnDeviceLost() = 0;
        virtual void OnDeviceRestored() = 0;
    };

    // Controls all the DirectX device resources.
    class DeviceResources
    {
    public:
        DeviceResources();
        void SetWindow(winrt::Windows::UI::Core::CoreWindow const& window);
        void SetLogicalSize(winrt::Windows::Foundation::Size logicalSize);
        void SetCurrentOrientation(winrt::Windows::Graphics::Display::DisplayOrientations currentOrientation);
        void SetDpi(float dpi);
        void ValidateDevice();
        void HandleDeviceLost();
        void RegisterDeviceNotify(IDeviceNotify* deviceNotify);
        void Trim();
        void Present();

        // Device Accessors.
        winrt::Windows::Foundation::Size GetOutputSize() const          { return m_outputSize; }
        winrt::Windows::Foundation::Size GetLogicalSize() const         { return m_logicalSize; }

        // D3D Accessors.
        winrt::com_ptr<ID3D11Device3> const& GetD3DDevice() const       { return m_d3dDevice; }
        ID3D11DeviceContext3* GetD3DDeviceContext() const               { return m_d3dContext.get(); }
        IDXGISwapChain1* GetSwapChain() const                           { return m_swapChain.get(); }
        D3D_FEATURE_LEVEL GetDeviceFeatureLevel() const                 { return m_d3dFeatureLevel; }
        ID3D11RenderTargetView* GetBackBufferRenderTargetView() const   { return m_d3dRenderTargetView.get(); }
        ID3D11DepthStencilView* GetDepthStencilView() const             { return m_d3dDepthStencilView.get(); }
        D3D11_VIEWPORT GetScreenViewport() const                        { return m_screenViewport; }
        DirectX::XMFLOAT4X4 GetOrientationTransform3D() const           { return m_orientationTransform3D; }

        // D2D Accessors.
        ID2D1Factory3* GetD2DFactory() const                            { return m_d2dFactory.get(); }
        ID2D1Device2* GetD2DDevice() const                              { return m_d2dDevice.get(); }
        ID2D1DeviceContext2* GetD2DDeviceContext() const                { return m_d2dContext.get(); }
        ID2D1Bitmap1* GetD2DTargetBitmap() const                        { return m_d2dTargetBitmap.get(); }
        IDWriteFactory3* GetDWriteFactory() const                       { return m_dwriteFactory.get(); }
        IWICImagingFactory2* GetWicImagingFactory() const               { return m_wicFactory.get(); }
        D2D1::Matrix3x2F GetOrientationTransform2D() const              { return m_orientationTransform2D; }

    private:
        void CreateDeviceIndependentResources();
        void CreateDeviceResources();
        void CreateWindowSizeDependentResources();

        DXGI_MODE_ROTATION ComputeDisplayRotation();

        // Direct3D objects.
        winrt::com_ptr<ID3D11Device3>                                   m_d3dDevice;
        winrt::com_ptr<ID3D11DeviceContext3>                            m_d3dContext;
        winrt::com_ptr<IDXGISwapChain1>                                 m_swapChain;

        // Direct3D rendering objects. Required for 3D.
        winrt::com_ptr<ID3D11RenderTargetView>                          m_d3dRenderTargetView;
        winrt::com_ptr<ID3D11DepthStencilView>                          m_d3dDepthStencilView;
        D3D11_VIEWPORT                                                  m_screenViewport;

        // Direct2D drawing components.
        winrt::com_ptr<ID2D1Factory3>                                   m_d2dFactory;
        winrt::com_ptr<ID2D1Device2>                                    m_d2dDevice;
        winrt::com_ptr<ID2D1DeviceContext2>                             m_d2dContext;
        winrt::com_ptr<ID2D1Bitmap1>                                    m_d2dTargetBitmap;

        // DirectWrite drawing components.
        winrt::com_ptr<IDWriteFactory3>                                 m_dwriteFactory;
        winrt::com_ptr<IWICImagingFactory2>                             m_wicFactory;

        // Cached reference to the Window.
        winrt::agile_ref<winrt::Windows::UI::Core::CoreWindow>          m_window{ nullptr };

        // Cached device properties.
        D3D_FEATURE_LEVEL                                               m_d3dFeatureLevel;
        winrt::Windows::Foundation::Size                                m_d3dRenderTargetSize;
        winrt::Windows::Foundation::Size                                m_outputSize;
        winrt::Windows::Foundation::Size                                m_logicalSize;
        winrt::Windows::Graphics::Display::DisplayOrientations          m_nativeOrientation;
        winrt::Windows::Graphics::Display::DisplayOrientations          m_currentOrientation;
        float                                                           m_dpi;

        // Transforms used for display orientation.
        D2D1::Matrix3x2F                                                m_orientationTransform2D;
        DirectX::XMFLOAT4X4                                             m_orientationTransform3D;

        // The IDeviceNotify can be held directly as it owns the DeviceResources.
        IDeviceNotify* m_deviceNotify;
    };
}
