#include "NetworkClient.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>

// Command definitions (matching server)
#define CMD_REQ_REGISTER    0x0101
#define CMD_RES_REGISTER    0x0102
#define CMD_REQ_LOGIN       0x0103
#define CMD_RES_LOGIN       0x0104
#define CMD_REQ_LOGOUT      0x0106
#define CMD_RES_LOGOUT      0x0107

// QuickMode commands
#define CMD_REQ_START_QUICKMODE    0x0500
#define CMD_NOTIFY_GAME_START      0x0501
#define CMD_NOTIFY_QUESTION        0x0502
#define CMD_REQ_SUBMIT_ANSWER      0x0503
#define CMD_RES_SUBMIT_ANSWER      0x0504
#define CMD_NOTIFY_ANSWER_RESULT   0x0505
#define CMD_REQ_USE_LIFELINE       0x0506
#define CMD_RES_USE_LIFELINE       0x0507
#define CMD_NOTIFY_GAME_OVER       0x0509
#define CMD_REQ_GET_QUESTION       0x050A

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_loggedIn(false)
    , m_userId(0)
    , m_lastQuestionSessionId(0)
    , m_lastQuestionRound(0)
    , m_lastQuestionId(0)
{
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkClient::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &NetworkClient::onSocketError);
    connect(m_socket, &QTcpSocket::stateChanged, this, &NetworkClient::onSocketStateChanged);
}

NetworkClient::~NetworkClient()
{
    disconnectFromServer();
}

bool NetworkClient::connectToServer(const QString &host, quint16 port)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        emit errorOccurred("Đã kết nối đến server");
        return false;
    }

    qDebug() << "Connecting to server:" << host << ":" << port;
    m_socket->connectToHost(host, port);
    
    // Wait for connection (with timeout)
    if (!m_socket->waitForConnected(3000)) {
        qDebug() << "Connection failed:" << m_socket->errorString();
        return false;
    }
    
    qDebug() << "Connected successfully";
    return true;
}

void NetworkClient::disconnectFromServer()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    }
    m_loggedIn = false;
    m_token.clear();
    m_buffer.clear();
}

bool NetworkClient::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

bool NetworkClient::isLoggedIn() const
{
    return m_loggedIn;
}

QString NetworkClient::getToken() const
{
    return m_token;
}

QByteArray NetworkClient::createPacketHeader(quint16 cmd, quint16 user_id, quint32 length)
{
    QByteArray header;
    QDataStream stream(&header, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);  // Network byte order
    stream << cmd << user_id << length;
    return header;
}

void NetworkClient::sendPacket(quint16 cmd, quint16 user_id, const QByteArray &json)
{
    if (!isConnected()) {
        emit errorOccurred("Chưa kết nối đến server");
        return;
    }

    QByteArray header = createPacketHeader(cmd, user_id, json.size());
    m_socket->write(header);
    m_socket->write(json);
    m_socket->flush();

    qDebug() << "Sent packet: cmd=" << QString::number(cmd, 16) 
             << "user_id=" << user_id << "length=" << json.size();

    // Use async signal/slot mechanism only - no blocking wait
    // Response will be handled by onReadyRead() when data arrives
    // Check if data is already available (non-blocking)
    if (m_socket->bytesAvailable() > 0) {
        qDebug() << "Data already available, reading response...";
        onReadyRead();
    }
}

void NetworkClient::sendRegister(const QString &username, const QString &password)
{
    QJsonObject obj;
    obj["username"] = username;
    obj["password"] = password;

    QJsonDocument doc(obj);
    QByteArray json = doc.toJson(QJsonDocument::Compact);

    sendPacket(CMD_REQ_REGISTER, 0, json);
}

void NetworkClient::sendLogin(const QString &username, const QString &password)
{
    QJsonObject obj;
    obj["username"] = username;
    obj["password"] = password;

    QJsonDocument doc(obj);
    QByteArray json = doc.toJson(QJsonDocument::Compact);

    qDebug() << "Sending login request for username:" << username;
    sendPacket(CMD_REQ_LOGIN, 0, json);
}

void NetworkClient::sendLogout()
{
    QJsonObject obj;  // Empty object
    QJsonDocument doc(obj);
    QByteArray json = doc.toJson(QJsonDocument::Compact);

    quint16 user_id = m_loggedIn ? m_userId : 0;
    sendPacket(CMD_REQ_LOGOUT, user_id, json);
}

quint16 NetworkClient::getUserId() const
{
    return m_userId;
}

void NetworkClient::sendStartQuickMode()
{
    QJsonObject obj;  // Empty object
    QJsonDocument doc(obj);
    QByteArray json = doc.toJson(QJsonDocument::Compact);

    sendPacket(CMD_REQ_START_QUICKMODE, m_userId, json);
}

