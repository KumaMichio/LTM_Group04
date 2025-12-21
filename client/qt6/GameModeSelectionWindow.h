#ifndef GAMEMODESELECTIONWINDOW_H
#define GAMEMODESELECTIONWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class GameModeSelectionWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameModeSelectionWindow(const QString &username, QWidget *parent = nullptr);
    ~GameModeSelectionWindow();

signals:
    void quickModeSelected();
    void oneVNModeSelected();

private slots:
    void onQuickModeClicked();
    void onOneVNModeClicked();

private:
    QLabel *m_titleLabel;
    QLabel *m_welcomeLabel;
    QPushButton *m_quickModeButton;
    QPushButton *m_oneVNButton;
    QWidget *m_centralWidget;
    QVBoxLayout *m_layout;
    QString m_username;
};

#endif // GAMEMODESELECTIONWINDOW_H
