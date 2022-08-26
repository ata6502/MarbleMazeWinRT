// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#pragma once

class MediaStreamer
{
private:
    WAVEFORMATEX    m_waveFormat;
    uint32_t        m_maxStreamLengthInBytes;

public:
    winrt::com_ptr<IMFSourceReader> m_reader;
    winrt::com_ptr<IMFMediaType> m_audioType;

public:
    MediaStreamer();
    ~MediaStreamer();

    WAVEFORMATEX& GetOutputWaveFormatEx()
    {
        return m_waveFormat;
    }

    uint32_t GetMaxStreamLengthInBytes()
    {
        return m_maxStreamLengthInBytes;
    }

    void Initialize(_In_ const WCHAR* url);
    bool GetNextBuffer(uint8_t* buffer, uint32_t maxBufferSize, uint32_t* bufferLength);
    void ReadAll(uint8_t* buffer, uint32_t maxBufferSize, uint32_t* bufferLength);
    void Restart();
};