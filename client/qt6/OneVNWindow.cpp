#include "OneVNWindow.h"
#include "GameModeSelectionWindow.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QTime>

OneVNWindow::OneVNWindow(const QString &username, NetworkClient *client, QWidget *parent)
    : QMainWindow(parent)
    , m_client(client)
    , m_username(username)
    , m_userId(client ? client->getUserId() : 0)
    , m_roomId(0)
    , m_sessionId(0)
    , m_isOwner(false)
    , m_currentState(ROOM_SELECTION)
    , m_currentRound(0)
    , m_totalRounds(0)
    , m_myScore(0)
    , m_eliminated(false)
    , m_waitingForAnswer(false)
    , m_questionTimer(nullptr)
    , m_timerUpdateTimer(nullptr)
    , m_timeRemaining(15)
{
    setWindowTitle("1vN Mode - Ai là triệu phú");
    setMinimumSize(1000, 700);
    
    // Center window
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    if (!m_client) {
        QMessageBox::critical(this, "Lỗi", "NetworkClient không hợp lệ!");
        return;
    }
    
    // Connect NetworkClient signals
    connect(m_client, &NetworkClient::oneVNRoomCreated, this, &OneVNWindow::onRoomCreated);
    connect(m_client, &NetworkClient::oneVNRoomJoined, this, &OneVNWindow::onRoomJoined);
    connect(m_client, &NetworkClient::oneVNRoomUpdate, this, &OneVNWindow::onRoomUpdate);
    connect(m_client, &NetworkClient::oneVNGameStart1VN, this, &OneVNWindow::onGameStart1VN);
    connect(m_client, &NetworkClient::oneVNQuestion1VNReceived, this, &OneVNWindow::onQuestion1VNReceived);
    connect(m_client, &NetworkClient::oneVNAnswerResult1VN, this, &OneVNWindow::onAnswerResult1VN);
    connect(m_client, &NetworkClient::oneVNElimination, this, &OneVNWindow::onElimination);
    connect(m_client, &NetworkClient::oneVNGameOver1VN, this, &OneVNWindow::onGameOver1VN);
    connect(m_client, &NetworkClient::errorOccurred, this, [this](const QString &error) {
        QMessageBox::warning(this, "Lỗi", error);
    });
    
    // Create stacked widget for screens
    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);
    
    // Setup all screens
    setupRoomSelectionScreen();
    setupWaitingRoomScreen();
    setupGameScreen();
    setupGameOverScreen();
    
    // Show initial screen
    showScreen(ROOM_SELECTION);
    
    // Set beautiful gradient background
    setStyleSheet(
        "QMainWindow {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "}"
    );
}

OneVNWindow::~OneVNWindow()
{
    if (m_questionTimer) {
        m_questionTimer->stop();
        delete m_questionTimer;
    }
    if (m_timerUpdateTimer) {
        m_timerUpdateTimer->stop();
        delete m_timerUpdateTimer;
    }
}

void OneVNWindow::showScreen(ScreenState state)
{
    m_currentState = state;
    switch (state) {
        case ROOM_SELECTION:
            m_stackedWidget->setCurrentWidget(m_roomSelectionWidget);
            break;
        case WAITING_ROOM:
            m_stackedWidget->setCurrentWidget(m_waitingRoomWidget);
            break;
        case GAME_PLAYING:
            m_stackedWidget->setCurrentWidget(m_gameWidget);
            break;
        case GAME_OVER:
            m_stackedWidget->setCurrentWidget(m_gameOverWidget);
            break;
    }
}

