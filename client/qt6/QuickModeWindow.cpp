#include "QuickModeWindow.h"
#include "WinWindow.h"
#include "LoseWindow.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QRandomGenerator>
#include <QMap>

QuickModeWindow::QuickModeWindow(const QString &username, NetworkClient *client, QWidget *parent)
    : QMainWindow(parent)
    , m_client(client)
    , m_username(username)
    , m_sessionId(0)
    , m_currentRound(0)
    , m_score(0)
    , m_totalRounds(15)
    , m_lifelineRemaining(2)
    , m_waitingForAnswer(false)
    , m_answerTimeoutTimer(nullptr)
    , m_lastProcessedSessionId(0)
    , m_lastProcessedRound(0)
    , m_gameOverHandled(false)
{
    setWindowTitle("QuickMode - Ai là triệu phú");
    setMinimumSize(800, 600);
    
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
    connect(m_client, &NetworkClient::quickModeGameStart, 
            this, &QuickModeWindow::onGameStart);
    connect(m_client, &NetworkClient::quickModeQuestionReceived, 
            this, &QuickModeWindow::onQuestionReceived);
    connect(m_client, &NetworkClient::quickModeAnswerResult, 
            this, &QuickModeWindow::onAnswerResult);
    connect(m_client, &NetworkClient::quickModeGameOver, 
            this, &QuickModeWindow::onGameOver);
    connect(m_client, &NetworkClient::quickModeLifelineResult, 
            this, &QuickModeWindow::onLifelineResult);
    connect(m_client, &NetworkClient::errorOccurred, 
            this, [this](const QString &error) {
                QMessageBox::warning(this, "Lỗi", error);
            });
    
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
    
    // Lifeline button
    m_lifelineButton = new QPushButton("50:50", m_centralWidget);
    m_lifelineButton->setMinimumSize(150, 50);
    m_lifelineButton->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #FF9800, stop:1 #F57C00);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 10px;"
        "    padding: 10px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #FFB74D, stop:1 #FF9800);"
        "}"
        "QPushButton:disabled {"
        "    background-color: #cccccc;"
        "    color: #666666;"
        "}"
    );
    connect(m_lifelineButton, &QPushButton::clicked, this, &QuickModeWindow::onLifelineClicked);
    
    m_lifelineLayout = new QHBoxLayout();
    m_lifelineLayout->addStretch();
    m_lifelineLayout->addWidget(m_lifelineButton);
    m_lifelineLayout->addStretch();
    
    // Add to main layout
    m_mainLayout->addWidget(m_scoreLabel);
    m_mainLayout->addWidget(m_questionLabel);
    m_mainLayout->addLayout(m_optionsLayout);
    m_mainLayout->addLayout(m_lifelineLayout);
    m_mainLayout->addStretch();
    
    setCentralWidget(m_centralWidget);
    
    // Set beautiful gradient background
    setStyleSheet(
        "QMainWindow {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "        stop:0 #f093fb, stop:1 #f5576c);"
        "}"
    );
    
    // Update lifeline button
    updateLifelineButton();
    
    // Set initial loading message
    m_questionLabel->setText("Đang khởi tạo game...");
    
    // Create timeout timer for answer requests
    m_answerTimeoutTimer = new QTimer(this);
    m_answerTimeoutTimer->setSingleShot(true);
    connect(m_answerTimeoutTimer, &QTimer::timeout, this, [this]() {
        if (m_waitingForAnswer) {
            qDebug() << "Answer timeout, resetting state";
            m_waitingForAnswer = false;
            // Re-enable buttons
            m_optionA->setEnabled(true);
            m_optionB->setEnabled(true);
            m_optionC->setEnabled(true);
            m_optionD->setEnabled(true);
            m_lifelineButton->setEnabled(m_lifelineRemaining > 0);
            QMessageBox::warning(this, "Lỗi", "Không nhận được phản hồi từ server. Vui lòng thử lại.");
        }
    });
    
    // Start game by sending start request
    m_client->sendStartQuickMode();
}

