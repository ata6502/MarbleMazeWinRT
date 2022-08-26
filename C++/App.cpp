// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
// Converted to WinRT/C++ by ata6502

#include "pch.h"
#include "DeviceResources.h"
#include "MarbleMazeMain.h"
#include "UserInterface.h"

#include <dxgidebug.h>

using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::Graphics::Display;
using namespace winrt::Windows::System;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Input;

// The main entry point for our app. Connects the app with the Windows shell and handles application lifecycle events.
struct App : winrt::implements<App, IFrameworkViewSource, IFrameworkView>
{
    IFrameworkView CreateView()
    {
        return *this;
    }

    // The first method called when the IFrameworkView is being created.
    void Initialize(CoreApplicationView const& applicationView)
    {
        // Register event handlers for app lifecycle. This example includes Activated, so that we
        // can make the CoreWindow active and start rendering on the window.
        applicationView.Activated({ this, &App::OnActivated });

        CoreApplication::Suspending({ this, &App::OnSuspending });
        CoreApplication::Resuming({ this, &App::OnResuming });

        // At this point we have access to the device.
        // We can create the device-dependent resources.
        m_deviceResources = std::make_shared<DX::DeviceResources>();
    }

    // Called when the CoreWindow object is created (or re-created).
    void SetWindow(CoreWindow const& window)
    {
        m_deviceResources->SetWindow(window);

        window.PointerCursor(CoreCursor(CoreCursorType::Arrow, 0));

        // Disable all pointer visual feedback for better performance when touching.
        PointerVisualizationSettings visualizationSettings{ PointerVisualizationSettings::GetForCurrentView() };
        visualizationSettings.IsContactFeedbackEnabled(false);
        visualizationSettings.IsBarrelButtonFeedbackEnabled(false);

        // Window event handlers.
        window.Activated({ this, &App::OnWindowActivationChanged });
        window.SizeChanged({ this, &App::OnWindowSizeChanged });
        window.VisibilityChanged({ this, &App::OnVisibilityChanged });
        window.Closed({ this, &App::OnWindowClosed });

        // Pointer event handlers.
        window.PointerPressed({ this, &App::OnPointerPressed });
        window.PointerReleased({ this, &App::OnPointerReleased });
        window.PointerMoved({ this, &App::OnPointerMoved });

        // Keyboard event handlers.
        window.KeyDown({ this, &App::OnKeyDown });
        window.KeyUp({ this, &App::OnKeyUp });

        // Display information event handlers.
        DisplayInformation currentDisplayInformation{ DisplayInformation::GetForCurrentView() };
        currentDisplayInformation.DpiChanged({ this, &App::OnDpiChanged });
        currentDisplayInformation.OrientationChanged({ this, &App::OnOrientationChanged });
        DisplayInformation::DisplayContentsInvalidated({ this, &App::OnDisplayContentsInvalidated });
    }

    // Initializes scene resources or loads a previously saved app state.
    winrt::fire_and_forget Load([[maybe_unused]] winrt::hstring const& entryPoint)
    {
        auto lifetime = get_strong();

        if (!m_main)
        {
            m_main = winrt::make_self<MarbleMaze::MarbleMazeMain>(m_deviceResources);
        }

        // IsDeferredLoadReady checks to see if resources have already been loaded.
        // If they have been loaded then the deferred load is skipped.
        // This is more efficient, and it avoids a potential race condition where the application
        // starts rendering after resume, but the resources are being reloaded at the same time 
        // which could cause random crashes.

        if (!m_main->IsDeferredLoadReady())
        {
            co_await m_main->LoadDeferredResourcesAsync(true, false);
        }
    }

    // Called when the IFrameworkView class is torn down while the app is in the foreground.
    void Uninitialize()
    {
    }

    // Called after the window becomes active.
    void Run()
    {
        m_main->Run();

#ifdef _DEBUG
        // Dump debug info when exiting.
        DumpD3DDebug();
#endif //_DEGBUG
    }

    #pragma region Application lifecycle event handlers

    void OnActivated([[maybe_unused]] CoreApplicationView const& applicationView, [[maybe_unused]] IActivatedEventArgs const& args)
    {
        // Run() won't start until the CoreWindow is activated.
        CoreWindow::GetForCurrentThread().Activate();
        m_main->SetWindowVisibility(true);
    }

    winrt::fire_and_forget OnSuspending([[maybe_unused]] IInspectable const& sender, [[maybe_unused]] SuspendingEventArgs const& args)
    {
        auto lifetime = get_strong();

        // Save app state asynchronously after requesting a deferral. Holding a deferral
        // indicates that the application is busy performing suspending operations. Be
        // aware that a deferral may not be held indefinitely. After about five seconds,
        // the app will be forced to exit.
        SuspendingDeferral deferral = args.SuspendingOperation().GetDeferral();

        co_await winrt::resume_background();

        m_deviceResources->Trim();
        m_main->OnSuspending();

        deferral.Complete();
    }

