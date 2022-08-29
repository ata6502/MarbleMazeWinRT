// TODO: Licence

#include "pch.h"

#include "MarbleMazeUI.h"
#include "UserInterface.h"

#include <dcommon.h>

using namespace MarbleMaze;

inline D2D1_RECT_F ConvertRect(winrt::Windows::Foundation::Size source)
{
    // ignore the source.X and source.Y  These are the location on the screen
    // yet we don't want to use them because all coordinates are window relative.
    return D2D1::RectF(0.0f, 0.0f, source.Width, source.Height);
}

inline void InflateRect(D2D1_RECT_F& rect, float x, float y)
{
    rect.left -= x;
    rect.right += x;
    rect.top -= y;
    rect.bottom += y;
}

MarbleMazeUI::MarbleMazeUI(std::shared_ptr<DX::DeviceResources> const& deviceResources) :
    m_deviceResources(deviceResources)
{
}

void MarbleMazeUI::CreateWindowSizeDependentResources()
{
    // user interface
    const float padding = 32.0f;
    D2D1_RECT_F clientRect = ConvertRect(m_deviceResources->GetLogicalSize());
    InflateRect(clientRect, -padding, -padding);

    D2D1_RECT_F topHalfRect = clientRect;
    topHalfRect.bottom = ((clientRect.top + clientRect.bottom) / 2.0f) - (padding / 2.0f);
    D2D1_RECT_F bottomHalfRect = clientRect;
    bottomHalfRect.top = topHalfRect.bottom + padding;

    m_startGameButton.Initialize();
    m_startGameButton.SetAlignment(AlignCenter, AlignFar);
    m_startGameButton.SetContainer(topHalfRect);
    m_startGameButton.SetText(L"Start Game");
    m_startGameButton.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_startGameButton.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_startGameButton.SetPadding(D2D1::SizeF(32.0f, 16.0f));
    m_startGameButton.GetTextStyle().SetFontSize(72.0f);
    UserInterface::GetInstance().RegisterElement(&m_startGameButton);

    m_highScoreButton.Initialize();
    m_highScoreButton.SetAlignment(AlignCenter, AlignNear);
    m_highScoreButton.SetContainer(bottomHalfRect);
    m_highScoreButton.SetText(L"High Scores");
    m_highScoreButton.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_highScoreButton.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_highScoreButton.SetPadding(D2D1::SizeF(32.0f, 16.0f));
    m_highScoreButton.GetTextStyle().SetFontSize(72.0f);
    UserInterface::GetInstance().RegisterElement(&m_highScoreButton);

    m_highScoreTable.Initialize();
    m_highScoreTable.SetAlignment(AlignCenter, AlignCenter);
    m_highScoreTable.SetContainer(clientRect);
    m_highScoreTable.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_highScoreTable.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BOLD);
    m_highScoreTable.GetTextStyle().SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    m_highScoreTable.GetTextStyle().SetFontSize(60.0f);
    UserInterface::GetInstance().RegisterElement(&m_highScoreTable);

    m_preGameCountdownTimer.Initialize();
    m_preGameCountdownTimer.SetAlignment(AlignCenter, AlignCenter);
    m_preGameCountdownTimer.SetContainer(clientRect);
    m_preGameCountdownTimer.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_preGameCountdownTimer.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_preGameCountdownTimer.GetTextStyle().SetFontSize(144.0f);
    UserInterface::GetInstance().RegisterElement(&m_preGameCountdownTimer);

    m_inGameStopwatchTimer.Initialize();
    m_inGameStopwatchTimer.SetAlignment(AlignNear, AlignFar);
    m_inGameStopwatchTimer.SetContainer(clientRect);
    m_inGameStopwatchTimer.SetTextColor(D2D1::ColorF(D2D1::ColorF::White, 0.75f));
    m_inGameStopwatchTimer.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_inGameStopwatchTimer.GetTextStyle().SetFontSize(96.0f);
    UserInterface::GetInstance().RegisterElement(&m_inGameStopwatchTimer);

    m_checkpointText.Initialize();
    m_checkpointText.SetAlignment(AlignCenter, AlignCenter);
    m_checkpointText.SetContainer(clientRect);
    m_checkpointText.SetText(L"Checkpoint!");
    m_checkpointText.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_checkpointText.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_checkpointText.GetTextStyle().SetFontSize(72.0f);
    UserInterface::GetInstance().RegisterElement(&m_checkpointText);

    m_pausedText.Initialize();
    m_pausedText.SetAlignment(AlignCenter, AlignCenter);
    m_pausedText.SetContainer(clientRect);
    m_pausedText.SetText(L"Paused");
    m_pausedText.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_pausedText.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BLACK);
    m_pausedText.GetTextStyle().SetFontSize(72.0f);
    UserInterface::GetInstance().RegisterElement(&m_pausedText);

    m_resultsText.Initialize();
    m_resultsText.SetAlignment(AlignCenter, AlignCenter);
    m_resultsText.SetContainer(clientRect);
    m_resultsText.SetTextColor(D2D1::ColorF(D2D1::ColorF::White));
    m_resultsText.GetTextStyle().SetFontWeight(DWRITE_FONT_WEIGHT_BOLD);
    m_resultsText.GetTextStyle().SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    m_resultsText.GetTextStyle().SetFontSize(36.0f);
    UserInterface::GetInstance().RegisterElement(&m_resultsText);
}

