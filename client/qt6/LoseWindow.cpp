#include "LoseWindow.h"
#include "QuickModeWindow.h"
#include "GameModeSelectionWindow.h"
#include "NetworkClient.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox>

LoseWindow::LoseWindow(const QString &username, int questionsAnswered, NetworkClient *client, QWidget *parent)
    : QMainWindow(parent)
    , m_username(username)
    , m_questionsAnswered(questionsAnswered)
    , m_client(client)
{
    setWindowTitle("Kết Thúc - Ai là triệu phú");
    setMinimumSize(500, 400);
    
    // Center window
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    // Create central widget
    m_centralWidget = new QWidget(this);
    m_layout = new QVBoxLayout(m_centralWidget);
    m_layout->setAlignment(Qt::AlignCenter);
    m_layout->setSpacing(30);
    
    // Title
    m_titleLabel = new QLabel("😔 Rất tiếc!", m_centralWidget);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(36);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(
        "QLabel {"
        "    color: #f44336;"
        "    background-color: white;"
        "    padding: 20px;"
        "    border-radius: 15px;"
        "    border: 3px solid #f44336;"
        "}"
    );
    
    // Message
    m_messageLabel = new QLabel(
        QString("❌ Bạn đã trả lời sai!\n\n"
                "📊 Số câu đã trả lời đúng: %1\n\n"
                "💪 Hãy thử lại lần sau! 💪").arg(questionsAnswered),
        m_centralWidget
    );
    QFont msgFont = m_messageLabel->font();
    msgFont.setPointSize(16);
    msgFont.setBold(true);
    m_messageLabel->setFont(msgFont);
    m_messageLabel->setAlignment(Qt::AlignCenter);
    m_messageLabel->setWordWrap(true);
    m_messageLabel->setStyleSheet(
        "QLabel {"
        "    color: white;"
        "    background-color: rgba(255,255,255,0.2);"
        "    padding: 20px;"
        "    border-radius: 10px;"
        "}"
    );
    
    // Buttons
    m_playAgainButton = new QPushButton("🔄 Chơi lại", m_centralWidget);
    m_playAgainButton->setMinimumSize(220, 60);
    m_playAgainButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #f093fb, stop:1 #f5576c);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 12px;"
        "    padding: 12px;"
        "    font-size: 14pt;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #f0a3fb, stop:1 #f5677c);"
        "}"
    );
    m_backToMenuButton = new QPushButton("🏠 Về menu chính", m_centralWidget);
    m_backToMenuButton->setMinimumSize(220, 60);
    m_backToMenuButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #667eea, stop:1 #764ba2);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 12px;"
        "    padding: 12px;"
        "    font-size: 14pt;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #7c8ef0, stop:1 #8a5fb8);"
        "}"
    );
    
    connect(m_playAgainButton, &QPushButton::clicked, this, &LoseWindow::onPlayAgainClicked);
    connect(m_backToMenuButton, &QPushButton::clicked, this, &LoseWindow::onBackToMenuClicked);
    
    m_layout->addWidget(m_titleLabel);
    m_layout->addWidget(m_messageLabel);
    m_layout->addWidget(m_playAgainButton);
    m_layout->addWidget(m_backToMenuButton);
    m_layout->addStretch();
    
    setCentralWidget(m_centralWidget);
    setStyleSheet(
        "QMainWindow {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #ee0979, stop:1 #ff6a00);"
        "}"
    );
}

LoseWindow::~LoseWindow()
{
}

void LoseWindow::onPlayAgainClicked()
{
    if (!m_client) {
        QMessageBox::warning(this, "Lỗi", "NetworkClient không hợp lệ!");
        return;
    }
    
    // Disable button to prevent multiple clicks
    m_playAgainButton->setEnabled(false);
    
    // Create new QuickModeWindow without parent to avoid window management issues
    QuickModeWindow *quickModeWindow = new QuickModeWindow(m_username, m_client, nullptr);
    quickModeWindow->setAttribute(Qt::WA_DeleteOnClose, true);  // Auto-delete when closed
    quickModeWindow->show();
    
    // Close this window
    this->close();
}

void LoseWindow::onBackToMenuClicked()
{
    GameModeSelectionWindow *menuWindow = new GameModeSelectionWindow(m_username, this);
    menuWindow->show();
    this->close();
}