void NetworkClient::sendGetQuestion(qint64 sessionId, int round)
{
    qDebug() << "sendGetQuestion: sessionId=" << sessionId << "round=" << round;
    QJsonObject obj;
    obj["session_id"] = sessionId;
    obj["round"] = round;

    QJsonDocument doc(obj);
    QByteArray json = doc.toJson(QJsonDocument::Compact);
    
    qDebug() << "Sending CMD_REQ_GET_QUESTION with JSON:" << json;

    sendPacket(CMD_REQ_GET_QUESTION, m_userId, json);
}

void NetworkClient::sendSubmitAnswer(qint64 sessionId, int round, const QString &answer)
{
    QJsonObject obj;
    obj["session_id"] = sessionId;
    obj["round"] = round;
    obj["answer"] = answer;

    QJsonDocument doc(obj);
    QByteArray json = doc.toJson(QJsonDocument::Compact);

    sendPacket(CMD_REQ_SUBMIT_ANSWER, m_userId, json);
}

void NetworkClient::sendUseLifeline(qint64 sessionId, int round)
{
    QJsonObject obj;
    obj["session_id"] = sessionId;
    obj["round"] = round;

    QJsonDocument doc(obj);
    QByteArray json = doc.toJson(QJsonDocument::Compact);

    sendPacket(CMD_REQ_USE_LIFELINE, m_userId, json);
}

void NetworkClient::onReadyRead()
{
    QByteArray data = m_socket->readAll();
    qDebug() << "Received data:" << data.size() << "bytes";
    m_buffer.append(data);
    processBuffer();
}

void NetworkClient::processBuffer()
{
    while (m_buffer.size() >= 8) {  // Minimum header size
        // Read header
        QDataStream stream(m_buffer);
        stream.setByteOrder(QDataStream::BigEndian);

        quint16 cmd, user_id;
        quint32 length;
        stream >> cmd >> user_id >> length;

        qDebug() << "Processing packet: cmd=" << QString::number(cmd, 16) 
                 << "user_id=" << user_id << "length=" << length 
                 << "buffer_size=" << m_buffer.size();

        // Check if we have complete packet
        if (m_buffer.size() < 8 + static_cast<int>(length)) {
            qDebug() << "Waiting for more data. Need:" << (8 + length) << "Have:" << m_buffer.size();
            // Wait for more data
            return;
        }

        // Extract payload
        QByteArray payload = m_buffer.mid(8, static_cast<int>(length));
        m_buffer.remove(0, 8 + static_cast<int>(length));

        qDebug() << "Payload:" << payload;

        // Parse and emit signal
        parsePacket(cmd, payload);
    }
}

