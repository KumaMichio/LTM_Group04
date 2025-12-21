#ifndef LOSEWINDOW_H
#define LOSEWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class LoseWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoseWindow(const QString &username, int questionsAnswered, QWidget *parent = nullptr);
    ~LoseWindow();

private slots:
    void onPlayAgainClicked();
    void onBackToMenuClicked();

private:
    QLabel *m_titleLabel;
    QLabel *m_messageLabel;
    QPushButton *m_playAgainButton;
    QPushButton *m_backToMenuButton;
    QWidget *m_centralWidget;
    QVBoxLayout *m_layout;
    QString m_username;
    int m_questionsAnswered;
};

#endif // LOSEWINDOW_H