QuickModeWindow::~QuickModeWindow()
{
}

void QuickModeWindow::requestNextQuestion()
{
    if (m_sessionId == 0) {
        qDebug() << "ERROR: Session ID is 0, cannot request question";
        QMessageBox::warning(this, "Lỗi", "Session chưa được khởi tạo!");
        return;
    }
    
    // Reset state before requesting next question
    m_waitingForAnswer = false;
    m_selectedAnswer.clear();
    resetButtonStyles();
    
    // Stop any active timeout timer
    if (m_answerTimeoutTimer && m_answerTimeoutTimer->isActive()) {
        m_answerTimeoutTimer->stop();
    }
    
    m_currentRound++;
    qDebug() << "Requesting question for round" << m_currentRound << "sessionId=" << m_sessionId;
    m_client->sendGetQuestion(m_sessionId, m_currentRound);
}

void QuickModeWindow::updateLifelineButton()
{
    if (m_lifelineRemaining > 0) {
        m_lifelineButton->setEnabled(true);
        m_lifelineButton->setText(QString("50:50 (%1)").arg(m_lifelineRemaining));
    } else {
        m_lifelineButton->setEnabled(false);
        m_lifelineButton->setText("50:50 (Hết)");
    }
}

void QuickModeWindow::showQuestion(const Question &q)
{
    qDebug() << "showQuestion called with content:" << q.content;
    if (q.content.isEmpty()) {
        qDebug() << "WARNING: Question content is empty!";
        return;
    }
    
    // Force update the label - use setText and then repaint/update
    m_questionLabel->setText(q.content);
    m_questionLabel->repaint();
    m_questionLabel->update();
    
    m_optionA->setText("A: " + q.optionA);
    m_optionB->setText("B: " + q.optionB);
    m_optionC->setText("C: " + q.optionC);
    m_optionD->setText("D: " + q.optionD);
    
    qDebug() << "Question label text set to:" << m_questionLabel->text();
    qDebug() << "Question label actual text:" << m_questionLabel->text();
    
    // Enable all buttons
    m_optionA->setEnabled(true);
    m_optionB->setEnabled(true);
    m_optionC->setEnabled(true);
    m_optionD->setEnabled(true);
}

void QuickModeWindow::onAnswerClicked()
{
    if (m_waitingForAnswer || m_sessionId == 0) return;
    
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;
    
    // Get selected answer
    QString buttonText = clickedButton->text();
    QString selectedAnswer = buttonText.left(1);  // "A", "B", "C", or "D"
    
    m_selectedAnswer = selectedAnswer;
    m_waitingForAnswer = true;
    
    // Disable all buttons
    m_optionA->setEnabled(false);
    m_optionB->setEnabled(false);
    m_optionC->setEnabled(false);
    m_optionD->setEnabled(false);
    m_lifelineButton->setEnabled(false);
    
    // Start timeout timer (5 seconds)
    if (m_answerTimeoutTimer) {
        m_answerTimeoutTimer->start(5000);
    }
    
    // Send answer to server
    m_client->sendSubmitAnswer(m_sessionId, m_currentRound, selectedAnswer);
}

void QuickModeWindow::checkAnswer(const QString &selectedAnswer)
{
    // This is now handled by onAnswerResult slot
    // Keep for compatibility if needed
}

void QuickModeWindow::onGameStart(qint64 sessionId, int totalRounds)
{
    qDebug() << "onGameStart: sessionId=" << sessionId << "totalRounds=" << totalRounds;
    m_sessionId = sessionId;
    m_totalRounds = totalRounds;
    m_currentRound = 0;
    m_score = 0;
    m_lifelineRemaining = 2;
    
    // Reset state
    m_waitingForAnswer = false;
    m_selectedAnswer.clear();
    m_lastProcessedSessionId = 0;
    m_lastProcessedRound = 0;
    m_currentQuestionData.questionId = 0;  // Reset question tracking
    m_gameOverHandled = false;  // Reset game over flag
    
    updateLifelineButton();
    
    // Request first question
    qDebug() << "Requesting first question...";
    requestNextQuestion();
}

