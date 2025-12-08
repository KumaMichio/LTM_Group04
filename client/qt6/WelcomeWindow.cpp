#include "WelcomeWindow.h"
#include <QApplication>
#include <QScreen>

WelcomeWindow::WelcomeWindow(const QString &username, QWidget *parent)
    : QMainWindow(parent)
{
    // Set window properties
    setWindowTitle("Chào mừng - Ai là triệu phú");
    setMinimumSize(800, 600);
    
    // Center window on screen
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    // Create central widget and layout
    m_centralWidget = new QWidget(this);
    m_layout = new QVBoxLayout(m_centralWidget);
    m_layout->setAlignment(Qt::AlignCenter);
    
    // Create welcome label
    m_welcomeLabel = new QLabel("hello", m_centralWidget);
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    
    // Style the label
    QFont font = m_welcomeLabel->font();
    font.setPointSize(24);
    font.setBold(true);
    m_welcomeLabel->setFont(font);
    
    // Set white background
    m_centralWidget->setStyleSheet("background-color: white;");
    m_welcomeLabel->setStyleSheet("color: black; background-color: transparent;");
    
    // Add label to layout
    m_layout->addWidget(m_welcomeLabel);
    
    // Set central widget
    setCentralWidget(m_centralWidget);
}

WelcomeWindow::~WelcomeWindow()
{
    // Qt will handle cleanup automatically
}