void OneVNWindow::setupRoomSelectionScreen()
{
    m_roomSelectionWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_roomSelectionWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(30);
    
    // Title
    QLabel *titleLabel = new QLabel("🏆 1vN MODE 🏆", m_roomSelectionWidget);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(28);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: white; padding: 10px;");
    
    QLabel *subtitleLabel = new QLabel("Chế độ đối kháng", m_roomSelectionWidget);
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(16);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("color: rgba(255,255,255,0.9); padding: 5px;");
    
    // Create room section
    QWidget *createRoomWidget = new QWidget(m_roomSelectionWidget);
    createRoomWidget->setStyleSheet(
        "QWidget {"
        "    background-color: rgba(255,255,255,0.95);"
        "    border-radius: 15px;"
        "    padding: 20px;"
        "}"
    );
    QVBoxLayout *createLayout = new QVBoxLayout(createRoomWidget);
    createLayout->setSpacing(15);
    
    QLabel *createTitle = new QLabel("Tạo phòng mới", createRoomWidget);
    QFont createTitleFont = createTitle->font();
    createTitleFont.setPointSize(18);
    createTitleFont.setBold(true);
    createTitle->setFont(createTitleFont);
    createTitle->setStyleSheet("color: #667eea;");
    
    // Question count inputs
    QHBoxLayout *configLayout = new QHBoxLayout();
    configLayout->setSpacing(20);
    
    QVBoxLayout *easyLayout = new QVBoxLayout();
    QLabel *easyLabel = new QLabel("Câu dễ:", createRoomWidget);
    m_easyCountSpin = new QSpinBox(createRoomWidget);
    m_easyCountSpin->setRange(0, 10);
    m_easyCountSpin->setValue(5);
    m_easyCountSpin->setStyleSheet("QSpinBox { padding: 8px; font-size: 14px; border: 2px solid #667eea; border-radius: 5px; }");
    easyLayout->addWidget(easyLabel);
    easyLayout->addWidget(m_easyCountSpin);
    
    QVBoxLayout *mediumLayout = new QVBoxLayout();
    QLabel *mediumLabel = new QLabel("Câu trung bình:", createRoomWidget);
    m_mediumCountSpin = new QSpinBox(createRoomWidget);
    m_mediumCountSpin->setRange(0, 10);
    m_mediumCountSpin->setValue(5);
    m_mediumCountSpin->setStyleSheet("QSpinBox { padding: 8px; font-size: 14px; border: 2px solid #667eea; border-radius: 5px; }");
    mediumLayout->addWidget(mediumLabel);
    mediumLayout->addWidget(m_mediumCountSpin);
    
    QVBoxLayout *hardLayout = new QVBoxLayout();
    QLabel *hardLabel = new QLabel("Câu khó:", createRoomWidget);
    m_hardCountSpin = new QSpinBox(createRoomWidget);
    m_hardCountSpin->setRange(0, 10);
    m_hardCountSpin->setValue(5);
    m_hardCountSpin->setStyleSheet("QSpinBox { padding: 8px; font-size: 14px; border: 2px solid #667eea; border-radius: 5px; }");
    hardLayout->addWidget(hardLabel);
    hardLayout->addWidget(m_hardCountSpin);
    
    configLayout->addLayout(easyLayout);
    configLayout->addLayout(mediumLayout);
    configLayout->addLayout(hardLayout);
    
    m_createRoomButton = new QPushButton("Tạo phòng", createRoomWidget);
    m_createRoomButton->setMinimumSize(200, 50);
    m_createRoomButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #4CAF50, stop:1 #45a049);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 10px;"
        "    padding: 12px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #5CBF60, stop:1 #55B059);"
        "}"
    );
    connect(m_createRoomButton, &QPushButton::clicked, this, &OneVNWindow::onCreateRoomClicked);
    
    createLayout->addWidget(createTitle);
    createLayout->addLayout(configLayout);
    createLayout->addWidget(m_createRoomButton, 0, Qt::AlignCenter);
    
    // Join room section
    QWidget *joinRoomWidget = new QWidget(m_roomSelectionWidget);
    joinRoomWidget->setStyleSheet(
        "QWidget {"
        "    background-color: rgba(255,255,255,0.95);"
        "    border-radius: 15px;"
        "    padding: 20px;"
        "}"
    );
    QVBoxLayout *joinLayout = new QVBoxLayout(joinRoomWidget);
    joinLayout->setSpacing(15);
    
    QLabel *joinTitle = new QLabel("Tham gia phòng", joinRoomWidget);
    QFont joinTitleFont = joinTitle->font();
    joinTitleFont.setPointSize(18);
    joinTitleFont.setBold(true);
    joinTitle->setFont(joinTitleFont);
    joinTitle->setStyleSheet("color: #667eea;");
    
    QHBoxLayout *joinInputLayout = new QHBoxLayout();
    QLabel *roomIdLabel = new QLabel("Room ID:", joinRoomWidget);
    m_roomIdInput = new QLineEdit(joinRoomWidget);
    m_roomIdInput->setPlaceholderText("Nhập Room ID");
    m_roomIdInput->setStyleSheet("QLineEdit { padding: 8px; font-size: 14px; border: 2px solid #667eea; border-radius: 5px; }");
    joinInputLayout->addWidget(roomIdLabel);
    joinInputLayout->addWidget(m_roomIdInput);
    
    m_joinRoomButton = new QPushButton("Tham gia", joinRoomWidget);
    m_joinRoomButton->setMinimumSize(200, 50);
    m_joinRoomButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #2196F3, stop:1 #1976D2);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 10px;"
        "    padding: 12px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #42A5F5, stop:1 #2196F3);"
        "}"
    );
    connect(m_joinRoomButton, &QPushButton::clicked, this, &OneVNWindow::onJoinRoomClicked);
    
    joinLayout->addWidget(joinTitle);
    joinLayout->addLayout(joinInputLayout);
    joinLayout->addWidget(m_joinRoomButton, 0, Qt::AlignCenter);
    
    // Add to main layout
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addSpacing(20);
    layout->addWidget(createRoomWidget);
    layout->addSpacing(15);
    layout->addWidget(joinRoomWidget);
    layout->addStretch();
    
    m_stackedWidget->addWidget(m_roomSelectionWidget);
}

