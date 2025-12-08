#ifndef QUICKMODEWINDOW_H
#define QUICKMODEWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QTimer>

struct Question {
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
    explicit QuickModeWindow(const QString &username, QWidget *parent = nullptr);
    ~QuickModeWindow();

private slots:
    void onAnswerClicked();
    void onTimeout();

private:
    void loadQuestion();
    void showQuestion(const Question &q);
    void checkAnswer(const QString &selectedAnswer);
    void showWinScreen();
    void showLoseScreen();
    void resetGame();
    
    QLabel *m_questionLabel;
    QLabel *m_scoreLabel;
    QPushButton *m_optionA;
    QPushButton *m_optionB;
    QPushButton *m_optionC;
    QPushButton *m_optionD;
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_optionsLayout;
    
    QString m_username;
    int m_currentQuestion;
    int m_score;
    Question m_currentQuestionData;
    QString m_selectedAnswer;
    
    // Sample questions (temporary - should be loaded from server)
    QList<Question> m_questions;
    void initializeQuestions();
};

#endif // QUICKMODEWINDOW_H