void QuickModeWindow::onQuestionReceived(qint64 sessionId, int round, qint64 questionId, 
                                         const QString &content, const QJsonObject &options, 
                                         const QString &difficulty)
{
    qDebug() << "onQuestionReceived: sessionId=" << sessionId << "m_sessionId=" << m_sessionId 
             << "round=" << round << "m_currentRound=" << m_currentRound 
             << "questionId=" << questionId;
    
    if (sessionId != m_sessionId) {
        qDebug() << "Session ID mismatch, ignoring question";
        return;
    }
    
    // SOLUTION 2: Second layer of defense - check duplicate at UI level
    // Use questionId for more reliable duplicate detection
    if (round == m_currentRound && 
        m_currentQuestionData.questionId == questionId && 
        questionId != 0) {
        qDebug() << "[QuickModeWindow] Question for round" << round 
                 << "already processed (duplicate signal), ignoring. questionId=" << questionId;
        return;
    }
    
    // Fallback: also check by content if questionId is 0 (shouldn't happen normally)
    if (round == m_currentRound && 
        questionId == 0 && 
        m_currentQuestionData.content == content && 
        !content.isEmpty()) {
        qDebug() << "[QuickModeWindow] Question for round" << round 
                 << "already processed (duplicate by content), ignoring";
        return;
    }
    
    qDebug() << "Processing question: round=" << round << "content=" << content << "questionId=" << questionId;
    
    // Update current round BEFORE updating question data
    m_currentRound = round;
    
    // Update question data
    m_currentQuestionData.questionId = questionId;
    m_currentQuestionData.content = content;
    m_currentQuestionData.optionA = options["A"].toString();
    m_currentQuestionData.optionB = options["B"].toString();
    m_currentQuestionData.optionC = options["C"].toString();
    m_currentQuestionData.optionD = options["D"].toString();
    
    qDebug() << "Options: A=" << m_currentQuestionData.optionA 
             << "B=" << m_currentQuestionData.optionB
             << "C=" << m_currentQuestionData.optionC
             << "D=" << m_currentQuestionData.optionD;
    
    // Clear the "Đang khởi tạo game..." message first
    if (m_questionLabel->text() == "Đang khởi tạo game...") {
        m_questionLabel->clear();
        m_questionLabel->repaint();
    }
    
    // Show question - force UI update
    showQuestion(m_currentQuestionData);
    m_scoreLabel->setText(QString("📊 Câu hỏi: %1/%2").arg(round).arg(m_totalRounds));
    
    // Force repaint of the entire window to ensure UI updates
    m_questionLabel->repaint();
    m_questionLabel->update();
    QApplication::processEvents(); // Process pending events to ensure UI updates
    
    // Enable buttons
    m_optionA->setEnabled(true);
    m_optionB->setEnabled(true);
    m_optionC->setEnabled(true);
    m_optionD->setEnabled(true);
    updateLifelineButton();
    
    qDebug() << "Question displayed successfully. Label text:" << m_questionLabel->text();
    qDebug() << "Label is visible:" << m_questionLabel->isVisible() << "isEnabled:" << m_questionLabel->isEnabled();
}