void OneVNWindow::setupWaitingRoomScreen()
{
    m_waitingRoomWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_waitingRoomWidget);
    layout->setSpacing(20);
    layout->setAlignment(Qt::AlignCenter);
    
    // Title
    m_waitingRoomTitle = new QLabel("Đang chờ trong phòng...", m_waitingRoomWidget);
    QFont titleFont = m_waitingRoomTitle->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    m_waitingRoomTitle->setFont(titleFont);
    m_waitingRoomTitle->setAlignment(Qt::AlignCenter);
    m_waitingRoomTitle->setStyleSheet("color: white; padding: 10px;");
    
    // Room ID label
    m_roomIdLabel = new QLabel("Room ID: -", m_waitingRoomWidget);
    QFont roomIdFont = m_roomIdLabel->font();
    roomIdFont.setPointSize(16);
    m_roomIdLabel->setFont(roomIdFont);
    m_roomIdLabel->setAlignment(Qt::AlignCenter);
    m_roomIdLabel->setStyleSheet("color: rgba(255,255,255,0.9); padding: 5px;");
    
    // Members list
    QWidget *membersWidget = new QWidget(m_waitingRoomWidget);
    membersWidget->setStyleSheet(
        "QWidget {"
        "    background-color: rgba(255,255,255,0.95);"
        "    border-radius: 15px;"
        "    padding: 20px;"
        "}"
    );
    QVBoxLayout *membersLayout = new QVBoxLayout(membersWidget);
    
    QLabel *membersTitle = new QLabel("Thành viên trong phòng:", membersWidget);
    QFont membersTitleFont = membersTitle->font();
    membersTitleFont.setPointSize(16);
    membersTitleFont.setBold(true);
    membersTitle->setFont(membersTitleFont);
    membersTitle->setStyleSheet("color: #667eea;");
    
    m_membersList = new QListWidget(membersWidget);
    m_membersList->setStyleSheet(
        "QListWidget {"
        "    border: 2px solid #667eea;"
        "    border-radius: 10px;"
        "    padding: 10px;"
        "    background-color: white;"
        "}"
        "QListWidget::item {"
        "    padding: 8px;"
        "    border-bottom: 1px solid #e0e0e0;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #667eea;"
        "    color: white;"
        "}"
    );
    m_membersList->setMinimumHeight(200);
    
    membersLayout->addWidget(membersTitle);
    membersLayout->addWidget(m_membersList);
    
    // Status label
    m_waitingStatusLabel = new QLabel("Chờ owner bắt đầu game...", m_waitingRoomWidget);
    m_waitingStatusLabel->setAlignment(Qt::AlignCenter);
    m_waitingStatusLabel->setStyleSheet("color: rgba(255,255,255,0.9); padding: 10px; font-size: 14px;");
    
    // Buttons
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(20);
    
    m_startGameButton = new QPushButton("Bắt đầu game", m_waitingRoomWidget);
    m_startGameButton->setMinimumSize(200, 50);
    m_startGameButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #4CAF50, stop:1 #45a049);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 10px;"
        "    padding: 12px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #5CBF60, stop:1 #55B059);"
        "}"
        "QPushButton:disabled {"
        "    background-color: #cccccc;"
        "    color: #666666;"
        "}"
    );
    m_startGameButton->setEnabled(false);
    connect(m_startGameButton, &QPushButton::clicked, this, &OneVNWindow::onStartGameClicked);
    
    m_leaveRoomButton = new QPushButton("Rời phòng", m_waitingRoomWidget);
    m_leaveRoomButton->setMinimumSize(200, 50);
    m_leaveRoomButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #f44336, stop:1 #d32f2f);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 10px;"
        "    padding: 12px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #ef5350, stop:1 #e53935);"
        "}"
    );
    connect(m_leaveRoomButton, &QPushButton::clicked, this, &OneVNWindow::onLeaveRoomClicked);
    
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(m_startGameButton);
    buttonsLayout->addWidget(m_leaveRoomButton);
    buttonsLayout->addStretch();
    
    layout->addStretch();
    layout->addWidget(m_waitingRoomTitle);
    layout->addWidget(m_roomIdLabel);
    layout->addWidget(membersWidget);
    layout->addWidget(m_waitingStatusLabel);
    layout->addLayout(buttonsLayout);
    layout->addStretch();
    
    m_stackedWidget->addWidget(m_waitingRoomWidget);
}

