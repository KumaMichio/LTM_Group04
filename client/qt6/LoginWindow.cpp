#include "LoginWindow.h"
#include "GameModeSelectionWindow.h"
#include "ui_LoginWindow.h"
#include <QMessageBox>
#include <QDebug>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
    , m_client(new NetworkClient(this))
    , m_gameModeWindow(nullptr)
{
    ui->setupUi(this);
    setupUI();

    // Connect NetworkClient signals
    connect(m_client, &NetworkClient::connected, this, &LoginWindow::onServerConnected);
    connect(m_client, &NetworkClient::disconnected, this, &LoginWindow::onServerDisconnected);
    connect(m_client, &NetworkClient::registerResponse, this, &LoginWindow::onRegisterResponse);
    connect(m_client, &NetworkClient::loginResponse, this, &LoginWindow::onLoginResponse);
    connect(m_client, &NetworkClient::logoutResponse, this, &LoginWindow::onLogoutResponse);
    connect(m_client, &NetworkClient::errorOccurred, this, &LoginWindow::onErrorOccurred);

    // Connect UI signals
    connect(ui->connectButton, &QPushButton::clicked, this, &LoginWindow::onConnectClicked);
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(ui->registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &LoginWindow::onLogoutClicked);

    // Initial state
    updateConnectionStatus(false);
    updateLoginState(false);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

NetworkClient* LoginWindow::getNetworkClient() const
{
    return m_client;
}

void LoginWindow::setupUI()
{
    // Set default values
    ui->hostLineEdit->setText("localhost");
    ui->portSpinBox->setRange(1, 65535);
    ui->portSpinBox->setValue(9000);  // Default server port

    // Set password echo mode
    ui->loginPasswordEdit->setEchoMode(QLineEdit::Password);
    ui->registerPasswordEdit->setEchoMode(QLineEdit::Password);
    ui->registerConfirmPasswordEdit->setEchoMode(QLineEdit::Password);

    // Set placeholder texts
    ui->loginUsernameEdit->setPlaceholderText("Nhập tên đăng nhập");
    ui->loginPasswordEdit->setPlaceholderText("Nhập mật khẩu");
    ui->registerUsernameEdit->setPlaceholderText("Nhập tên đăng nhập");
    ui->registerPasswordEdit->setPlaceholderText("Nhập mật khẩu");
    ui->registerConfirmPasswordEdit->setPlaceholderText("Xác nhận mật khẩu");
    
    // Apply beautiful styles
    setStyleSheet(
        "QMainWindow {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "}"
        "QGroupBox {"
        "    font-weight: bold;"
        "    font-size: 12pt;"
        "    color: white;"
        "    border: 2px solid rgba(255,255,255,0.3);"
        "    border-radius: 10px;"
        "    margin-top: 10px;"
        "    padding-top: 15px;"
        "    background-color: rgba(255,255,255,0.1);"
        "}"
        "QLineEdit, QSpinBox {"
        "    background-color: white;"
        "    border: 2px solid #ddd;"
        "    border-radius: 8px;"
        "    padding: 8px;"
        "    font-size: 11pt;"
        "    selection-background-color: #667eea;"
        "}"
        "QLineEdit:focus, QSpinBox:focus {"
        "    border: 2px solid #667eea;"
        "    background-color: #f8f9ff;"
        "}"
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 12px 24px;"
        "    font-size: 12pt;"
        "    font-weight: bold;"
        "    min-height: 40px;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #7c8ef0, stop:1 #8a5fb8);"
        "    transform: scale(1.05);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #5568d0, stop:1 #6a3a92);"
        "}"
        "QPushButton:disabled {"
        "    background-color: #cccccc;"
        "    color: #666666;"
        "}"
        "QLabel {"
        "    color: white;"
        "    font-size: 11pt;"
        "}"
        "QTabWidget::pane {"
        "    border: 2px solid rgba(255,255,255,0.3);"
        "    border-radius: 10px;"
        "    background-color: rgba(255,255,255,0.95);"
        "    padding: 10px;"
        "}"
        "QTabBar::tab {"
        "    background-color: rgba(255,255,255,0.2);"
        "    color: white;"
        "    border: none;"
        "    border-top-left-radius: 8px;"
        "    border-top-right-radius: 8px;"
        "    padding: 10px 20px;"
        "    font-size: 11pt;"
        "    font-weight: bold;"
        "    margin-right: 2px;"
        "}"
        "QTabBar::tab:selected {"
        "    background-color: rgba(255,255,255,0.95);"
        "    color: #667eea;"
        "}"
        "QTabBar::tab:hover:!selected {"
        "    background-color: rgba(255,255,255,0.4);"
        "}"
    );
}

void LoginWindow::onConnectClicked()
{
    QString host = ui->hostLineEdit->text().trimmed();
    quint16 port = static_cast<quint16>(ui->portSpinBox->value());

    if (host.isEmpty()) {
        showError("Vui lòng nhập địa chỉ server");
        return;
    }

    showSuccess("Đang kết nối...");
    m_client->connectToServer(host, port);
}

void LoginWindow::onLoginClicked()
{
    QString username = ui->loginUsernameEdit->text().trimmed();
    QString password = ui->loginPasswordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        showError("Vui lòng nhập đầy đủ thông tin");
        return;
    }

    if (username.length() > 32) {
        showError("Tên đăng nhập không được quá 32 ký tự");
        return;
    }

    if (!m_client->isConnected()) {
        showError("Chưa kết nối đến server");
        return;
    }

    clearStatus();
    m_client->sendLogin(username, password);
}