void QuickModeWindow::onAnswerResult(qint64 sessionId, int round, bool correct, 
                                    const QString &correctAnswer, int score, bool gameOver)
{
    if (sessionId != m_sessionId) return;
    
    // Prevent duplicate processing
    if (sessionId == m_lastProcessedSessionId && round == m_lastProcessedRound) {
        qDebug() << "Duplicate answer result detected, ignoring. sessionId=" << sessionId << "round=" << round;
        return;
    }
    
    // Update last processed
    m_lastProcessedSessionId = sessionId;
    m_lastProcessedRound = round;
    
    // Stop timeout timer
    if (m_answerTimeoutTimer && m_answerTimeoutTimer->isActive()) {
        m_answerTimeoutTimer->stop();
    }
    
    // Reset waiting flag immediately
    m_waitingForAnswer = false;
    
    m_score = score;
    m_currentQuestionData.correctAnswer = correctAnswer;
    
    // Highlight correct/wrong answer
    QPushButton *selectedBtn = nullptr;
    QPushButton *correctBtn = nullptr;
    
    if (m_selectedAnswer == "A") selectedBtn = m_optionA;
    else if (m_selectedAnswer == "B") selectedBtn = m_optionB;
    else if (m_selectedAnswer == "C") selectedBtn = m_optionC;
    else if (m_selectedAnswer == "D") selectedBtn = m_optionD;
    
    if (correctAnswer == "A") correctBtn = m_optionA;
    else if (correctAnswer == "B") correctBtn = m_optionB;
    else if (correctAnswer == "C") correctBtn = m_optionC;
    else if (correctAnswer == "D") correctBtn = m_optionD;
    
    if (selectedBtn) {
        QString color = correct ? QString("#4CAF50") : QString("#f44336");
        selectedBtn->setStyleSheet(QString("QPushButton { background-color: %1; color: white; }").arg(color));
    }
    if (correctBtn && !correct) {
        correctBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; }");
    }
    
    // Handle game over cases
    // NOTE: Chỉ log, không gọi showWinScreen/showLoseScreen ở đây
    // Việc hiển thị màn hình kết thúc sẽ được xử lý bởi onGameOver() để tránh duplicate
    if (gameOver) {
        if (correct) {
            // Round 15 correct - won! Wait for onGameOver to show win screen
            qDebug() << "Game won! Round 15 answered correctly. Waiting for onGameOver()";
        } else {
            // Wrong answer - lost. Wait for onGameOver to show lose screen
            qDebug() << "Game lost! Wrong answer. Waiting for onGameOver()";
        }
        return;
    }
    
    // Not game over - continue game
    if (correct) {
        // Continue to next question after delay
        // Use QTimer::singleShot with lambda to ensure proper cleanup
        QTimer::singleShot(1500, this, [this]() {
            // Check if game is still active (not closed)
            if (m_sessionId != 0 && !m_waitingForAnswer) {
                resetButtonStyles();
                requestNextQuestion();
            }
        });
    } else {
        // Wrong answer - show lose screen
        // This should not happen if gameOver is false, but handle it anyway
        QTimer::singleShot(1500, this, [this]() {
            if (m_sessionId != 0) {
                showLoseScreen();
            }
        });
    }
}

void QuickModeWindow::onGameOver(qint64 sessionId, int finalScore, int totalRounds, 
                                 const QString &status, bool win)
{
    if (sessionId != m_sessionId) return;
    
    // Prevent duplicate game over handling
    if (m_gameOverHandled) {
        qDebug() << "Game over already handled, ignoring duplicate signal";
        return;
    }
    
    // Mark as handled
    m_gameOverHandled = true;
    
    m_score = finalScore;
    
    qDebug() << "Handling game over: win=" << win << "score=" << finalScore << "status=" << status;
    
    if (win) {
        showWinScreen();
    } else {
        showLoseScreen();
    }
}

