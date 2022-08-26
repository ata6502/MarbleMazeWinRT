// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#include "pch.h"
#include "PersistentState.h"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Storage;
using namespace DirectX;

void PersistentState::Initialize(
    _In_ winrt::Windows::Foundation::Collections::IPropertySet settingsValues,
    _In_ std::wstring const& key
    )
{
    m_settingsValues = settingsValues;
    m_keyName = key;
}

void PersistentState::SaveBool(std::wstring const& key, bool value)
{
    std::wstring fullKey = m_keyName + key;
    if (m_settingsValues.HasKey(fullKey))
    {
        m_settingsValues.Remove(fullKey);
    }
    m_settingsValues.Insert(fullKey, winrt::box_value(value));
}

void PersistentState::SaveInt32(std::wstring const& key, int value)
{
    std::wstring fullKey = m_keyName + key;
    if (m_settingsValues.HasKey(fullKey))
    {
        m_settingsValues.Remove(fullKey);
    }
    m_settingsValues.Insert(fullKey, PropertyValue::CreateInt32(value));
}

void PersistentState::SaveSingle(std::wstring const& key, float value)
{
    std::wstring fullKey = m_keyName + key;
    if (m_settingsValues.HasKey(fullKey))
    {
        m_settingsValues.Remove(fullKey);
    }
    m_settingsValues.Insert(fullKey, PropertyValue::CreateSingle(value));
}

void PersistentState::SaveXMFLOAT3(std::wstring const& key, DirectX::XMFLOAT3 value)
{
    std::wstring fullKey = m_keyName + key;
    if (m_settingsValues.HasKey(fullKey))
    {
        m_settingsValues.Remove(fullKey);
    }
    m_settingsValues.Insert(fullKey, PropertyValue::CreateSingleArray({ value.x, value.y, value.z }));
}

void PersistentState::SaveString(std::wstring const& key, std::wstring const& string)
{
    std::wstring fullKey = m_keyName + key;
    if (m_settingsValues.HasKey(fullKey))
    {
        m_settingsValues.Remove(fullKey);
    }
    m_settingsValues.Insert(fullKey, PropertyValue::CreateString(string));
}

bool PersistentState::LoadBool(std::wstring const& key, bool defaultValue)
{
    std::wstring fullKey = m_keyName + key;
    if (m_settingsValues.HasKey(fullKey))
    {
        return winrt::unbox_value_or(m_settingsValues.TryLookup(fullKey), defaultValue);
    }
    return defaultValue;
}

int PersistentState::LoadInt32(std::wstring const& key, int defaultValue)
{
    std::wstring fullKey = m_keyName + key;
    if (m_settingsValues.HasKey(fullKey))
    {
        return winrt::unbox_value_or(m_settingsValues.TryLookup(fullKey), defaultValue);
    }
    return defaultValue;
}

float PersistentState::LoadSingle(std::wstring const& key, float defaultValue)
{
    std::wstring fullKey = m_keyName + key;
    if (m_settingsValues.HasKey(fullKey))
    {
        return winrt::unbox_value_or(m_settingsValues.TryLookup(m_keyName + key), defaultValue);
    }
    return defaultValue;
}

XMFLOAT3 PersistentState::LoadXMFLOAT3(std::wstring const& key, DirectX::XMFLOAT3 defaultValue)
{
    std::wstring fullKey = m_keyName + key;
    auto propertyValue = winrt::unbox_value_or(m_settingsValues.TryLookup(fullKey), winrt::Windows::Foundation::IPropertyValue{});
    if (m_settingsValues.HasKey(fullKey))
    {
        winrt::com_array<float> array3;
        propertyValue.GetSingleArray(array3);
        return { array3[0], array3[1], array3[2] };
    }
    return defaultValue;
}

std::wstring PersistentState::LoadString(std::wstring const& key, std::wstring const& defaultValue)
{
    std::wstring fullKey = m_keyName + key;
    if (m_settingsValues.HasKey(fullKey))
    {
        return winrt::unbox_value_or<winrt::hstring>(m_settingsValues.TryLookup(m_keyName + key), defaultValue).c_str();
    }
    return defaultValue;
}
