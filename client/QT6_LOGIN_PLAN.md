# Plan: Tạo GUI Login/Logout với Qt6

## Overview
Tạo Qt6 GUI client với chức năng đăng ký, đăng nhập và đăng xuất. Client sẽ kết nối đến C server qua TCP socket và sử dụng protocol binary header + JSON payload.

## Protocol Specification

### Packet Structure
```
PacketHeader (8 bytes):
- cmd: uint16_t (network byte order)
- user_id: uint16_t (network byte order, 0 if not logged in)
- length: uint32_t (network byte order, payload size in bytes)

Payload: JSON string (variable length)
```

### Commands
- `CMD_REQ_REGISTER = 0x0101` - Đăng ký
- `CMD_RES_REGISTER = 0x0102` - Response đăng ký
- `CMD_REQ_LOGIN = 0x0103` - Đăng nhập
- `CMD_RES_LOGIN = 0x0104` - Response đăng nhập
- `CMD_REQ_LOGOUT = 0x0106` - Đăng xuất
- `CMD_RES_LOGOUT = 0x0107` - Response đăng xuất

### Request/Response Format

**Register Request:**
```json
{"username": "alice", "password": "secret123"}
```

**Register Response (Success):**
```json
{}  // Empty payload or simple OK
```

**Register Response (Error):**
```json
{"error": "USERNAME_EXISTS"}  // or "REGISTER_FAILED"
```

**Login Request:**
```json
{"username": "alice", "password": "secret123"}
```

**Login Response (Success):**
```json
{"token": "abc123..."}
```

**Login Response (Error):**
```json
{"error": "LOGIN_FAILED"}
```

**Logout Request:**
```json
{}  // Empty payload
```

**Logout Response:**
```json
{}  // Empty payload
```

## Implementation Plan

### Task 1: Setup Qt6 Project Structure
**Files to create:**
- `client/qt6/CMakeLists.txt` - CMake configuration
- `client/qt6/main.cpp` - Entry point
- `client/qt6/LoginWindow.ui` - UI form (Qt Designer)
- `client/qt6/LoginWindow.h` - Login window header
- `client/qt6/LoginWindow.cpp` - Login window implementation
- `client/qt6/NetworkClient.h` - Network client class header
- `client/qt6/NetworkClient.cpp` - Network client class implementation

**CMakeLists.txt structure:**
```cmake
cmake_minimum_required(VERSION 3.16)
project(MillionaireClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

qt6_standard_project_setup()

set(SOURCES
    main.cpp
    LoginWindow.cpp
    NetworkClient.cpp
)

set(HEADERS
    LoginWindow.h
    NetworkClient.h
)

set(UI_FORMS
    LoginWindow.ui
)

qt6_add_executable(MillionaireClient ${SOURCES} ${HEADERS} ${UI_FORMS})

target_link_libraries(MillionaireClient
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)
```

### Task 2: Create NetworkClient Class
**File**: `client/qt6/NetworkClient.h`

Create a class to handle TCP socket communication:
- Connect to server
- Send packets (header + JSON payload)
- Receive packets and parse
- Signal/slot for responses

**Key methods:**
```cpp
class NetworkClient : public QObject {
    Q_OBJECT
public:
    explicit NetworkClient(QObject *parent = nullptr);
    bool connectToServer(const QString &host, quint16 port);
    void disconnectFromServer();
    bool isConnected() const;
    
    // Send requests
    void sendRegister(const QString &username, const QString &password);
    void sendLogin(const QString &username, const QString &password);
    void sendLogout();
    
signals:
    void connected();
    void disconnected();
    void registerResponse(bool success, const QString &error);
    void loginResponse(bool success, const QString &token, const QString &error);
    void logoutResponse(bool success);
    void errorOccurred(const QString &error);
    
private slots:
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError error);
    
private:
    QTcpSocket *m_socket;
    QString m_token;
    bool m_loggedIn;
    
    // Packet handling
    void sendPacket(quint16 cmd, quint16 user_id, const QByteArray &json);
    void parsePacket(const QByteArray &data);
    QByteArray createPacketHeader(quint16 cmd, quint16 user_id, quint32 length);
};
```

