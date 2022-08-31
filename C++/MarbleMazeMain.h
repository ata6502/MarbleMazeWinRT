// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#pragma once

#include "Audio.h"
#include "Camera.h"
#include "Collision.h"
#include "DeviceResources.h"
#include "LoadScreen.h"
#include "MarbleMazeUI.h"
#include "PersistentState.h"
#include "Physics.h"
#include "SampleOverlay.h"
#include "SimpleSdkMesh.h"
#include "StepTimer.h"

// Describes the constant buffer that draws the meshes.
struct ConstantBuffer
{
    DirectX::XMFLOAT4X4 model;
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;

    DirectX::XMFLOAT3 marblePosition;
    float marbleRadius;
    float lightStrength;
};

enum class GameState
{
    Initial,
    MainMenu,
    HighScoreDisplay,
    PreGameCountdown,
    InGameActive,
    InGamePaused,
    PostGameResults,
};

enum class CheckpointState
{
    None, // No checkpoint hit.
    Save, // A save-game checkpoint was hit.
    Goal, // The end of game "goal" point was hit.
};

// Renders Direct2D and 3D content on the screen.
namespace MarbleMaze
{
    class MarbleMazeMain : public winrt::implements<MarbleMazeMain, winrt::Windows::Foundation::IInspectable>, DX::IDeviceNotify
    {
    public:
        MarbleMazeMain(std::shared_ptr<DX::DeviceResources> const& deviceResources);
        ~MarbleMazeMain();

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

        void CreateWindowSizeDependentResources();
        void Run();

        winrt::Windows::Foundation::IAsyncAction LoadDeferredResourcesAsync(bool delay, bool deviceOnly);

        void OnSuspending();
        void OnResuming();

        void AddTouch(int id, winrt::Windows::Foundation::Point point);
        void UpdateTouch(int id, winrt::Windows::Foundation::Point point);
        void RemoveTouch(int id);
        void KeyDown(winrt::Windows::System::VirtualKey key);
        void KeyUp(winrt::Windows::System::VirtualKey key);
        void OnFocusChange(bool active);
        bool IsDeferredLoadReady() { return m_deferredResourcesReady; }
        void SetWindowVisibility(bool visible);
        void SetWindowClosed();
        void ReleaseUserInterfaceResources();

    private:
        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>                m_deviceResources;

        // Sample overlay class.
        std::unique_ptr<SampleOverlay>                      m_sampleOverlay;

        // Rendering loop timer.
        DX::StepTimer                                       m_timer;

        std::unique_ptr<LoadScreen>                         m_loadScreen;
        std::unique_ptr<MarbleMazeUI>                       m_ui;

        winrt::com_ptr<ID3D11InputLayout>                   m_inputLayout;
        winrt::com_ptr<ID3D11VertexShader>                  m_vertexShader;
        winrt::com_ptr<ID3D11PixelShader>                   m_pixelShader;
        winrt::com_ptr<ID3D11SamplerState>                  m_sampler;
        winrt::com_ptr<ID3D11Buffer>                        m_constantBuffer;
        winrt::com_ptr<ID3D11BlendState>                    m_blendState;
        winrt::Windows::Devices::Sensors::Accelerometer     m_accelerometer{ nullptr };

        typedef std::vector<DirectX::XMFLOAT3> Checkpoints;
        typedef std::map<int, DirectX::XMFLOAT2> TouchMap;
        typedef std::queue<D2D1_POINT_2F> PointQueue;

        std::unique_ptr<Camera>                             m_camera;
        ConstantBuffer                                      m_mazeConstantBufferData;
        SimpleSdkMesh                                             m_mazeMesh;
        ConstantBuffer                                      m_marbleConstantBufferData;
        SimpleSdkMesh                                             m_marbleMesh;
        unsigned int                                        m_vertexStride;
        float                                               m_lightStrength;
        float                                               m_targetLightStrength;
        Collision                                           m_collision;
        Physics                                             m_physics;
        Audio                                               m_audio;
        GameState                                           m_gameState;
        bool                                                m_resetCamera;
        bool                                                m_resetMarbleRotation;
        Checkpoints                                         m_checkpoints;
        size_t                                              m_currentCheckpoint;
        TouchMap                                            m_touches;
        PointQueue                                          m_pointQueue;
        bool                                                m_pauseKeyActive;
        bool                                                m_pauseKeyPressed;
        bool                                                m_homeKeyActive;
        bool                                                m_homeKeyPressed;
        bool                                                m_windowActive;
        bool                                                m_deferredResourcesReady;
        PersistentState                                     m_persistentState;

        // Input
        std::vector<winrt::Windows::Gaming::Input::Gamepad> m_myGamepads;
        winrt::Windows::Gaming::Input::Gamepad              m_gamepad{ nullptr };
        winrt::Windows::Gaming::Input::GamepadReading       m_newReading;
        winrt::Windows::Gaming::Input::GamepadReading       m_oldReading;
        bool                                                m_currentGamepadNeedsRefresh;
        const float                                         m_deadzoneRadius = 0.1f;
        const float                                         m_deadzoneSquared = m_deadzoneRadius * m_deadzoneRadius;
        const float                                         m_controllerScaleFactor = 8.0f;
        const float                                         m_touchScaleFactor = 2.0f;
        const float                                         m_accelerometerScaleFactor = 3.5f;

        bool                                                m_windowClosed;
        bool                                                m_windowVisible;

        HRESULT ExtractTrianglesFromMesh(
            SimpleSdkMesh& mesh,
            const char* meshName,
            std::vector<Triangle>& triangles
        );

        void ResetCheckpoints();
        CheckpointState UpdateCheckpoints();

        void Update();
        bool Render();
        void SetGameState(GameState nextState);
        void SaveState();
        void LoadState();

        void OnGamepadAdded(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Gaming::Input::Gamepad const& gamepad);
        void OnGamepadRemoved(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Gaming::Input::Gamepad const& gamepad);
        bool ButtonJustPressed(winrt::Windows::Gaming::Input::GamepadButtons selection);
        bool ButtonJustReleased(winrt::Windows::Gaming::Input::GamepadButtons selection);
        winrt::Windows::Gaming::Input::Gamepad GetLastGamepad();

        winrt::fire_and_forget HandleDeviceRestored();
    };
}