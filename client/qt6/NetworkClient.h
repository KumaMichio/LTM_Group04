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
    
    // QuickMode requests
    void sendStartQuickMode();
    void sendGetQuestion(qint64 sessionId, int round);
    void sendSubmitAnswer(qint64 sessionId, int round, const QString &answer);
    void sendUseLifeline(qint64 sessionId, int round);
    
    // 1vN Mode requests
    void sendCreateRoom(int easyCount, int mediumCount, int hardCount);
    void sendJoinRoom(qint64 roomId);
    void sendLeaveRoom(qint64 roomId);
    void sendStartGame1VN(qint64 roomId);
    void sendSubmitAnswer1VN(qint64 sessionId, int round, const QString &answer, double timeLeft);
    
    quint16 getUserId() const;

signals:
    void connected();
    void disconnected();
    void registerResponse(bool success, const QString &error);
    void loginResponse(bool success, const QString &token, const QString &error);
    void logoutResponse(bool success);
    void errorOccurred(const QString &error);
    
    // QuickMode signals
    void quickModeGameStart(qint64 sessionId, int totalRounds);
    void quickModeQuestionReceived(qint64 sessionId, int round, qint64 questionId, 
                                   const QString &content, const QJsonObject &options, 
                                   const QString &difficulty);
    void quickModeAnswerResult(qint64 sessionId, int round, bool correct, 
                               const QString &correctAnswer, int score, bool gameOver);
    void quickModeGameOver(qint64 sessionId, int finalScore, int totalRounds, 
                          const QString &status, bool win);
    void quickModeLifelineResult(qint64 sessionId, int round, 
                                const QStringList &remainingOptions, 
                                const QStringList &removedOptions, int remaining);
    
    // 1vN Mode signals
    void oneVNRoomCreated(qint64 roomId);
    void oneVNRoomJoined(bool success, const QString &error);
    void oneVNRoomUpdate(const QJsonArray &members);
    void oneVNGameStart1VN(qint64 sessionId, qint64 roomId, int totalRounds);
    void oneVNQuestion1VNReceived(int round, int totalRounds, const QString &difficulty,
                                   qint64 questionId, const QString &content,
                                   const QJsonObject &options, int timeLimit);
    void oneVNAnswerResult1VN(bool correct, int score, int totalScore, bool eliminated);
    void oneVNElimination(qint64 userId, int round);
    void oneVNGameOver1VN(qint64 winnerId, const QJsonArray &leaderboard);

private slots:
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError error);
    void onSocketStateChanged(QAbstractSocket::SocketState state);

private:
    QTcpSocket *m_socket;
    QString m_token;
    bool m_loggedIn;
    quint16 m_userId;  // User ID from login
    QByteArray m_buffer;  // Buffer for incomplete packets
    
    // Duplicate prevention tracking for questions
    qint64 m_lastQuestionSessionId;
    int m_lastQuestionRound;
    qint64 m_lastQuestionId;

    // Packet handling
    void sendPacket(quint16 cmd, quint16 user_id, const QByteArray &json);
    void parsePacket(quint16 cmd, const QByteArray &jsonData);
    QByteArray createPacketHeader(quint16 cmd, quint16 user_id, quint32 length);
    void processBuffer();
};

#endif // NETWORKCLIENT_H