### Task 3: Implement NetworkClient Packet Handling
**File**: `client/qt6/NetworkClient.cpp`

**Key implementation details:**

1. **Packet Header Creation:**
```cpp
QByteArray NetworkClient::createPacketHeader(quint16 cmd, quint16 user_id, quint32 length) {
    QByteArray header;
    QDataStream stream(&header, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);  // Network byte order
    stream << cmd << user_id << length;
    return header;
}
```

2. **Send Packet:**
```cpp
void NetworkClient::sendPacket(quint16 cmd, quint16 user_id, const QByteArray &json) {
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        emit errorOccurred("Not connected to server");
        return;
    }
    
    QByteArray header = createPacketHeader(cmd, user_id, json.size());
    m_socket->write(header);
    m_socket->write(json);
    m_socket->flush();
}
```

3. **Receive and Parse Packet:**
```cpp
void NetworkClient::onReadyRead() {
    static QByteArray buffer;
    buffer.append(m_socket->readAll());
    
    while (buffer.size() >= 8) {  // Minimum header size
        // Read header
        QDataStream stream(buffer);
        stream.setByteOrder(QDataStream::BigEndian);
        
        quint16 cmd, user_id;
        quint32 length;
        stream >> cmd >> user_id >> length;
        
        if (buffer.size() < 8 + length) {
            // Wait for more data
            return;
        }
        
        // Extract payload
        QByteArray payload = buffer.mid(8, length);
        buffer.remove(0, 8 + length);
        
        // Parse and emit signal
        parsePacket(cmd, payload);
    }
}
```

4. **Parse Response:**
```cpp
void NetworkClient::parsePacket(quint16 cmd, const QByteArray &jsonData) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);
    
    if (error.error != QJsonParseError::NoError) {
        emit errorOccurred("Invalid JSON response");
        return;
    }
    
    QJsonObject obj = doc.object();
    
    switch (cmd) {
        case 0x0102:  // CMD_RES_REGISTER
            if (obj.contains("error")) {
                emit registerResponse(false, obj["error"].toString());
            } else {
                emit registerResponse(true, "");
            }
            break;
            
        case 0x0104:  // CMD_RES_LOGIN
            if (obj.contains("error")) {
                emit loginResponse(false, "", obj["error"].toString());
            } else if (obj.contains("token")) {
                m_token = obj["token"].toString();
                m_loggedIn = true;
                emit loginResponse(true, m_token, "");
            }
            break;
            
        case 0x0107:  // CMD_RES_LOGOUT
            m_loggedIn = false;
            m_token.clear();
            emit logoutResponse(true);
            break;
    }
}
```

### Task 4: Create LoginWindow UI
**File**: `client/qt6/LoginWindow.ui`

Create UI with Qt Designer:
- **Login Tab:**
  - Username input (QLineEdit)
  - Password input (QLineEdit with password echo mode)
  - "Đăng nhập" button
  - Status label (for error messages)
  
- **Register Tab:**
  - Username input (QLineEdit)
  - Password input (QLineEdit with password echo mode)
  - Confirm password input (QLineEdit with password echo mode)
  - "Đăng ký" button
  - Status label (for error messages)

- **Server Settings:**
  - Host input (QLineEdit, default: "localhost")
  - Port input (QSpinBox, default: 8080)
  - "Kết nối" button

- **Layout:**
  - QTabWidget with Login and Register tabs
  - QVBoxLayout for each tab
  - Status bar or label at bottom

### Task 5: Implement LoginWindow Class
**File**: `client/qt6/LoginWindow.h`

```cpp
#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include "NetworkClient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QMainWindow {
    Q_OBJECT
    
public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();
    
private slots:
    void onConnectClicked();
    void onLoginClicked();
    void onRegisterClicked();
    void onRegisterResponse(bool success, const QString &error);
    void onLoginResponse(bool success, const QString &token, const QString &error);
    void onLogoutResponse(bool success);
    void onServerConnected();
    void onServerDisconnected();
    void onErrorOccurred(const QString &error);
    
private:
    Ui::LoginWindow *ui;
    NetworkClient *m_client;
    
    void setupUI();
    void updateConnectionStatus(bool connected);
    void showError(const QString &message);
    void showSuccess(const QString &message);
};
#endif
```

