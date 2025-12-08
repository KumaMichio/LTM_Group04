#ifndef WELCOMEWINDOW_H
#define WELCOMEWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class WelcomeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WelcomeWindow(const QString &username, QWidget *parent = nullptr);
    ~WelcomeWindow();

private:
    QLabel *m_welcomeLabel;
    QWidget *m_centralWidget;
    QVBoxLayout *m_layout;
};

#endif // WELCOMEWINDOW_H