    void OnResuming([[maybe_unused]] IInspectable const& sender, [[maybe_unused]] IInspectable const& args)
    {
        // Restore any data or state that was unloaded on suspend. By default, data
        // and state are persisted when resuming from suspend. Note that this event
        // does not occur if the app was previously terminated.
        m_main->OnResuming();
    }
    #pragma endregion

    #pragma region Window event handlers

    void OnWindowActivationChanged([[maybe_unused]] CoreWindow const& sender, WindowActivatedEventArgs const& args)
    {
        if (args.WindowActivationState() == CoreWindowActivationState::Deactivated)
        {
            m_main->OnFocusChange(false);
        }
        else if (args.WindowActivationState() == CoreWindowActivationState::CodeActivated
            || args.WindowActivationState() == CoreWindowActivationState::PointerActivated)
        {
            m_main->OnFocusChange(true);
        }
    }

    void OnWindowSizeChanged([[maybe_unused]] CoreWindow const& window, WindowSizeChangedEventArgs const& args)
    {
        m_deviceResources->SetLogicalSize(args.Size());
        m_main->CreateWindowSizeDependentResources();
    }

    void OnVisibilityChanged([[maybe_unused]] CoreWindow const& sender, VisibilityChangedEventArgs const& args)
    {
        m_main->SetWindowVisibility(args.Visible());
    }

    void OnWindowClosed([[maybe_unused]] CoreWindow const& sender, [[maybe_unused]] CoreWindowEventArgs const& args)
    {
        m_main->SetWindowClosed();
    }
    #pragma endregion

    #pragma region Pointer event handlers

    void OnPointerPressed([[maybe_unused]] CoreWindow const& sender, PointerEventArgs const& args)
    {
        m_main->AddTouch(args.CurrentPoint().PointerId(), args.CurrentPoint().Position());
    }

    void OnPointerReleased([[maybe_unused]] CoreWindow const& sender, PointerEventArgs const& args)
    {
        m_main->RemoveTouch(args.CurrentPoint().PointerId());
    }

    void OnPointerMoved([[maybe_unused]] CoreWindow const& sender, PointerEventArgs const& args)
    {
        m_main->UpdateTouch(args.CurrentPoint().PointerId(), args.CurrentPoint().Position());
    }
    #pragma endregion

    #pragma region Keyboard event handlers

    void OnKeyDown([[maybe_unused]] CoreWindow const& sender, KeyEventArgs const& args)
    {
        m_main->KeyDown(args.VirtualKey());
    }

    void OnKeyUp([[maybe_unused]] CoreWindow const& sender, KeyEventArgs const& args)
    {
        m_main->KeyUp(args.VirtualKey());

#ifdef _DEBUG
        // Pressing F4 cause the app to exit, so that DumpD3DDebug method gets called on exit.
        if (args.VirtualKey() == VirtualKey::F4)
        {
            m_main->SetWindowClosed();
        }
#endif // _DEBUG
    }
    #pragma endregion

    #pragma region Display information event handlers
 
    void OnDpiChanged(DisplayInformation const& sender, [[maybe_unused]] IInspectable const& args)
    {
        m_deviceResources->SetDpi(sender.LogicalDpi());
        m_main->CreateWindowSizeDependentResources();
    }

    void OnOrientationChanged(DisplayInformation const& sender, [[maybe_unused]] IInspectable const& args)
    {
        m_deviceResources->SetCurrentOrientation(sender.CurrentOrientation());
        m_main->CreateWindowSizeDependentResources();
    }

    void OnDisplayContentsInvalidated([[maybe_unused]] DisplayInformation const& sender, [[maybe_unused]] IInspectable const& args)
    {
        m_deviceResources->ValidateDevice();
    }
    #pragma endregion

private:
    std::shared_ptr<DX::DeviceResources> m_deviceResources;
    winrt::com_ptr<MarbleMaze::MarbleMazeMain> m_main;

#ifdef _DEBUG
    // App method that deletes all D3D resources, dumps debug info.
    void DumpD3DDebug();
#endif
};

#ifdef _DEBUG
// App method that deletes all D3D resources, dumps debug info.
void App::DumpD3DDebug()
{
    // This is debug code to free up all allocated D3D resources, and generate debug output for leaked objects.
    // The way this works is that D3D keeps track of all objects. When we destroy the m_main and m_deviceResources classes, 
    // all D3D objects should be released. If any objects that D3D thinks are not released are being leaked and the code should be examined.

    // Get the debug interface for the device.
    winrt::com_ptr<IDXGIDebug1> debugInterface = nullptr;
    winrt::check_hresult(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debugInterface)));

    // Free up all D3D resources.
    m_deviceResources->GetD3DDeviceContext()->ClearState();
    m_deviceResources->GetD3DDeviceContext()->Flush();

    UserInterface::GetInstance().Release();
    m_main = nullptr;
    m_deviceResources = nullptr;

    // Dump the list of leaked objects to the debugger output window.
    winrt::check_hresult(debugInterface->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL));
}
#endif

// The main function is only used to initialize our IFrameworkView class.
int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    CoreApplication::Run(winrt::make<App>());
}