void OneVNWindow::setupGameScreen()
{
    m_gameWidget = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(m_gameWidget);
    mainLayout->setSpacing(20);
    
    // Left side: Question and options
    QWidget *questionWidget = new QWidget(m_gameWidget);
    questionWidget->setStyleSheet("background-color: rgba(255,255,255,0.95); border-radius: 15px; padding: 20px;");
    QVBoxLayout *questionLayout = new QVBoxLayout(questionWidget);
    questionLayout->setSpacing(15);
    
    // Round label
    m_gameRoundLabel = new QLabel("Câu hỏi: 0/0", questionWidget);
    QFont roundFont = m_gameRoundLabel->font();
    roundFont.setPointSize(18);
    roundFont.setBold(true);
    m_gameRoundLabel->setFont(roundFont);
    m_gameRoundLabel->setAlignment(Qt::AlignCenter);
    m_gameRoundLabel->setStyleSheet("color: #667eea; padding: 10px; background-color: #f0f0f0; border-radius: 10px;");
    
    // Timer label
    m_gameTimerLabel = new QLabel("⏱ 15", questionWidget);
    QFont timerFont = m_gameTimerLabel->font();
    timerFont.setPointSize(32);
    timerFont.setBold(true);
    m_gameTimerLabel->setFont(timerFont);
    m_gameTimerLabel->setAlignment(Qt::AlignCenter);
    m_gameTimerLabel->setStyleSheet("color: #f44336; padding: 10px;");
    
    // Question label
    m_gameQuestionLabel = new QLabel("", questionWidget);
    QFont questionFont = m_gameQuestionLabel->font();
    questionFont.setPointSize(20);
    questionFont.setBold(true);
    m_gameQuestionLabel->setFont(questionFont);
    m_gameQuestionLabel->setAlignment(Qt::AlignCenter);
    m_gameQuestionLabel->setWordWrap(true);
    m_gameQuestionLabel->setMinimumHeight(150);
    m_gameQuestionLabel->setStyleSheet(
        "QLabel {"
        "    background-color: white;"
        "    color: #333333;"
        "    padding: 20px;"
        "    border-radius: 10px;"
        "    border: 3px solid #667eea;"
        "}"
    );
    
    // Options layout
    QGridLayout *optionsLayout = new QGridLayout();
    optionsLayout->setSpacing(15);
    
    m_gameOptionA = new QPushButton("A:", questionWidget);
    m_gameOptionB = new QPushButton("B:", questionWidget);
    m_gameOptionC = new QPushButton("C:", questionWidget);
    m_gameOptionD = new QPushButton("D:", questionWidget);
    
    QList<QPushButton*> buttons = {m_gameOptionA, m_gameOptionB, m_gameOptionC, m_gameOptionD};
    QStringList colors = {"#FF6B6B", "#4ECDC4", "#FFE66D", "#95E1D3"};
    
    for (int i = 0; i < buttons.size(); i++) {
        QPushButton *btn = buttons[i];
        btn->setMinimumSize(250, 80);
        QFont btnFont = btn->font();
        btnFont.setPointSize(14);
        btnFont.setBold(true);
        btn->setFont(btnFont);
        
        QString color = colors[i];
        btn->setStyleSheet(
            QString("QPushButton {"
                    "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                    "        stop:0 %1, stop:1 %2);"
                    "    color: white;"
                    "    border: none;"
                    "    border-radius: 12px;"
                    "    padding: 15px;"
                    "}"
                    "QPushButton:hover {"
                    "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                    "        stop:0 %3, stop:1 %4);"
                    "}"
                    "QPushButton:disabled {"
                    "    background-color: #cccccc;"
                    "    color: #666666;"
                    "}").arg(color, color, color, color)
        );
        connect(btn, &QPushButton::clicked, this, &OneVNWindow::onAnswerClicked);
    }
    
    optionsLayout->addWidget(m_gameOptionA, 0, 0);
    optionsLayout->addWidget(m_gameOptionB, 0, 1);
    optionsLayout->addWidget(m_gameOptionC, 1, 0);
    optionsLayout->addWidget(m_gameOptionD, 1, 1);
    
    questionLayout->addWidget(m_gameRoundLabel);
    questionLayout->addWidget(m_gameTimerLabel);
    questionLayout->addWidget(m_gameQuestionLabel);
    questionLayout->addLayout(optionsLayout);
    questionLayout->addStretch();
    
    // Right side: Leaderboard
    QWidget *leaderboardWidget = new QWidget(m_gameWidget);
    leaderboardWidget->setStyleSheet("background-color: rgba(255,255,255,0.95); border-radius: 15px; padding: 20px;");
    QVBoxLayout *leaderboardLayout = new QVBoxLayout(leaderboardWidget);
    
    QLabel *leaderboardTitle = new QLabel("🏆 Bảng xếp hạng", leaderboardWidget);
    QFont leaderboardTitleFont = leaderboardTitle->font();
    leaderboardTitleFont.setPointSize(18);
    leaderboardTitleFont.setBold(true);
    leaderboardTitle->setFont(leaderboardTitleFont);
    leaderboardTitle->setStyleSheet("color: #667eea; padding: 5px;");
    
    m_gameLeaderboardList = new QListWidget(leaderboardWidget);
    m_gameLeaderboardList->setStyleSheet(
        "QListWidget {"
        "    border: 2px solid #667eea;"
        "    border-radius: 10px;"
        "    padding: 10px;"
        "    background-color: white;"
        "}"
        "QListWidget::item {"
        "    padding: 8px;"
        "    border-bottom: 1px solid #e0e0e0;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #667eea;"
        "    color: white;"
        "}"
    );
    
    leaderboardLayout->addWidget(leaderboardTitle);
    leaderboardLayout->addWidget(m_gameLeaderboardList);
    
    mainLayout->addWidget(questionWidget, 2);
    mainLayout->addWidget(leaderboardWidget, 1);
    
    m_stackedWidget->addWidget(m_gameWidget);
}

