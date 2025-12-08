#include "QuickModeWindow.h"
#include "WinWindow.h"
#include "LoseWindow.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QRandomGenerator>

QuickModeWindow::QuickModeWindow(const QString &username, QWidget *parent)
    : QMainWindow(parent)
    , m_username(username)
    , m_currentQuestion(0)
    , m_score(0)
{
    setWindowTitle("QuickMode - Ai là triệu phú");
    setMinimumSize(800, 600);
    
    // Center window
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    // Initialize questions
    initializeQuestions();
    
    // Create central widget
    m_centralWidget = new QWidget(this);
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setSpacing(20);
    
    // Score label
    m_scoreLabel = new QLabel("📊 Câu hỏi: 0/15", m_centralWidget);
    QFont scoreFont = m_scoreLabel->font();
    scoreFont.setPointSize(16);
    scoreFont.setBold(true);
    m_scoreLabel->setFont(scoreFont);
    m_scoreLabel->setAlignment(Qt::AlignCenter);
    m_scoreLabel->setStyleSheet(
        "QLabel {"
        "    background-color: rgba(255,255,255,0.9);"
        "    color: #667eea;"
        "    padding: 10px 20px;"
        "    border-radius: 20px;"
        "    font-weight: bold;"
        "}"
    );
    
    // Question label
    m_questionLabel = new QLabel("", m_centralWidget);
    QFont questionFont = m_questionLabel->font();
    questionFont.setPointSize(18);
    questionFont.setBold(true);
    m_questionLabel->setFont(questionFont);
    m_questionLabel->setAlignment(Qt::AlignCenter);
    m_questionLabel->setWordWrap(true);
    m_questionLabel->setMinimumHeight(120);
    m_questionLabel->setStyleSheet(
        "QLabel {"
        "    background-color: white;"
        "    color: #333333;"
        "    padding: 25px;"
        "    border-radius: 15px;"
        "    border: 3px solid #667eea;"
        "    font-weight: bold;"
        "}"
    );
    
    // Options layout
    m_optionsLayout = new QHBoxLayout();
    m_optionsLayout->setSpacing(15);
    
    // Create option buttons
    m_optionA = new QPushButton("A: ", m_centralWidget);
    m_optionB = new QPushButton("B: ", m_centralWidget);
    m_optionC = new QPushButton("C: ", m_centralWidget);
    m_optionD = new QPushButton("D: ", m_centralWidget);
    
    QList<QPushButton*> buttons = {m_optionA, m_optionB, m_optionC, m_optionD};
    QStringList colors = {"#FF6B6B", "#4ECDC4", "#FFE66D", "#95E1D3"};
    QStringList hoverColors = {"#FF5252", "#3DB8B0", "#FFD93D", "#7DD3C6"};
    
    for (int i = 0; i < buttons.size(); i++) {
        QPushButton *btn = buttons[i];
        btn->setMinimumSize(200, 90);
        QFont btnFont = btn->font();
        btnFont.setPointSize(13);
        btnFont.setBold(true);
        btn->setFont(btnFont);
        
        QString color = colors[i];
        QString hoverColor = hoverColors[i];
        btn->setStyleSheet(
            QString("QPushButton {"
                    "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                    "        stop:0 %1, stop:1 %2);"
                    "    color: white;"
                    "    border: none;"
                    "    border-radius: 12px;"
                    "    padding: 15px;"
                    "    font-weight: bold;"
                    "}"
                    "QPushButton:hover {"
                    "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                    "        stop:0 %3, stop:1 %4);"
                    "}"
                    "QPushButton:pressed {"
                    "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                    "        stop:0 %5, stop:1 %6);"
                    "}"
                    "QPushButton:disabled {"
                    "    background-color: #cccccc;"
                    "    color: #666666;"
                    "}").arg(color, color, hoverColor, hoverColor, color, color)
        );
        connect(btn, &QPushButton::clicked, this, &QuickModeWindow::onAnswerClicked);
    }
    
    m_optionsLayout->addWidget(m_optionA);
    m_optionsLayout->addWidget(m_optionB);
    m_optionsLayout->addWidget(m_optionC);
    m_optionsLayout->addWidget(m_optionD);
    
    // Add to main layout
    m_mainLayout->addWidget(m_scoreLabel);
    m_mainLayout->addWidget(m_questionLabel);
    m_mainLayout->addLayout(m_optionsLayout);
    m_mainLayout->addStretch();
    
    setCentralWidget(m_centralWidget);
    
    // Set beautiful gradient background
    setStyleSheet(
        "QMainWindow {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #f093fb, stop:1 #f5576c);"
        "}"
    );
    
    // Load first question
    loadQuestion();
}