void LoginWindow::onRegisterClicked()
{
    QString username = ui->registerUsernameEdit->text().trimmed();
    QString password = ui->registerPasswordEdit->text();
    QString confirmPassword = ui->registerConfirmPasswordEdit->text();

    if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        showError("Vui lòng nhập đầy đủ thông tin");
        return;
    }

    if (username.length() > 32) {
        showError("Tên đăng nhập không được quá 32 ký tự");
        return;
    }

    if (password.length() < 3) {
        showError("Mật khẩu phải có ít nhất 3 ký tự");
        return;
    }

    if (password != confirmPassword) {
        showError("Mật khẩu xác nhận không khớp");
        return;
    }

    if (!m_client->isConnected()) {
        showError("Chưa kết nối đến server");
        return;
    }

    clearStatus();
    m_client->sendRegister(username, password);
}

void LoginWindow::onLogoutClicked()
{
    if (!m_client->isLoggedIn()) {
        showError("Bạn chưa đăng nhập");
        return;
    }

    m_client->sendLogout();
}

void LoginWindow::onRegisterResponse(bool success, const QString &error)
{
    if (success) {
        showSuccess("Đăng ký thành công! Vui lòng đăng nhập.");
        // Switch to login tab
        ui->tabWidget->setCurrentIndex(0);
        // Clear register fields
        ui->registerUsernameEdit->clear();
        ui->registerPasswordEdit->clear();
        ui->registerConfirmPasswordEdit->clear();
    } else {
        QString errorMsg = "Đăng ký thất bại";
        if (!error.isEmpty()) {
            if (error == "USERNAME_EXISTS") {
                errorMsg = "Tên đăng nhập đã tồn tại";
            } else if (error == "REGISTER_FAILED") {
                errorMsg = "Lỗi đăng ký. Vui lòng thử lại.";
            } else {
                errorMsg = "Lỗi: " + error;
            }
        }
        showError(errorMsg);
    }
}

void LoginWindow::onLoginResponse(bool success, const QString &token, const QString &error)
{
    if (success) {
        showSuccess("Đăng nhập thành công!");
        updateLoginState(true);
        // Clear password fields
        ui->loginPasswordEdit->clear();
        
        // Get username for game mode selection window
        QString username = ui->loginUsernameEdit->text().trimmed();
        
        // Open game mode selection window
        if (m_gameModeWindow) {
            m_gameModeWindow->close();
            delete m_gameModeWindow;
        }
        m_gameModeWindow = new GameModeSelectionWindow(username, this);
        m_gameModeWindow->show();
        
        // Hide login window
        this->hide();
    } else {
        QString errorMsg = "Đăng nhập thất bại";
        if (!error.isEmpty()) {
            if (error == "LOGIN_FAILED") {
                errorMsg = "Sai tên đăng nhập hoặc mật khẩu";
            } else {
                errorMsg = "Lỗi: " + error;
            }
        }
        showError(errorMsg);
        updateLoginState(false);
    }
}

void LoginWindow::onLogoutResponse(bool success)
{
    if (success) {
        showSuccess("Đăng xuất thành công!");
        updateLoginState(false);
        ui->loginUsernameEdit->clear();
        ui->loginPasswordEdit->clear();
        
        // Close game mode window if open
        if (m_gameModeWindow) {
            m_gameModeWindow->close();
            delete m_gameModeWindow;
            m_gameModeWindow = nullptr;
        }
    } else {
        showError("Đăng xuất thất bại");
    }
}

void LoginWindow::onServerConnected()
{
    showSuccess("Đã kết nối đến server");
    updateConnectionStatus(true);
    // Enable login/register buttons when connected
    updateLoginState(false);  // false = not logged in yet, but connected
}

void LoginWindow::onServerDisconnected()
{
    showError("Đã ngắt kết nối với server");
    updateConnectionStatus(false);
    updateLoginState(false);  // Disable buttons when disconnected
}

void LoginWindow::onErrorOccurred(const QString &error)
{
    showError(error);
}

void LoginWindow::updateConnectionStatus(bool connected)
{
    if (connected) {
        ui->connectionStatusLabel->setText("Trạng thái: Đã kết nối");
        ui->connectionStatusLabel->setStyleSheet("color: green;");
        ui->connectButton->setText("Ngắt kết nối");
    } else {
        ui->connectionStatusLabel->setText("Trạng thái: Chưa kết nối");
        ui->connectionStatusLabel->setStyleSheet("color: red;");
        ui->connectButton->setText("Kết nối");
    }
}

void LoginWindow::updateLoginState(bool loggedIn)
{
    ui->loginButton->setEnabled(!loggedIn && m_client->isConnected());
    ui->registerButton->setEnabled(!loggedIn && m_client->isConnected());
    ui->logoutButton->setEnabled(loggedIn);
    ui->loginUsernameEdit->setEnabled(!loggedIn);
    ui->loginPasswordEdit->setEnabled(!loggedIn);
    ui->registerUsernameEdit->setEnabled(!loggedIn);
    ui->registerPasswordEdit->setEnabled(!loggedIn);
    ui->registerConfirmPasswordEdit->setEnabled(!loggedIn);
}

void LoginWindow::showError(const QString &message)
{
    ui->statusLabel->setText(message);
    ui->statusLabel->setStyleSheet("color: red;");
}

void LoginWindow::showSuccess(const QString &message)
{
    ui->statusLabel->setText(message);
    ui->statusLabel->setStyleSheet("color: green;");
}

void LoginWindow::clearStatus()
{
    ui->statusLabel->clear();
}