void QuickModeWindow::onLifelineResult(qint64 sessionId, int round, 
                                       const QStringList &remainingOptions, 
                                       const QStringList &removedOptions, int remaining)
{
    if (sessionId != m_sessionId) return;
    
    qDebug() << "Lifeline result received: remaining=" << remainingOptions 
             << "removed=" << removedOptions << "remaining_count=" << remaining;
    
    m_lifelineRemaining = remaining;
    updateLifelineButton();
    
    // Visual feedback: Disable and gray out removed options
    for (const QString &opt : removedOptions) {
        QPushButton *btn = nullptr;
        if (opt == "A") btn = m_optionA;
        else if (opt == "B") btn = m_optionB;
        else if (opt == "C") btn = m_optionC;
        else if (opt == "D") btn = m_optionD;
        
        if (btn) {
            btn->setEnabled(false);
            // Gray out the button with visual feedback
            btn->setStyleSheet(
                "QPushButton {"
                "    background-color: #888888;"
                "    color: #ffffff;"
                "    border: 2px solid #666666;"
                "    border-radius: 12px;"
                "    padding: 15px;"
                "    font-weight: bold;"
                "    opacity: 0.5;"
                "}"
            );
            qDebug() << "Disabled option:" << opt;
        }
    }
    
    // Ensure remaining options are enabled and have original style
    // Map option letters to button indices for styling
    QMap<QString, int> optionToIndex;
    optionToIndex["A"] = 0;
    optionToIndex["B"] = 1;
    optionToIndex["C"] = 2;
    optionToIndex["D"] = 3;
    
    QStringList colors = {"#FF6B6B", "#4ECDC4", "#FFE66D", "#95E1D3"};
    QStringList hoverColors = {"#FF5252", "#3DB8B0", "#FFD93D", "#7DD3C6"};
    
    for (const QString &opt : remainingOptions) {
        QPushButton *btn = nullptr;
        if (opt == "A") btn = m_optionA;
        else if (opt == "B") btn = m_optionB;
        else if (opt == "C") btn = m_optionC;
        else if (opt == "D") btn = m_optionD;
        
        if (btn && optionToIndex.contains(opt)) {
            btn->setEnabled(true);
            int idx = optionToIndex[opt];
            QString color = colors[idx];
            QString hoverColor = hoverColors[idx];
            
            // Restore original style for remaining options
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
            qDebug() << "Kept option enabled with original style:" << opt;
        }
    }
    
    // Force UI update
    QApplication::processEvents();
}

void QuickModeWindow::onLifelineClicked()
{
    if (m_waitingForAnswer || m_sessionId == 0 || m_lifelineRemaining <= 0) return;
    
    m_client->sendUseLifeline(m_sessionId, m_currentRound);
    m_lifelineButton->setEnabled(false);
}

void QuickModeWindow::resetButtonStyles()
{
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
                    "}"
                    "QPushButton:disabled {"
                    "    background-color: #cccccc;"
                    "    color: #666666;"
                    "}").arg(color, color, hoverColor, hoverColor, color, color)
        );
    }
}

void QuickModeWindow::showWinScreen()
{
    // Create WinWindow without parent to avoid window management issues
    WinWindow *winWindow = new WinWindow(m_username, m_score, m_client, nullptr);
    winWindow->setAttribute(Qt::WA_DeleteOnClose, true);  // Auto-delete when closed
    winWindow->show();
    
    // Close this window
    this->close();
}

void QuickModeWindow::showLoseScreen()
{
    // Create LoseWindow without parent to avoid window management issues
    LoseWindow *loseWindow = new LoseWindow(m_username, m_currentRound, m_client, nullptr);
    loseWindow->setAttribute(Qt::WA_DeleteOnClose, true);  // Auto-delete when closed
    loseWindow->show();
    
    // Close this window
    this->close();
}

void QuickModeWindow::onTimeout()
{
    // Handle timeout if needed
}

void QuickModeWindow::resetGame()
{
    m_currentRound = 0;
    m_score = 0;
    m_lifelineRemaining = 2;
    m_sessionId = 0;
    m_currentQuestionData.questionId = 0;  // Reset question tracking
    m_lastProcessedSessionId = 0;
    m_lastProcessedRound = 0;
    m_gameOverHandled = false;  // Reset game over flag
    updateLifelineButton();
    m_client->sendStartQuickMode();
}