void OneVNWindow::setupGameOverScreen()
{
    m_gameOverWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_gameOverWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(30);
    
    m_winnerLabel = new QLabel("🏆 Game Over 🏆", m_gameOverWidget);
    QFont winnerFont = m_winnerLabel->font();
    winnerFont.setPointSize(28);
    winnerFont.setBold(true);
    m_winnerLabel->setFont(winnerFont);
    m_winnerLabel->setAlignment(Qt::AlignCenter);
    m_winnerLabel->setStyleSheet("color: white; padding: 10px;");
    
    QWidget *leaderboardWidget = new QWidget(m_gameOverWidget);
    leaderboardWidget->setStyleSheet(
        "QWidget {"
        "    background-color: rgba(255,255,255,0.95);"
        "    border-radius: 15px;"
        "    padding: 20px;"
        "}"
    );
    QVBoxLayout *leaderboardLayout = new QVBoxLayout(leaderboardWidget);
    
    QLabel *leaderboardTitle = new QLabel("🏆 Bảng xếp hạng cuối cùng", leaderboardWidget);
    QFont leaderboardTitleFont = leaderboardTitle->font();
    leaderboardTitleFont.setPointSize(20);
    leaderboardTitleFont.setBold(true);
    leaderboardTitle->setFont(leaderboardTitleFont);
    leaderboardTitle->setStyleSheet("color: #667eea; padding: 10px;");
    
    m_finalLeaderboardList = new QListWidget(leaderboardWidget);
    m_finalLeaderboardList->setStyleSheet(
        "QListWidget {"
        "    border: 2px solid #667eea;"
        "    border-radius: 10px;"
        "    padding: 10px;"
        "    background-color: white;"
        "    min-height: 300px;"
        "}"
        "QListWidget::item {"
        "    padding: 10px;"
        "    border-bottom: 1px solid #e0e0e0;"
        "    font-size: 14px;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #667eea;"
        "    color: white;"
        "}"
    );
    m_finalLeaderboardList->setMinimumHeight(300);
    
    leaderboardLayout->addWidget(leaderboardTitle);
    leaderboardLayout->addWidget(m_finalLeaderboardList);
    
    m_backToMenuButton = new QPushButton("Về menu chính", m_gameOverWidget);
    m_backToMenuButton->setMinimumSize(250, 60);
    m_backToMenuButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 12px;"
        "    padding: 15px;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #7c8ff1, stop:1 #8653ab);"
        "}"
    );
    connect(m_backToMenuButton, &QPushButton::clicked, this, [this]() {
        this->close();
        if (parent()) {
            qobject_cast<QWidget*>(parent())->show();
        }
    });
    
    layout->addStretch();
    layout->addWidget(m_winnerLabel);
    layout->addWidget(leaderboardWidget);
    layout->addWidget(m_backToMenuButton, 0, Qt::AlignCenter);
    layout->addStretch();
    
    m_stackedWidget->addWidget(m_gameOverWidget);
}

// Room management slots
void OneVNWindow::onCreateRoomClicked()
{
    int easyCount = m_easyCountSpin->value();
    int mediumCount = m_mediumCountSpin->value();
    int hardCount = m_hardCountSpin->value();
    
    if (easyCount + mediumCount + hardCount == 0) {
        QMessageBox::warning(this, "Lỗi", "Tổng số câu hỏi phải lớn hơn 0!");
        return;
    }
    
    m_client->sendCreateRoom(easyCount, mediumCount, hardCount);
    m_createRoomButton->setEnabled(false);
}

void OneVNWindow::onJoinRoomClicked()
{
    bool ok;
    qint64 roomId = m_roomIdInput->text().toLongLong(&ok);
    
    if (!ok || roomId <= 0) {
        QMessageBox::warning(this, "Lỗi", "Room ID không hợp lệ!");
        return;
    }
    
    m_client->sendJoinRoom(roomId);
    m_joinRoomButton->setEnabled(false);
}

void OneVNWindow::onStartGameClicked()
{
    if (m_roomId == 0) {
        QMessageBox::warning(this, "Lỗi", "Chưa có room ID!");
        return;
    }
    
    m_client->sendStartGame1VN(m_roomId);
    m_startGameButton->setEnabled(false);
}

void OneVNWindow::onLeaveRoomClicked()
{
    if (m_roomId > 0) {
        m_client->sendLeaveRoom(m_roomId);
    }
    showScreen(ROOM_SELECTION);
    m_roomId = 0;
    m_isOwner = false;
    m_membersList->clear();
}

