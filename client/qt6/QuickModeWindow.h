#ifndef QUICKMODEWINDOW_H
#define QUICKMODEWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QTimer>
#include "NetworkClient.h"

struct Question {
    qint64 questionId;  // Question ID for duplicate detection
    QString content;
    QString optionA;
    QString optionB;
    QString optionC;
    QString optionD;
    QString correctAnswer;  // "A", "B", "C", or "D"
};

class QuickModeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QuickModeWindow(const QString &username, NetworkClient *client, QWidget *parent = nullptr);
    ~QuickModeWindow();

private slots:
    void onAnswerClicked();
    void onLifelineClicked();
    void onTimeout();
    
    // NetworkClient signals
    void onGameStart(qint64 sessionId, int totalRounds);
    void onQuestionReceived(qint64 sessionId, int round, qint64 questionId, 
                            const QString &content, const QJsonObject &options, 
                            const QString &difficulty);
    void onAnswerResult(qint64 sessionId, int round, bool correct, 
                       const QString &correctAnswer, int score, bool gameOver);
    void onGameOver(qint64 sessionId, int finalScore, int totalRounds, 
                   const QString &status, bool win);
    void onLifelineResult(qint64 sessionId, int round, 
                         const QStringList &remainingOptions, 
                         const QStringList &removedOptions, int remaining);

private:
    void requestNextQuestion();
    void showQuestion(const Question &q);
    void checkAnswer(const QString &selectedAnswer);
    void showWinScreen();
    void showLoseScreen();
    void resetGame();
    void updateLifelineButton();
    void resetButtonStyles();
    
    QLabel *m_questionLabel;
    QLabel *m_scoreLabel;
    QPushButton *m_optionA;
    QPushButton *m_optionB;
    QPushButton *m_optionC;
    QPushButton *m_optionD;
    QPushButton *m_lifelineButton;
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_optionsLayout;
    QHBoxLayout *m_lifelineLayout;
    
    NetworkClient *m_client;
    QString m_username;
    qint64 m_sessionId;
    int m_currentRound;
    int m_score;
    int m_totalRounds;
    Question m_currentQuestionData;
    QString m_selectedAnswer;
    int m_lifelineRemaining;
    bool m_waitingForAnswer;
    QTimer *m_answerTimeoutTimer;  // Timer for answer timeout
    qint64 m_lastProcessedSessionId;  // Prevent duplicate processing
    int m_lastProcessedRound;  // Prevent duplicate processing
    bool m_gameOverHandled;  // Prevent duplicate game over screen display
};

#endif // QUICKMODEWINDOW_H
