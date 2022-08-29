
#include "DeviceResources.h"

// UI
#include "CountdownTimer.h"
#include "HighScoreTable.h"
#include "StopwatchTimer.h"
#include "TextButton.h"
#include "TextElement.h"

namespace MarbleMaze
{
    class MarbleMazeUI
    {
    public:
        MarbleMazeUI(std::shared_ptr<DX::DeviceResources> const& deviceResources);
        void CreateWindowSizeDependentResources();

        // Pre-game stopwatch timer
        void StartPreGameCountdown();
        void HidePreGameCountdown();
        bool IsCountdownComplete() const { return m_preGameCountdownTimer.IsCountdownComplete(); }

        // In-game stopwatch timer
        void ResetGameStopwatch();
        void ShowGameStopwatch();
        float GetCurrentStopwatchTime() const { return m_inGameStopwatchTimer.GetElapsedTime(); }
        void SetCurrentGameStopwatchTime(float elapsedTime);
        void HideGameStopwatch();

        // High score table
        void HideHighScoreTable();
        void ShowHighScoreTable();
        void ResetHighScoreTable();
        void AddHighScore(HighScoreEntry entry);
        void AddNewHighScore();
        HighScoreEntries GetHighScoreEntries() { return m_highScoreTable.GetEntries(); }

        // Menu and menu buttons
        void ShowMenu();
        void HideMenu();
        void SetPressedButton();
        void ToggleSelectedButton();
        bool IsStartButtonPressed() const { return m_startGameButton.IsPressed(); }
        void SetStartButtonPressed(bool pressed) { m_startGameButton.SetPressed(pressed); }
        bool IsHighScoreButtonPressed() const { return m_highScoreButton.IsPressed(); }
        void SetHighScoreButtonPressed(bool pressed) { m_highScoreButton.SetPressed(pressed); }

        // "Paused" text and pause/resume game
        void PauseGame();
        void ResumeGame();
        bool IsPausedPressed() const { return m_pausedText.IsPressed(); }
        void SetPausedPressed(bool pressed) { m_pausedText.SetPressed(pressed); }

        // Checkpoints
        void ShowCheckpoint();

        // Post-game results (info)
        void ShowPostGameInfo();
        void HidePostGameInfo();

    private:
        std::shared_ptr<DX::DeviceResources>    m_deviceResources;
        TextButton                              m_startGameButton;
        TextButton                              m_highScoreButton;
        HighScoreTable                          m_highScoreTable;
        CountdownTimer                          m_preGameCountdownTimer;
        StopwatchTimer                          m_inGameStopwatchTimer;
        TextElement                             m_checkpointText;
        TextButton                              m_pausedText;
        TextElement                             m_resultsText;
        HighScoreEntry                          m_newHighScore;
    };
}