// NetworkClient slots - Room
void OneVNWindow::onRoomCreated(qint64 roomId)
{
    m_roomId = roomId;
    m_isOwner = true;
    m_roomIdLabel->setText(QString("Room ID: %1").arg(roomId));
    m_createRoomButton->setEnabled(true);
    showScreen(WAITING_ROOM);
    m_startGameButton->setEnabled(true);
    m_waitingStatusLabel->setText("Bạn là owner. Bấm 'Bắt đầu game' khi đã có đủ người chơi (tối thiểu 2 người).");
}

void OneVNWindow::onRoomJoined(bool success, const QString &error)
{
    m_joinRoomButton->setEnabled(true);
    
    if (!success) {
        QMessageBox::warning(this, "Lỗi", "Tham gia phòng thất bại: " + error);
        return;
    }
    
    // Get room_id from input (user entered it)
    bool ok;
    qint64 roomId = m_roomIdInput->text().toLongLong(&ok);
    if (ok && roomId > 0) {
        m_roomId = roomId;
    }
    
    // Note: room_id will be set when we receive room update or start game
    // For now, just show waiting screen
    m_roomIdLabel->setText(QString("Room ID: %1").arg(m_roomId));
    showScreen(WAITING_ROOM);
    m_waitingStatusLabel->setText("Đã tham gia phòng. Đang chờ owner bắt đầu game...");
    
    // Clear members list (will be populated when game starts or room update arrives)
    m_membersList->clear();
}

void OneVNWindow::onRoomUpdate(const QJsonArray &members)
{
    // This is called when server broadcasts room updates
    // Currently server doesn't broadcast this, but we handle it for future implementation
    m_membersList->clear();
    m_players.clear();
    
    for (const QJsonValue &val : members) {
        QJsonObject member = val.toObject();
        qint64 userId = member["user_id"].toVariant().toLongLong();
        QString username = member.contains("username") ? member["username"].toString() : QString("User %1").arg(userId);
        
        QString displayText = username;
        if (userId == m_userId) {
            displayText += " (Bạn)";
        }
        
        m_membersList->addItem(displayText);
        
        PlayerInfo player;
        player.userId = userId;
        player.username = username;
        player.score = 0;
        player.eliminated = false;
        player.rank = 0;
        m_players.append(player);
    }
    
    // Update status
    int memberCount = members.size();
    if (m_isOwner) {
        if (memberCount < 2) {
            m_waitingStatusLabel->setText(QString("Cần thêm %1 người chơi nữa để bắt đầu (tối thiểu 2 người).").arg(2 - memberCount));
            m_startGameButton->setEnabled(false);
        } else {
            m_waitingStatusLabel->setText(QString("Đã có %1 người chơi. Có thể bắt đầu game!").arg(memberCount));
            m_startGameButton->setEnabled(true);
        }
    } else {
        m_waitingStatusLabel->setText(QString("Đang chờ owner bắt đầu game... (%1 người chơi)").arg(memberCount));
    }
}

// NetworkClient slots - Game
void OneVNWindow::onGameStart1VN(qint64 sessionId, qint64 roomId, int totalRounds)
{
    m_sessionId = sessionId;
    m_roomId = roomId;
    m_totalRounds = totalRounds;
    m_currentRound = 0;
    m_myScore = 0;
    m_eliminated = false;
    m_waitingForAnswer = false;
    
    // Initialize players list if empty (from waiting room)
    // Players will be populated from leaderboard when first question arrives
    if (m_players.isEmpty()) {
        // Add yourself
        PlayerInfo self;
        self.userId = m_userId;
        self.username = m_username;
        self.score = 0;
        self.eliminated = false;
        self.rank = 0;
        m_players.append(self);
    }
    
    // Reset all players' scores
    for (PlayerInfo &player : m_players) {
        player.score = 0;
        player.eliminated = false;
        player.rank = 0;
    }
    
    showScreen(GAME_PLAYING);
    updateLeaderboard(QJsonArray()); // Empty initially
}

void OneVNWindow::onQuestion1VNReceived(int round, int totalRounds, const QString &difficulty,
                                         qint64 questionId, const QString &content,
                                         const QJsonObject &options, int timeLimit)
{
    if (m_eliminated) {
        return; // Don't show question if eliminated
    }
    
    m_currentRound = round;
    m_totalRounds = totalRounds;
    m_timeRemaining = timeLimit;
    
    // Store question data
    m_currentQuestion.round = round;
    m_currentQuestion.totalRounds = totalRounds;
    m_currentQuestion.difficulty = difficulty;
    m_currentQuestion.questionId = questionId;
    m_currentQuestion.content = content;
    m_currentQuestion.optionA = options["A"].toString();
    m_currentQuestion.optionB = options["B"].toString();
    m_currentQuestion.optionC = options["C"].toString();
    m_currentQuestion.optionD = options["D"].toString();
    
    // Update UI
    m_gameRoundLabel->setText(QString("Câu hỏi: %1/%2 (%3)").arg(round).arg(totalRounds).arg(difficulty));
    m_gameQuestionLabel->setText(content);
    m_gameOptionA->setText("A: " + m_currentQuestion.optionA);
    m_gameOptionB->setText("B: " + m_currentQuestion.optionB);
    m_gameOptionC->setText("C: " + m_currentQuestion.optionC);
    m_gameOptionD->setText("D: " + m_currentQuestion.optionD);
    
    // Enable buttons
    enableAllButtons();
    m_waitingForAnswer = false;
    m_selectedAnswer.clear();
    
    // Start timer
    if (m_questionTimer) {
        m_questionTimer->stop();
        delete m_questionTimer;
    }
    if (m_timerUpdateTimer) {
        m_timerUpdateTimer->stop();
        delete m_timerUpdateTimer;
    }
    
    m_questionTimer = new QTimer(this);
    m_questionTimer->setSingleShot(true);
    m_questionTimer->setInterval(timeLimit * 1000);
    connect(m_questionTimer, &QTimer::timeout, this, &OneVNWindow::onQuestionTimeout);
    m_questionTimer->start();
    
    m_timerUpdateTimer = new QTimer(this);
    connect(m_timerUpdateTimer, &QTimer::timeout, this, &OneVNWindow::updateTimer);
    m_timerUpdateTimer->start(1000); // Update every second
    
    updateTimer(); // Initial update
}

