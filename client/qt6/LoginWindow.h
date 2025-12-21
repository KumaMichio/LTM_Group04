#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include "NetworkClient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class GameModeSelectionWindow;  // Forward declaration

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();
    NetworkClient* getNetworkClient() const;

private slots:
    void onConnectClicked();
    void onLoginClicked();
    void onRegisterClicked();
    void onLogoutClicked();
    
    // NetworkClient signals
    void onRegisterResponse(bool success, const QString &error);
    void onLoginResponse(bool success, const QString &token, const QString &error);
    void onLogoutResponse(bool success);
    void onServerConnected();
    void onServerDisconnected();
    void onErrorOccurred(const QString &error);

private:
    Ui::LoginWindow *ui;
    NetworkClient *m_client;
    GameModeSelectionWindow *m_gameModeWindow;

    void setupUI();
    void updateConnectionStatus(bool connected);
    void showError(const QString &message);
    void showSuccess(const QString &message);
    void clearStatus();
    void updateLoginState(bool loggedIn);
};

#endif // LOGINWINDOW_H

