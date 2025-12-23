#ifndef ONEVNWINDOW_H
#define ONEVNWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QTimer>
#include <QListWidget>
#include <QStackedWidget>
#include <QSpinBox>
#include <QLineEdit>
#include "NetworkClient.h"

// Forward declaration
struct OneVNQuestion {
    int round;
    int totalRounds;
    QString difficulty;
    qint64 questionId;
    QString content;
    QString optionA;
    QString optionB;
    QString optionC;
    QString optionD;
};

struct PlayerInfo {
    qint64 userId;
    QString username;
    int score;
    bool eliminated;
    int rank;
};

class OneVNWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit OneVNWindow(const QString &username, NetworkClient *client, QWidget *parent = nullptr);
    ~OneVNWindow();

private slots:
    // Room management
    void onCreateRoomClicked();
    void onJoinRoomClicked();
    void onStartGameClicked();
    void onLeaveRoomClicked();
    
    // Game actions
    void onAnswerClicked();
    void onQuestionTimeout();
    void updateTimer();
    
    // NetworkClient signals - Room
    void onRoomCreated(qint64 roomId);
    void onRoomJoined(bool success, const QString &error);
    void onRoomUpdate(const QJsonArray &members);
    
    // NetworkClient signals - Game
    void onGameStart1VN(qint64 sessionId, qint64 roomId, int totalRounds);
    void onQuestion1VNReceived(int round, int totalRounds, const QString &difficulty,
                                qint64 questionId, const QString &content,
                                const QJsonObject &options, int timeLimit);
    void onAnswerResult1VN(bool correct, int score, int totalScore, bool eliminated);
    void onElimination(qint64 userId, int round);
    void onGameOver1VN(qint64 winnerId, const QJsonArray &leaderboard);

private:
    enum ScreenState {
        ROOM_SELECTION,      // Tạo phòng hoặc tham gia phòng
        WAITING_ROOM,        // Đang chờ trong phòng
        GAME_PLAYING,        // Đang chơi game
        GAME_OVER            // Game kết thúc
    };
    
    void showScreen(ScreenState state);
    void setupRoomSelectionScreen();
    void setupWaitingRoomScreen();
    void setupGameScreen();
    void setupGameOverScreen();
    
    void updateLeaderboard(const QJsonArray &leaderboard);
    void resetButtonStyles();
    void disableAllButtons();
    void enableAllButtons();
    
    // UI Components - Screens
    QStackedWidget *m_stackedWidget;
    QWidget *m_roomSelectionWidget;
    QWidget *m_waitingRoomWidget;
    QWidget *m_gameWidget;
    QWidget *m_gameOverWidget;
    
    // Room Selection Screen
    QPushButton *m_createRoomButton;
    QPushButton *m_joinRoomButton;
    QSpinBox *m_easyCountSpin;
    QSpinBox *m_mediumCountSpin;
    QSpinBox *m_hardCountSpin;
    QLineEdit *m_roomIdInput;
    
    // Waiting Room Screen
    QLabel *m_waitingRoomTitle;
    QLabel *m_roomIdLabel;
    QListWidget *m_membersList;
    QPushButton *m_startGameButton;
    QPushButton *m_leaveRoomButton;
    QLabel *m_waitingStatusLabel;
    
    // Game Screen
    QLabel *m_gameRoundLabel;
    QLabel *m_gameQuestionLabel;
    QLabel *m_gameTimerLabel;
    QPushButton *m_gameOptionA;
    QPushButton *m_gameOptionB;
    QPushButton *m_gameOptionC;
    QPushButton *m_gameOptionD;
    QListWidget *m_gameLeaderboardList;
    
    // Game Over Screen
    QLabel *m_winnerLabel;
    QListWidget *m_finalLeaderboardList;
    QPushButton *m_backToMenuButton;
    
    // Data
    NetworkClient *m_client;
    QString m_username;
    qint64 m_userId;
    qint64 m_roomId;
    qint64 m_sessionId;
    bool m_isOwner;
    ScreenState m_currentState;
    
    // Game state
    OneVNQuestion m_currentQuestion;
    int m_currentRound;
    int m_totalRounds;
    int m_myScore;
    bool m_eliminated;
    bool m_waitingForAnswer;
    QTimer *m_questionTimer;  // Timer for each question (15s)
    QTimer *m_timerUpdateTimer;  // Timer to update UI every second
    int m_timeRemaining;  // Seconds remaining
    QString m_selectedAnswer;
    
    // Players data
    QList<PlayerInfo> m_players;
};

#endif // ONEVNWINDOW_H

