// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#pragma once

// A simple helper class that provides support for saving and loading various
// data types to an IPropertySet. Used by DirectX SDK samples to implement
// process lifetime management (PLM).
class PersistentState
{
public:
    void Initialize(
        _In_ winrt::Windows::Foundation::Collections::IPropertySet settingsValues,
        _In_ std::wstring const& key
        );

    void SaveBool(std::wstring const& key, bool value);
    void SaveInt32(std::wstring const& key, int value);
    void SaveSingle(std::wstring const& key, float value);
    void SaveXMFLOAT3(std::wstring const& key, DirectX::XMFLOAT3 value);
    void SaveString(std::wstring const& key, std::wstring const& string);

    bool LoadBool(std::wstring const& key, bool defaultValue);
    int  LoadInt32(std::wstring const& key, int defaultValue);
    float LoadSingle(std::wstring const& key, float defaultValue);
    DirectX::XMFLOAT3 LoadXMFLOAT3(std::wstring const& key, DirectX::XMFLOAT3 defaultValue);
    std::wstring LoadString(std::wstring const& key, std::wstring const& defaultValue);

private:
    std::wstring m_keyName;
    winrt::Windows::Foundation::Collections::IPropertySet m_settingsValues;
};
