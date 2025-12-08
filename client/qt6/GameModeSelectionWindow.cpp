#include "GameModeSelectionWindow.h"
#include "QuickModeWindow.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox>

GameModeSelectionWindow::GameModeSelectionWindow(const QString &username, QWidget *parent)
    : QMainWindow(parent)
    , m_username(username)
{
    setWindowTitle("Chọn Chế Độ Chơi - Ai là triệu phú");
    setMinimumSize(600, 400);
    
    // Center window
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    // Create central widget and layout
    m_centralWidget = new QWidget(this);
    m_layout = new QVBoxLayout(m_centralWidget);
    m_layout->setAlignment(Qt::AlignCenter);
    m_layout->setSpacing(30);
    
    // Welcome label
    m_welcomeLabel = new QLabel("🎮 Chào mừng, " + username + "! 🎮", m_centralWidget);
    QFont welcomeFont = m_welcomeLabel->font();
    welcomeFont.setPointSize(18);
    welcomeFont.setBold(true);
    m_welcomeLabel->setFont(welcomeFont);
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    m_welcomeLabel->setStyleSheet("color: #667eea; background-color: white; padding: 15px; border-radius: 10px;");
    
    // Title label
    m_titleLabel = new QLabel("💰 AI LÀ TRIỆU PHÚ 💰", m_centralWidget);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("color: white; padding: 10px;");
    
    // Subtitle
    QLabel *subtitleLabel = new QLabel("Chọn chế độ chơi", m_centralWidget);
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(14);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("color: rgba(255,255,255,0.9); padding: 5px;");
    
    // QuickMode button
    m_quickModeButton = new QPushButton("⚡ QuickMode\nChế độ nhanh - 15 câu hỏi", m_centralWidget);
    m_quickModeButton->setMinimumSize(350, 100);
    QFont buttonFont = m_quickModeButton->font();
    buttonFont.setPointSize(14);
    buttonFont.setBold(true);
    m_quickModeButton->setFont(buttonFont);
    m_quickModeButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #4CAF50, stop:1 #45a049);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 15px;"
        "    padding: 15px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #5CBF60, stop:1 #55B059);"
        "    transform: scale(1.02);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #3C9F40, stop:1 #359039);"
        "}"
    );
    
    // 1vN button
    m_oneVNButton = new QPushButton("⚔️ 1vN Mode\nChế độ đối kháng (Sắp ra mắt)", m_centralWidget);
    m_oneVNButton->setMinimumSize(350, 100);
    m_oneVNButton->setFont(buttonFont);
    m_oneVNButton->setEnabled(false);  // Chưa phát triển
    m_oneVNButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #cccccc;"
        "    color: #666666;"
        "    border: 2px dashed #999999;"
        "    border-radius: 15px;"
        "    padding: 15px;"
        "    font-weight: bold;"
        "}"
    );
    
    // Connect signals
    connect(m_quickModeButton, &QPushButton::clicked, this, &GameModeSelectionWindow::onQuickModeClicked);
    connect(m_oneVNButton, &QPushButton::clicked, this, &GameModeSelectionWindow::onOneVNModeClicked);
    
    // Add to layout
    m_layout->addStretch();
    m_layout->addWidget(m_welcomeLabel);
    m_layout->addWidget(m_titleLabel);
    m_layout->addWidget(subtitleLabel);
    m_layout->addSpacing(20);
    m_layout->addWidget(m_quickModeButton);
    m_layout->addSpacing(15);
    m_layout->addWidget(m_oneVNButton);
    m_layout->addStretch();
    
    // Set central widget
    setCentralWidget(m_centralWidget);
    
    // Set beautiful gradient background
    setStyleSheet(
        "QMainWindow {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "}"
    );
}

GameModeSelectionWindow::~GameModeSelectionWindow()
{
}

void GameModeSelectionWindow::onQuickModeClicked()
{
    // Open QuickMode window
    QuickModeWindow *quickModeWindow = new QuickModeWindow(m_username, this);
    quickModeWindow->show();
    this->hide();
}

void GameModeSelectionWindow::onOneVNModeClicked()
{
    QMessageBox::information(this, "Thông báo", "Chế độ 1vN đang được phát triển!");
}