void OneVNWindow::onAnswerResult1VN(bool correct, int score, int totalScore, bool eliminated)
{
    m_waitingForAnswer = false;
    m_myScore = totalScore;
    m_eliminated = eliminated;
    
    // Stop timers
    if (m_questionTimer) {
        m_questionTimer->stop();
    }
    if (m_timerUpdateTimer) {
        m_timerUpdateTimer->stop();
    }
    
    // Highlight answer
    QPushButton *selectedBtn = nullptr;
    if (m_selectedAnswer == "A") selectedBtn = m_gameOptionA;
    else if (m_selectedAnswer == "B") selectedBtn = m_gameOptionB;
    else if (m_selectedAnswer == "C") selectedBtn = m_gameOptionC;
    else if (m_selectedAnswer == "D") selectedBtn = m_gameOptionD;
    
    if (selectedBtn) {
        if (correct) {
            selectedBtn->setStyleSheet(selectedBtn->styleSheet() + 
                "QPushButton { background-color: #4CAF50 !important; }");
        } else {
            selectedBtn->setStyleSheet(selectedBtn->styleSheet() + 
                "QPushButton { background-color: #f44336 !important; }");
        }
    }
    
    disableAllButtons();
    
    if (eliminated) {
        QMessageBox::information(this, "Bị loại", 
            QString("Bạn đã bị loại!\nĐiểm số cuối cùng: %1").arg(totalScore));
    }
}

void OneVNWindow::onElimination(qint64 userId, int round)
{
    // Update player elimination status
    for (PlayerInfo &player : m_players) {
        if (player.userId == userId) {
            player.eliminated = true;
            break;
        }
    }
    
    // Update leaderboard if we have current data
    // (Leaderboard will be updated when next question arrives or game ends)
}

void OneVNWindow::onGameOver1VN(qint64 winnerId, const QJsonArray &leaderboard)
{
    // Stop timers
    if (m_questionTimer) {
        m_questionTimer->stop();
    }
    if (m_timerUpdateTimer) {
        m_timerUpdateTimer->stop();
    }
    
    updateLeaderboard(leaderboard);
    
    // Find winner name
    QString winnerName = "Không có";
    for (const QJsonValue &val : leaderboard) {
        QJsonObject player = val.toObject();
        if (player["user_id"].toVariant().toLongLong() == winnerId) {
            winnerName = QString("User %1").arg(winnerId);
            // Try to get username from players list
            for (const PlayerInfo &p : m_players) {
                if (p.userId == winnerId) {
                    winnerName = p.username;
                    break;
                }
            }
            break;
        }
    }
    
    m_winnerLabel->setText(QString("🏆 Người chiến thắng: %1 🏆").arg(winnerName));
    
    // Update final leaderboard
    m_finalLeaderboardList->clear();
    for (const QJsonValue &val : leaderboard) {
        QJsonObject player = val.toObject();
        int rank = player["rank"].toInt();
        qint64 userId = player["user_id"].toVariant().toLongLong();
        int score = player["score"].toInt();
        bool eliminated = player["eliminated"].toBool();
        
        QString username = QString("User %1").arg(userId);
        for (const PlayerInfo &p : m_players) {
            if (p.userId == userId) {
                username = p.username;
                break;
            }
        }
        
        QString displayText = QString("#%1 - %2: %3 điểm").arg(rank).arg(username).arg(score);
        if (eliminated) {
            displayText += " (Đã loại)";
        }
        if (userId == m_userId) {
            displayText += " ← Bạn";
        }
        
        m_finalLeaderboardList->addItem(displayText);
    }
    
    showScreen(GAME_OVER);
}