void MarbleMazeUI::HideMenu()
{
    m_startGameButton.SetVisible(false);
    m_highScoreButton.SetVisible(false);
}

void MarbleMazeUI::HideHighScoreTable()
{
    m_highScoreTable.SetVisible(false);
}

void MarbleMazeUI::HidePreGameCountdown()
{
    m_preGameCountdownTimer.SetVisible(false);
}

void MarbleMazeUI::HidePostGameInfo()
{
    m_resultsText.SetVisible(false);
}

void MarbleMazeUI::HideGameStopwatch()
{
    m_inGameStopwatchTimer.Stop();
    m_inGameStopwatchTimer.SetVisible(false);
}

void MarbleMazeUI::ResetGameStopwatch()
{
    m_inGameStopwatchTimer.Reset();
}

void MarbleMazeUI::ShowMenu()
{
    m_startGameButton.SetVisible(true);
    m_startGameButton.SetSelected(true);
    m_highScoreButton.SetVisible(true);
    m_highScoreButton.SetSelected(false);

    m_pausedText.SetVisible(false);
    m_inGameStopwatchTimer.SetVisible(false);
    m_preGameCountdownTimer.SetVisible(false);
}

void MarbleMazeUI::ShowHighScoreTable()
{
    m_highScoreTable.SetVisible(true);
}

void MarbleMazeUI::StartPreGameCountdown()
{
    m_inGameStopwatchTimer.SetVisible(false);
    m_preGameCountdownTimer.SetVisible(true);
    m_preGameCountdownTimer.StartCountdown(3);
}

void MarbleMazeUI::ShowGameStopwatch()
{
    m_inGameStopwatchTimer.SetVisible(true);
}

void MarbleMazeUI::ShowPostGameInfo()
{
    // Show post-game info.
    WCHAR formattedTime[32];
    m_inGameStopwatchTimer.GetFormattedTime(formattedTime, m_newHighScore.elapsedTime);
    WCHAR buffer[64];
    swprintf_s(
        buffer,
        L"%s\nYour time: %s",
        (m_newHighScore.wasJustAdded ? L"New High Score!" : L"Finished!"),
        formattedTime
    );
    m_resultsText.SetText(buffer);
    m_resultsText.SetVisible(true);
}

void MarbleMazeUI::ShowCheckpoint()
{
    m_checkpointText.SetVisible(true);
    m_checkpointText.SetTextOpacity(1.0f);
    m_checkpointText.FadeOut(2.0f);
}

void MarbleMazeUI::SetCurrentGameStopwatchTime(float elapsedTime)
{
    m_inGameStopwatchTimer.SetElapsedTime(elapsedTime);
}

void MarbleMazeUI::PauseGame()
{
    m_pausedText.SetVisible(true);
    m_inGameStopwatchTimer.Stop();
}

void MarbleMazeUI::ResumeGame()
{
    m_pausedText.SetVisible(false);
    m_inGameStopwatchTimer.Start();
}

void MarbleMazeUI::ResetHighScoreTable()
{
    m_highScoreTable.Reset();
}

void MarbleMazeUI::AddHighScore(HighScoreEntry entry)
{
    m_highScoreTable.AddEntry(entry);
}

void MarbleMazeUI::AddNewHighScore()
{
    m_inGameStopwatchTimer.Stop();
    m_newHighScore.elapsedTime = m_inGameStopwatchTimer.GetElapsedTime();
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);
    WCHAR buffer[64];
    swprintf_s(buffer, L"%d/%d/%d", systemTime.wYear, systemTime.wMonth, systemTime.wDay);
    m_newHighScore.tag = std::wstring(buffer);
    m_highScoreTable.AddNewEntry(m_newHighScore);
}

void MarbleMazeUI::SetPressedButton()
{
    if (m_startGameButton.GetSelected())
    {
        m_startGameButton.SetPressed(true);
    }
    if (m_highScoreButton.GetSelected())
    {
        m_highScoreButton.SetPressed(true);
    }
}

void MarbleMazeUI::ToggleSelectedButton()
{
    m_startGameButton.SetSelected(!m_startGameButton.GetSelected());
    m_highScoreButton.SetSelected(!m_startGameButton.GetSelected());
}