### Task 6: Implement LoginWindow Logic
**File**: `client/qt6/LoginWindow.cpp`

**Key implementation:**

1. **Connect to Server:**
```cpp
void LoginWindow::onConnectClicked() {
    QString host = ui->hostLineEdit->text();
    quint16 port = ui->portSpinBox->value();
    
    if (m_client->connectToServer(host, port)) {
        showSuccess("Đang kết nối...");
    } else {
        showError("Không thể kết nối đến server");
    }
}
```

2. **Login:**
```cpp
void LoginWindow::onLoginClicked() {
    QString username = ui->loginUsernameEdit->text();
    QString password = ui->loginPasswordEdit->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        showError("Vui lòng nhập đầy đủ thông tin");
        return;
    }
    
    if (!m_client->isConnected()) {
        showError("Chưa kết nối đến server");
        return;
    }
    
    m_client->sendLogin(username, password);
    showSuccess("Đang đăng nhập...");
}
```

3. **Register:**
```cpp
void LoginWindow::onRegisterClicked() {
    QString username = ui->registerUsernameEdit->text();
    QString password = ui->registerPasswordEdit->text();
    QString confirmPassword = ui->registerConfirmPasswordEdit->text();
    
    if (username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        showError("Vui lòng nhập đầy đủ thông tin");
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
    
    m_client->sendRegister(username, password);
    showSuccess("Đang đăng ký...");
}
```

4. **Handle Responses:**
```cpp
void LoginWindow::onLoginResponse(bool success, const QString &token, const QString &error) {
    if (success) {
        showSuccess("Đăng nhập thành công!");
        // TODO: Open main window or game lobby
        // For now, just show success message
    } else {
        showError("Đăng nhập thất bại: " + error);
    }
}

void LoginWindow::onRegisterResponse(bool success, const QString &error) {
    if (success) {
        showSuccess("Đăng ký thành công! Vui lòng đăng nhập.");
        // Switch to login tab
        ui->tabWidget->setCurrentIndex(0);
    } else {
        showError("Đăng ký thất bại: " + error);
    }
}
```

### Task 7: Create Main Entry Point
**File**: `client/qt6/main.cpp`

```cpp
#include <QApplication>
#include "LoginWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    LoginWindow window;
    window.show();
    
    return app.exec();
}
```

### Task 8: Add Logout Functionality
**File**: `client/qt6/LoginWindow.cpp`

Add logout button (visible when logged in):
- When logged in, show "Đăng xuất" button
- On click, call `m_client->sendLogout()`
- On logout success, reset UI to login state

### Task 9: Error Handling and Validation
**Files**: `client/qt6/LoginWindow.cpp`, `client/qt6/NetworkClient.cpp`

- Validate username length (max 32 chars)
- Validate password (min length, etc.)
- Handle network errors (connection lost, timeout)
- Show appropriate error messages in Vietnamese
- Disable buttons when not connected

### Task 10: Build Configuration
**Files**: 
- `client/qt6/CMakeLists.txt` (already in Task 1)
- `client/qt6/README.md` - Build instructions

**Build instructions:**
```bash
cd client/qt6
mkdir build
cd build
cmake ..
make
# Run: ./MillionaireClient
```

## Files Structure

```
client/qt6/
├── CMakeLists.txt
├── main.cpp
├── LoginWindow.h
├── LoginWindow.cpp
├── LoginWindow.ui
├── NetworkClient.h
├── NetworkClient.cpp
└── README.md
```

## Dependencies

- Qt6 Core
- Qt6 Widgets
- Qt6 Network
- CMake 3.16+

## Testing

1. **Test Connection:**
   - Start server
   - Connect with correct host/port
   - Verify connection status

2. **Test Register:**
   - Register new user
   - Try register duplicate username
   - Verify error messages

3. **Test Login:**
   - Login with correct credentials
   - Login with wrong credentials
   - Verify token received

4. **Test Logout:**
   - Login first
   - Logout
   - Verify state reset

## Next Steps (Future)

After login/logout works:
- Create main game window
- Implement game lobby UI
- Add friend list UI
- Add chat UI
- Implement game play UI