// Game actions
void OneVNWindow::onAnswerClicked()
{
    if (m_waitingForAnswer || m_eliminated || m_sessionId == 0) {
        return;
    }
    
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    
    QString answer;
    if (btn == m_gameOptionA) answer = "A";
    else if (btn == m_gameOptionB) answer = "B";
    else if (btn == m_gameOptionC) answer = "C";
    else if (btn == m_gameOptionD) answer = "D";
    else return;
    
    m_selectedAnswer = answer;
    m_waitingForAnswer = true;
    disableAllButtons();
    
    // Calculate time left
    double timeLeft = m_timeRemaining;
    if (m_questionTimer && m_questionTimer->isActive()) {
        int elapsed = m_questionTimer->interval() - m_questionTimer->remainingTime();
        timeLeft = (m_questionTimer->interval() - elapsed) / 1000.0;
        if (timeLeft < 0) timeLeft = 0;
    }
    
    m_client->sendSubmitAnswer1VN(m_sessionId, m_currentRound, answer, timeLeft);
}

void OneVNWindow::onQuestionTimeout()
{
    if (m_waitingForAnswer) {
        return; // Already answered
    }
    
    // Timeout - disable buttons
    disableAllButtons();
    m_timeRemaining = 0;
    m_gameTimerLabel->setText("⏱ 0");
    
    // Server will handle timeout and eliminate players
    // We just wait for next question or game over
}

void OneVNWindow::updateTimer()
{
    if (!m_questionTimer || !m_questionTimer->isActive()) {
        return;
    }
    
    m_timeRemaining = m_questionTimer->remainingTime() / 1000;
    if (m_timeRemaining < 0) m_timeRemaining = 0;
    
    m_gameTimerLabel->setText(QString("⏱ %1").arg(m_timeRemaining));
    
    // Change color when time is running out
    if (m_timeRemaining <= 5) {
        m_gameTimerLabel->setStyleSheet("color: #f44336; padding: 10px; font-weight: bold;");
    } else if (m_timeRemaining <= 10) {
        m_gameTimerLabel->setStyleSheet("color: #ff9800; padding: 10px; font-weight: bold;");
    } else {
        m_gameTimerLabel->setStyleSheet("color: #4CAF50; padding: 10px; font-weight: bold;");
    }
}

void OneVNWindow::updateLeaderboard(const QJsonArray &leaderboard)
{
    m_gameLeaderboardList->clear();
    
    if (leaderboard.isEmpty()) {
        // Use local players data if available
        for (const PlayerInfo &player : m_players) {
            QString displayText = QString("%1: %2 điểm").arg(player.username).arg(player.score);
            if (player.eliminated) {
                displayText += " (Đã loại)";
            }
            if (player.userId == m_userId) {
                displayText += " ← Bạn";
            }
            m_gameLeaderboardList->addItem(displayText);
        }
        return;
    }
    
    for (const QJsonValue &val : leaderboard) {
        QJsonObject player = val.toObject();
        int rank = player["rank"].toInt();
        qint64 userId = player["user_id"].toVariant().toLongLong();
        int score = player["score"].toInt();
        bool eliminated = player["eliminated"].toBool();
        
        QString username = QString("User %1").arg(userId);
        for (PlayerInfo &p : m_players) {
            if (p.userId == userId) {
                username = p.username;
                p.score = score;
                p.eliminated = eliminated;
                p.rank = rank;
                break;
            }
        }
        
        QString displayText = QString("#%1 - %2: %3 điểm").arg(rank).arg(username).arg(score);
        if (eliminated) {
            displayText += " ❌";
        }
        if (userId == m_userId) {
            displayText += " ← Bạn";
        }
        
        m_gameLeaderboardList->addItem(displayText);
    }
}

void OneVNWindow::resetButtonStyles()
{
    // Reset button styles to default
    QStringList colors = {"#FF6B6B", "#4ECDC4", "#FFE66D", "#95E1D3"};
    QList<QPushButton*> buttons = {m_gameOptionA, m_gameOptionB, m_gameOptionC, m_gameOptionD};
    
    for (int i = 0; i < buttons.size(); i++) {
        QString color = colors[i];
        buttons[i]->setStyleSheet(
            QString("QPushButton {"
                    "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                    "        stop:0 %1, stop:1 %2);"
                    "    color: white;"
                    "    border: none;"
                    "    border-radius: 12px;"
                    "    padding: 15px;"
                    "}"
                    "QPushButton:hover {"
                    "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                    "        stop:0 %3, stop:1 %4);"
                    "}"
                    "QPushButton:disabled {"
                    "    background-color: #cccccc;"
                    "    color: #666666;"
                    "}").arg(color, color, color, color)
        );
    }
}

void OneVNWindow::disableAllButtons()
{
    m_gameOptionA->setEnabled(false);
    m_gameOptionB->setEnabled(false);
    m_gameOptionC->setEnabled(false);
    m_gameOptionD->setEnabled(false);
}

void OneVNWindow::enableAllButtons()
{
    if (!m_eliminated) {
        m_gameOptionA->setEnabled(true);
        m_gameOptionB->setEnabled(true);
        m_gameOptionC->setEnabled(true);
        m_gameOptionD->setEnabled(true);
    }
}

