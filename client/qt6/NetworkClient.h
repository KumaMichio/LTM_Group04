#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>

class NetworkClient : public QObject
{
    Q_OBJECT

public:
    explicit NetworkClient(QObject *parent = nullptr);
    ~NetworkClient();

    bool connectToServer(const QString &host, quint16 port);
    void disconnectFromServer();
    bool isConnected() const;
    bool isLoggedIn() const;
    QString getToken() const;

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
    void onSocketStateChanged(QAbstractSocket::SocketState state);

private:
    QTcpSocket *m_socket;
    QString m_token;
    bool m_loggedIn;
    QByteArray m_buffer;  // Buffer for incomplete packets

    // Packet handling
    void sendPacket(quint16 cmd, quint16 user_id, const QByteArray &json);
    void parsePacket(quint16 cmd, const QByteArray &jsonData);
    QByteArray createPacketHeader(quint16 cmd, quint16 user_id, quint32 length);
    void processBuffer();
};

#endif // NETWORKCLIENT_H