QuickModeWindow::~QuickModeWindow()
{
}

void QuickModeWindow::initializeQuestions()
{
    // Sample questions (temporary - should be loaded from server)
    m_questions = {
        {"Thủ đô của Việt Nam là gì?", "Hà Nội", "Hồ Chí Minh", "Đà Nẵng", "Huế", "A"},
        {"2 + 2 = ?", "3", "4", "5", "6", "B"},
        {"Màu của lá cây thường là gì?", "Đỏ", "Xanh", "Vàng", "Trắng", "B"},
        {"Con vật nào được gọi là 'Chúa tể rừng xanh'?", "Hổ", "Sư tử", "Báo", "Gấu", "B"},
        {"Ngày Quốc khánh Việt Nam là ngày nào?", "1/1", "30/4", "2/9", "20/10", "C"},
        {"Số nguyên tố nhỏ nhất là?", "0", "1", "2", "3", "C"},
        {"Hành tinh nào gần Mặt Trời nhất?", "Sao Kim", "Sao Thủy", "Trái Đất", "Sao Hỏa", "B"},
        {"Ai là tác giả của 'Truyện Kiều'?", "Nguyễn Du", "Hồ Xuân Hương", "Nguyễn Trãi", "Lý Bạch", "A"},
        {"Nước nào có diện tích lớn nhất thế giới?", "Trung Quốc", "Mỹ", "Nga", "Canada", "C"},
        {"Đơn vị đo nhiệt độ phổ biến nhất là?", "Celsius", "Fahrenheit", "Kelvin", "Rankine", "A"},
        {"Thành phố nào được gọi là 'Thành phố không ngủ'?", "Paris", "New York", "Tokyo", "London", "B"},
        {"Số Pi (π) xấp xỉ bằng?", "3.14", "2.71", "1.41", "1.73", "A"},
        {"Loài động vật nào lớn nhất trên cạn?", "Voi", "Hươu cao cổ", "Hà mã", "Tê giác", "A"},
        {"Năm 2024 là năm con gì theo lịch âm?", "Mèo", "Rồng", "Rắn", "Ngựa", "B"},
        {"Ai phát minh ra bóng đèn điện?", "Edison", "Tesla", "Newton", "Einstein", "A"}
    };
}

void QuickModeWindow::loadQuestion()
{
    if (m_currentQuestion >= 15) {
        // Win!
        showWinScreen();
        return;
    }
    
    if (m_currentQuestion >= m_questions.size()) {
        // Not enough questions, repeat
        m_currentQuestionData = m_questions[m_currentQuestion % m_questions.size()];
    } else {
        m_currentQuestionData = m_questions[m_currentQuestion];
    }
    
    showQuestion(m_currentQuestionData);
    m_currentQuestion++;
    m_scoreLabel->setText(QString("Câu hỏi: %1/15").arg(m_currentQuestion));
}

void QuickModeWindow::showQuestion(const Question &q)
{
    m_questionLabel->setText(q.content);
    m_optionA->setText("A: " + q.optionA);
    m_optionB->setText("B: " + q.optionB);
    m_optionC->setText("C: " + q.optionC);
    m_optionD->setText("D: " + q.optionD);
    
    // Enable all buttons
    m_optionA->setEnabled(true);
    m_optionB->setEnabled(true);
    m_optionC->setEnabled(true);
    m_optionD->setEnabled(true);
}