void NetworkClient::parsePacket(quint16 cmd, const QByteArray &jsonData)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);

    if (error.error != QJsonParseError::NoError) {
        qDebug() << "JSON parse error:" << error.errorString();
        emit errorOccurred("Lỗi định dạng phản hồi từ server");
        return;
    }

    QJsonObject obj = doc.object();

    qDebug() << "Received packet: cmd=" << QString::number(cmd, 16) << "data=" << obj;

    switch (cmd) {
        case CMD_RES_REGISTER:
            if (obj.contains("error")) {
                emit registerResponse(false, obj["error"].toString());
            } else {
                emit registerResponse(true, "");
            }
            break;

        case CMD_RES_LOGIN:
            if (obj.contains("error")) {
                emit loginResponse(false, "", obj["error"].toString());
            } else if (obj.contains("token")) {
                m_token = obj["token"].toString();
                m_loggedIn = true;
                // Extract user_id if available
                if (obj.contains("user_id")) {
                    m_userId = static_cast<quint16>(obj["user_id"].toInt());
                }
                emit loginResponse(true, m_token, "");
            } else {
                emit loginResponse(false, "", "Phản hồi không hợp lệ");
            }
            break;

        case CMD_RES_LOGOUT:
            m_loggedIn = false;
            m_token.clear();
            m_userId = 0;
            emit logoutResponse(true);
            break;

        // QuickMode responses
        case CMD_NOTIFY_GAME_START:
            if (obj.contains("session_id") && obj.contains("total_rounds")) {
                qint64 sessionId = obj["session_id"].toVariant().toLongLong();
                int totalRounds = obj["total_rounds"].toInt();
                // Reset duplicate tracking when new game starts
                m_lastQuestionSessionId = 0;
                m_lastQuestionRound = 0;
                m_lastQuestionId = 0;
                emit quickModeGameStart(sessionId, totalRounds);
            }
            break;

        case CMD_NOTIFY_QUESTION:
            qDebug() << "CMD_NOTIFY_QUESTION received, checking fields...";
            qDebug() << "Has session_id:" << obj.contains("session_id");
            qDebug() << "Has round:" << obj.contains("round");
            qDebug() << "Has content:" << obj.contains("content");
            qDebug() << "Has options:" << obj.contains("options");
            if (obj.contains("session_id") && obj.contains("round") && 
                obj.contains("content") && obj.contains("options")) {
                qint64 sessionId = obj["session_id"].toVariant().toLongLong();
                int round = obj["round"].toInt();
                qint64 questionId = obj["question_id"].toVariant().toLongLong();
                
                // SOLUTION 1: Prevent duplicate at NetworkClient level (early blocking)
                if (sessionId == m_lastQuestionSessionId && 
                    round == m_lastQuestionRound && 
                    questionId == m_lastQuestionId) {
                    qDebug() << "[NetworkClient] Duplicate question packet detected and blocked. "
                             << "sessionId=" << sessionId << " round=" << round 
                             << " questionId=" << questionId;
                    break;  // Don't emit signal
                }
                
                // Update tracking
                m_lastQuestionSessionId = sessionId;
                m_lastQuestionRound = round;
                m_lastQuestionId = questionId;
                
                QString content = obj["content"].toString();
                QJsonObject options = obj["options"].toObject();
                QString difficulty = obj["difficulty"].toString();
                qDebug() << "Emitting quickModeQuestionReceived: sessionId=" << sessionId 
                         << "round=" << round << "content=" << content;
                emit quickModeQuestionReceived(sessionId, round, questionId, content, options, difficulty);
            } else {
                qDebug() << "CMD_NOTIFY_QUESTION missing required fields!";
            }
            break;

        case CMD_RES_SUBMIT_ANSWER:
            if (obj.contains("session_id") && obj.contains("round")) {
                qint64 sessionId = obj["session_id"].toVariant().toLongLong();
                int round = obj["round"].toInt();
                bool correct = obj["correct"].toBool();
                QString correctAnswer = obj["correct_answer"].toString();
                int score = obj["score"].toInt();
                bool gameOver = obj["game_over"].toBool();
                emit quickModeAnswerResult(sessionId, round, correct, correctAnswer, score, gameOver);
            }
            break;

        case CMD_NOTIFY_ANSWER_RESULT:
            // Additional notification, can be ignored or used for UI updates
            break;

        case CMD_NOTIFY_GAME_OVER:
            if (obj.contains("session_id")) {
                qint64 sessionId = obj["session_id"].toVariant().toLongLong();
                int finalScore = obj["final_score"].toInt();
                int totalRounds = obj["total_rounds"].toInt();
                QString status = obj["status"].toString();
                bool win = obj["win"].toBool();
                emit quickModeGameOver(sessionId, finalScore, totalRounds, status, win);
            }
            break;

        case CMD_RES_USE_LIFELINE:
            if (obj.contains("session_id") && obj.contains("round") && 
                obj.contains("remaining_options") && obj.contains("removed_options")) {
                qint64 sessionId = obj["session_id"].toVariant().toLongLong();
                int round = obj["round"].toInt();
                QStringList remainingOptions;
                QJsonArray remainingArr = obj["remaining_options"].toArray();
                for (const QJsonValue &val : remainingArr) {
                    remainingOptions << val.toString();
                }
                QStringList removedOptions;
                QJsonArray removedArr = obj["removed_options"].toArray();
                for (const QJsonValue &val : removedArr) {
                    removedOptions << val.toString();
                }
                int remaining = obj["lifeline_remaining"].toInt();
                emit quickModeLifelineResult(sessionId, round, remainingOptions, removedOptions, remaining);
            }
            break;

        default:
            qDebug() << "Unknown command:" << QString::number(cmd, 16);
            break;
    }
}

void NetworkClient::onSocketError(QAbstractSocket::SocketError error)
{
    QString errorMsg;
    switch (error) {
        case QAbstractSocket::ConnectionRefusedError:
            errorMsg = "Kết nối bị từ chối. Kiểm tra server có đang chạy không.";
            break;
        case QAbstractSocket::HostNotFoundError:
            errorMsg = "Không tìm thấy server. Kiểm tra địa chỉ host.";
            break;
        case QAbstractSocket::NetworkError:
            errorMsg = "Lỗi mạng. Kiểm tra kết nối internet.";
            break;
        case QAbstractSocket::RemoteHostClosedError:
            // Server closed connection - this is normal after response
            // Try to read any remaining data first
            if (m_socket->bytesAvailable() > 0) {
                qDebug() << "Server closed but data available, reading...";
                onReadyRead();
            }
            // Don't emit error if we're logged in (server closed after successful login)
            if (!m_loggedIn) {
                errorMsg = "Server đã đóng kết nối.";
                emit errorOccurred(errorMsg);
            }
            return;  // Don't emit error if logged in
        default:
            errorMsg = "Lỗi kết nối: " + m_socket->errorString();
            break;
    }
    emit errorOccurred(errorMsg);
}

void NetworkClient::onSocketStateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << "Socket state changed:" << state;
    if (state == QAbstractSocket::ConnectedState) {
        emit connected();
    } else if (state == QAbstractSocket::UnconnectedState) {
        qDebug() << "Socket disconnected";
        
        // Try to read any remaining data before disconnecting
        if (m_socket->bytesAvailable() > 0) {
            qDebug() << "Reading remaining data before disconnect:" << m_socket->bytesAvailable() << "bytes";
            onReadyRead();
        }
        
        emit disconnected();
        
        // Only clear login state if we didn't successfully login
        // (server closes connection after response, which is normal)
        if (!m_loggedIn) {
            m_token.clear();
        }
    }
}

