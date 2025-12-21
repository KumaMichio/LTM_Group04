#ifndef WINWINDOW_H
#define WINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class NetworkClient;

class WinWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WinWindow(const QString &username, int score, NetworkClient *client, QWidget *parent = nullptr);
    ~WinWindow();

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
    int m_score;
    NetworkClient *m_client;
};

#endif // WINWINDOW_H