void QuickModeWindow::onAnswerClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;
    
    // Get selected answer
    QString buttonText = clickedButton->text();
    QString selectedAnswer = buttonText.left(1);  // "A", "B", "C", or "D"
    
    m_selectedAnswer = selectedAnswer;
    
    // Disable all buttons
    m_optionA->setEnabled(false);
    m_optionB->setEnabled(false);
    m_optionC->setEnabled(false);
    m_optionD->setEnabled(false);
    
    // Check answer after a short delay
    QTimer::singleShot(500, this, [this, selectedAnswer]() {
        checkAnswer(selectedAnswer);
    });
}

void QuickModeWindow::checkAnswer(const QString &selectedAnswer)
{
    bool isCorrect = (selectedAnswer == m_currentQuestionData.correctAnswer);
    
    // Highlight correct/wrong answer
    QPushButton *selectedBtn = nullptr;
    QPushButton *correctBtn = nullptr;
    
    if (selectedAnswer == "A") selectedBtn = m_optionA;
    else if (selectedAnswer == "B") selectedBtn = m_optionB;
    else if (selectedAnswer == "C") selectedBtn = m_optionC;
    else if (selectedAnswer == "D") selectedBtn = m_optionD;
    
    if (m_currentQuestionData.correctAnswer == "A") correctBtn = m_optionA;
    else if (m_currentQuestionData.correctAnswer == "B") correctBtn = m_optionB;
    else if (m_currentQuestionData.correctAnswer == "C") correctBtn = m_optionC;
    else if (m_currentQuestionData.correctAnswer == "D") correctBtn = m_optionD;
    
    if (selectedBtn) {
        QString color = isCorrect ? QString("#4CAF50") : QString("#f44336");
        selectedBtn->setStyleSheet(QString("QPushButton { background-color: %1; color: white; }").arg(color));
    }
    if (correctBtn && !isCorrect) {
        correctBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; }");
    }
    
    if (isCorrect) {
        m_score++;
        // Continue to next question after delay
        QTimer::singleShot(1500, this, [this]() {
            loadQuestion();
        // Reset button styles
        QList<QPushButton*> buttons = {m_optionA, m_optionB, m_optionC, m_optionD};
        QStringList colors = {"#FF6B6B", "#4ECDC4", "#FFE66D", "#95E1D3"};
        QStringList hoverColors = {"#FF5252", "#3DB8B0", "#FFD93D", "#7DD3C6"};
        
        for (int i = 0; i < buttons.size(); i++) {
            QPushButton *btn = buttons[i];
            QString color = colors[i];
            QString hoverColor = hoverColors[i];
            btn->setStyleSheet(
                QString("QPushButton {"
                        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                        "        stop:0 %1, stop:1 %2);"
                        "    color: white;"
                        "    border: none;"
                        "    border-radius: 12px;"
                        "    padding: 15px;"
                        "    font-weight: bold;"
                        "}"
                        "QPushButton:hover {"
                        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                        "        stop:0 %3, stop:1 %4);"
                        "}"
                        "QPushButton:pressed {"
                        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                        "        stop:0 %5, stop:1 %6);"
                        "}").arg(color, color, hoverColor, hoverColor, color, color)
            );
        }
        });
    } else {
        // Wrong answer - show lose screen
        QTimer::singleShot(1500, this, &QuickModeWindow::showLoseScreen);
    }
}

void QuickModeWindow::showWinScreen()
{
    WinWindow *winWindow = new WinWindow(m_username, m_score, this);
    winWindow->show();
    this->hide();
}

void QuickModeWindow::showLoseScreen()
{
    LoseWindow *loseWindow = new LoseWindow(m_username, m_currentQuestion - 1, this);
    loseWindow->show();
    this->hide();
}

void QuickModeWindow::onTimeout()
{
    // Handle timeout if needed
}

void QuickModeWindow::resetGame()
{
    m_currentQuestion = 0;
    m_score = 0;
    loadQuestion();
}
