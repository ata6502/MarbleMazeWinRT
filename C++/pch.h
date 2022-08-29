// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#pragma once
#define NOMINMAX

#include <d2d1_3.h>
#include <d3d11_3.h>
#include <dwrite_3.h>
#include <dxgi1_3.h>
#include <wincodec.h>

#define XM_STRICT_VECTOR4 1
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#define XAUDIO2_HELPER_FUNCTIONS 1
#include <xaudio2.h>
#include <xaudio2fx.h>

#include <mmreg.h>
#include <mfidl.h>
#include <mfapi.h>
#include <mfreadwrite.h>
#include <mfmediaengine.h>

#include <XInput.h> 

#include <stdio.h>
#include <strsafe.h>

#include <memory>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Devices.Sensors.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Gaming.Input.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>

#include <crtdbg.h> // Microsoft's implementation of CRT; defines _ASSERTE

// Run-time assertion. ASSERT is evaluated only in Debug builds.
#define ASSERT _ASSERTE

#if defined(_DEBUG)
// Examples: 
// DebugTrace(L"num = %4.2f\n", num);
// DebugTrace(L"%s\n", str.c_str());
static void DebugTrace(const wchar_t* format, ...)
{
    // Generate the message string.
    va_list args;
    va_start(args, format); // initialize the argument list
    wchar_t buffer[1024];
    ASSERT(_vsnwprintf_s(buffer, _countof(buffer) - 1, format, args) != -1);
    va_end(args);

    OutputDebugStringW(buffer); // this is a Windows function
}
#endif
