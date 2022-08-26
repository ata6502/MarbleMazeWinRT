// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#include "pch.h"
#include "MediaStreamer.h"

MediaStreamer::MediaStreamer()
{
    m_reader = nullptr;
    m_audioType = nullptr;
    ZeroMemory(&m_waveFormat, sizeof(m_waveFormat));
}

MediaStreamer::~MediaStreamer()
{
}

void MediaStreamer::Initialize(_In_ const WCHAR* url)
{
    winrt::com_ptr<IMFMediaType> outputMediaType;
    winrt::com_ptr<IMFMediaType> mediaType;

    winrt::check_hresult(
        MFStartup(MF_VERSION)
    );

    winrt::check_hresult(
        MFCreateSourceReaderFromURL(url, nullptr, m_reader.put())
    );

    // Set the decoded output format as PCM.
    // XAudio2 on Windows can process PCM and ADPCM-encoded buffers.
    // When this sample uses Media Foundation, it always decodes into PCM.

    winrt::check_hresult(
        MFCreateMediaType(mediaType.put())
    );

    winrt::check_hresult(
        mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio)
    );

    winrt::check_hresult(
        mediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM)
    );

    winrt::check_hresult(
        m_reader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, mediaType.get())
    );

    // Get the complete WAVEFORMAT from the Media Type.
    winrt::check_hresult(
        m_reader->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), outputMediaType.put())
    );

    uint32_t formatSize = 0;
    WAVEFORMATEX* waveFormat;
    winrt::check_hresult(
        MFCreateWaveFormatExFromMFMediaType(outputMediaType.get(), &waveFormat, &formatSize)
    );
    CopyMemory(&m_waveFormat, waveFormat, sizeof(m_waveFormat));
    CoTaskMemFree(waveFormat);

    // Get the total length of the stream, in bytes.
    PROPVARIANT var;
    winrt::check_hresult(
        m_reader->GetPresentationAttribute(static_cast<DWORD>(MF_SOURCE_READER_MEDIASOURCE), MF_PD_DURATION, &var)
    );

    // duration is in 100ns units; convert to seconds, and round up
    // to the nearest whole byte.
    ULONGLONG duration = var.uhVal.QuadPart;
    m_maxStreamLengthInBytes =
        static_cast<unsigned int>(
            ((duration * static_cast<ULONGLONG>(m_waveFormat.nAvgBytesPerSec)) + 10000000) /
            10000000
            );
}

bool MediaStreamer::GetNextBuffer(uint8_t* buffer, uint32_t maxBufferSize, uint32_t* bufferLength)
{
    winrt::com_ptr<IMFSample> sample;
    winrt::com_ptr<IMFMediaBuffer> mediaBuffer;
    BYTE *audioData = nullptr;
    DWORD sampleBufferLength = 0;
    DWORD flags = 0;

    *bufferLength = 0;

    if (m_reader == nullptr)
    {
        return false;
    }

    winrt::check_hresult(
        m_reader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, nullptr, &flags, nullptr, sample.put())
    );

    if (sample == nullptr)
    {
        if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    winrt::check_hresult(
        sample->ConvertToContiguousBuffer(mediaBuffer.put())
    );

    winrt::check_hresult(
        mediaBuffer->Lock(&audioData, nullptr, &sampleBufferLength)
    );

    // Only copy the sample if the remaining buffer is large enough.
    if (sampleBufferLength <= maxBufferSize)
    {
        CopyMemory(buffer, audioData, sampleBufferLength);
        *bufferLength = sampleBufferLength;
    }

    if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MediaStreamer::ReadAll(uint8_t* buffer, uint32_t maxBufferSize, uint32_t* bufferLength)
{
    uint32_t valuesWritten = 0;
    uint32_t sampleBufferLength = 0;

    if (m_reader == nullptr)
    {
        return;
    }

    *bufferLength = 0;
    // If buffer isn't large enough, return
    if (maxBufferSize < m_maxStreamLengthInBytes)
    {
        return;
    }

    while (!GetNextBuffer(buffer + valuesWritten, maxBufferSize - valuesWritten, &sampleBufferLength))
    {
        valuesWritten += sampleBufferLength;
    }

    *bufferLength = valuesWritten + sampleBufferLength;
}

void MediaStreamer::Restart()
{
    if (m_reader == nullptr)
    {
        return;
    }

    PROPVARIANT var = {0};
    var.vt = VT_I8;

    winrt::check_hresult(
        m_reader->SetCurrentPosition(GUID_NULL, var)
    );
}
