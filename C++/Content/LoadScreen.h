// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#pragma once

class LoadScreen
{
public:
    LoadScreen();

    void Initialize(
        _In_ ID2D1Device* d2dDevice,
        _In_ ID2D1DeviceContext* d2dContext,
        _In_ IWICImagingFactory* wicFactory
    );

    void ResetDirectXResources();
    void ReleaseDeviceDependentResources();

    void UpdateForWindowSizeChange();

    void Render(D2D1::Matrix3x2F orientation2D);

private:
    winrt::com_ptr<ID2D1Factory1>           m_d2dFactory;
    winrt::com_ptr<ID2D1Device>             m_d2dDevice;
    winrt::com_ptr<ID2D1DeviceContext>      m_d2dContext;
    winrt::com_ptr<ID2D1DrawingStateBlock>  m_stateBlock;

    winrt::com_ptr<IWICImagingFactory>      m_wicFactory;
    winrt::com_ptr<ID2D1Bitmap>             m_bitmap;

    D2D1_SIZE_F                             m_imageSize;
    D2D1_SIZE_F                             m_offset;
    D2D1_SIZE_F                             m_totalSize;
};
