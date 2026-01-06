import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import "components"

Item {
    id: quickModeGame
    width: parent ? parent.width : 800
    height: parent ? parent.height : 600
    
    property StackView stackView
    property string username: ""
    property int timeLimit: 15
    property int timeRemaining: 15
    property bool timeExpiredHandled: false
    
    // Game state
    property real sessionId: 0  // Use real to handle large session IDs
    property int currentRound: 0
    property int totalRounds: 15
    property int score: 0
    property int lifelineRemaining: 2
    property bool waitingForAnswer: false
    property string selectedAnswer: ""
    property bool answerSubmitted: false  // Flag to prevent double-click
    property int currentQuestionId: 0
    
    // Question data
    property string questionContent: "Đang khởi tạo game..."
    property string optionA: ""
    property string optionB: ""
    property string optionC: ""
    property string optionD: ""
    property string correctAnswer: ""
    
    // Background gradient (dark purple)
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#241437" }
            GradientStop { position: 1.0; color: "#2A1845" }
        }
    }
    
    // Back button overlay
    Rectangle {
        width: 40
        height: 40
        radius: 12
        color: "#46306B"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 20
        anchors.topMargin: 20
        opacity: 0.9
        border.color: "#6A4BA5"
        border.width: 1

        Image {
            anchors.centerIn: parent
            source: "qrc:/icons/arrow-left.svg"
            width: 20
            height: 20
            sourceSize: Qt.size(20, 20)
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (stackView) {
                    stackView.pop()
                }
            }
        }
    }

    // Timer badge
    Rectangle {
        id: timerBadge
        width: 96
        height: 40
        radius: 12
        color: "#46306B"
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 32
        anchors.topMargin: 20
        opacity: 0.9
        border.color: "#6A4BA5"
        border.width: 1

        RowLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 6

            Text {
                Layout.alignment: Qt.AlignVCenter
                text: "Thời gian"
                font.family: "Lexend"
                font.pixelSize: 12
                color: "#EDE7F6"
            }

            Text {
                Layout.alignment: Qt.AlignVCenter
                text: timeRemaining + "s"
                font.family: "Lexend"
                font.pixelSize: 16
                font.bold: true
                color: timeRemaining > 3 ? "#FFC107" : "#E53935"
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 30
        spacing: 20
        
        // Score label
        Text {
            Layout.fillWidth: true
            text: "Câu hỏi: " + currentRound + "/" + totalRounds
            font.family: "Lexend"
            font.pixelSize: 18
            font.bold: true
            color: "#FFFFFF"
            horizontalAlignment: Text.AlignHCenter
        }
        
        // Question label
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            radius: 15
            color: "#443270"
            border.color: "#6A4BA5"
            border.width: 1
            
            ScrollView {
                anchors.fill: parent
                anchors.margins: 20
                clip: true
                
                Text {
                    width: parent.width
                    text: questionContent
                    font.family: "Lexend"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#FFFFFF"
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
        
        // Options buttons
        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 2
            columnSpacing: 15
            rowSpacing: 15
            uniformCellWidths: true
            
            GameOptionButton {
                id: optionAButton
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                Layout.minimumWidth: 0
                optionText: "A: " + optionA
                optionLetter: "A"
                enabled: !waitingForAnswer && optionA.length > 0
                
                onClicked: handleAnswerClick("A")
            }
            
            GameOptionButton {
                id: optionBButton
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                Layout.minimumWidth: 0
                optionText: "B: " + optionB
                optionLetter: "B"
                enabled: !waitingForAnswer && optionB.length > 0
                
                onClicked: handleAnswerClick("B")
            }
            
            GameOptionButton {
                id: optionCButton
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                Layout.minimumWidth: 0
                optionText: "C: " + optionC
                optionLetter: "C"
                enabled: !waitingForAnswer && optionC.length > 0
                
                onClicked: handleAnswerClick("C")
            }
            
            GameOptionButton {
                id: optionDButton
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                Layout.minimumWidth: 0
                optionText: "D: " + optionD
                optionLetter: "D"
                enabled: !waitingForAnswer && optionD.length > 0
                
                onClicked: handleAnswerClick("D")
            }
        }
        
        // Lifeline button (compact pill)
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            Layout.preferredHeight: 50

            Item { Layout.fillWidth: true }

            Rectangle {
                Layout.preferredWidth: 130
                Layout.preferredHeight: 38
                radius: 19
                color: (!waitingForAnswer && lifelineRemaining > 0 && optionA.length > 0) ? "#FFB300" : "#7A5E1A"
                border.color: "#FFC107"
                border.width: 1
                opacity: 0.95

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 6

                    Image {
                        source: "qrc:/icons/refresh-cw.svg"
                        width: 18
                        height: 18
                        sourceSize: Qt.size(18, 18)
                        opacity: (lifelineRemaining > 0) ? 1.0 : 0.5
                    }

                    Text {
                        text: lifelineRemaining > 0 ? "50:50 (" + lifelineRemaining + ")" : "50:50 (Hết)"
                        font.family: "Lexend"
                        font.pixelSize: 13
                        font.bold: true
                        color: "#1D0F2E"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    enabled: !waitingForAnswer && lifelineRemaining > 0 && optionA.length > 0
                    onClicked: {
                        if (sessionId > 0 && currentRound > 0) {
                            networkClient.sendUseLifeline(sessionId, currentRound)
                        }
                    }
                }
            }
        }
    }
    
    // Toast Message
    ToastMessage {
        id: toastMessage
        anchors.fill: parent
    }
    
    // Timer for answer timeout
    Timer {
        id: answerTimeoutTimer
        interval: 5000
        onTriggered: {
            if (waitingForAnswer) {
                waitingForAnswer = false
                toastMessage.show("Không nhận được phản hồi từ server", "#FF5252")
                resetButtonStyles()
            }
        }
    }

    // Countdown timer per question
    Timer {
        id: questionTimer
        interval: 1000
        repeat: true
        running: false
        onTriggered: {
            if (timeRemaining > 0) {
                timeRemaining--
            }
            if (timeRemaining <= 0) {
                running = false
                if (!timeExpiredHandled) {
                    handleTimeExpired()
                }
            }
        }
    }
    
    // Timer for next question delay
    Timer {
        id: nextQuestionTimer
        interval: 1500
        onTriggered: {
            console.log("=== nextQuestionTimer triggered ===")
            console.log("sessionId:", sessionId, "waitingForAnswer:", waitingForAnswer)
            if (sessionId > 0) {
                // Reset flags to allow next question
                waitingForAnswer = false
                answerSubmitted = false
                resetButtonStyles()
                requestNextQuestion()
            }
        }
    }
    
    // Timer for showing lose screen after wrong answer (2 seconds to show red/green colors)
    Timer {
        id: loseScreenTimer
        interval: 2000  // 2 seconds to show answer colors
        onTriggered: {
            if (stackView) {
                stackView.replace("LoseScreen.qml", {
                    "stackView": stackView,
                    "username": username,
                    "questionsAnswered": currentRound - 1
                })
            }
        }
    }
    
    // Connect to NetworkClient signals
    Connections {
        target: networkClient
        
        function onQuickModeGameStart(gameSessionId, rounds) {
            console.log("=== onQuickModeGameStart ===")
            console.log("gameSessionId (raw):", gameSessionId, "type:", typeof gameSessionId)
            sessionId = gameSessionId
            console.log("sessionId (after assignment):", sessionId, "type:", typeof sessionId)
            totalRounds = rounds
            currentRound = 0
            score = 0
            lifelineRemaining = 2
            waitingForAnswer = false
            answerSubmitted = false
            selectedAnswer = ""
            currentQuestionId = 0
            questionContent = "Đang khởi tạo game..."
            requestNextQuestion()
        }
        
        function onQuickModeQuestionReceived(gameSessionId, round, questionId, content, options, difficulty) {
            console.log("=== onQuickModeQuestionReceived ===")
            console.log("gameSessionId:", gameSessionId, "sessionId:", sessionId)
            console.log("round:", round, "currentRound:", currentRound)
            
            if (gameSessionId !== sessionId) {
                console.log("Session ID mismatch, ignoring")
                return
            }
            
            // Prevent duplicate
            if (round === currentRound && currentQuestionId === questionId && questionId !== 0) {
                console.log("Duplicate question, ignoring")
                return
            }
            
            // Reset ALL state first to clear previous question's state
            waitingForAnswer = false
            answerSubmitted = false
            selectedAnswer = ""
            timeRemaining = timeLimit
            timeExpiredHandled = false
            questionTimer.restart()
            
            // Reset all options to empty first (important for 50:50 lifeline cleanup)
            optionA = ""
            optionB = ""
            optionC = ""
            optionD = ""
            
            // Reset button styles BEFORE setting new options (clears previous question's highlight/gray state)
            resetButtonStyles()
            
            // Now set new question data
            currentRound = round
            currentQuestionId = questionId
            questionContent = content
            
            // Set options from QJsonObject - use bracket notation for QML compatibility
            var optA = options ? (options["A"] || "") : ""
            var optB = options ? (options["B"] || "") : ""
            var optC = options ? (options["C"] || "") : ""
            var optD = options ? (options["D"] || "") : ""
            
            console.log("Setting options - A:", optA, "B:", optB, "C:", optC, "D:", optD)
            
            // Set options directly - binding will update automatically
            optionA = optA
            optionB = optB
            optionC = optC
            optionD = optD
            
            console.log("Options set directly - A:", optionA, "B:", optionB, "C:", optionC, "D:", optionD)
            
            // Reset button styles again after setting new options
            // This ensures buttons are in clean state (no gray from previous question or 50:50)
            resetButtonStyles()
            
            // IMPORTANT: Explicitly enable buttons again (they might have been disabled directly in onQuickModeAnswerResult)
            // First, force enable to break any direct assignment, then let binding take over
            optionAButton.enabled = true
            optionBButton.enabled = true
            optionCButton.enabled = true
            optionDButton.enabled = true
            
            // Reset button styles AFTER enabling to ensure correct colors
            // This is important because resetStyle() checks enabled state
            resetButtonStyles()
            
            // Now binding will control: enabled: !waitingForAnswer && optionA.length > 0
            // Since waitingForAnswer = false and options are set, buttons will be enabled
            
            console.log("Question received, buttons enabled. waitingForAnswer:", waitingForAnswer, "answerSubmitted:", answerSubmitted)
            console.log("Options - A:", optionA, "B:", optionB, "C:", optionC, "D:", optionD)
            console.log("Button enabled states - A:", optionAButton.enabled, "B:", optionBButton.enabled, "C:", optionCButton.enabled, "D:", optionDButton.enabled)
        }
        
        function onQuickModeAnswerResult(gameSessionId, round, correct, answer, newScore, gameOver) {
            if (gameSessionId !== sessionId) return
            
            answerTimeoutTimer.stop()
            questionTimer.stop()
            
            // IMPORTANT: Keep buttons disabled until next question arrives
            // DO NOT reset waitingForAnswer and answerSubmitted here
            // They will be reset when onQuickModeQuestionReceived is called
            
            score = newScore
            correctAnswer = answer
            
            // Explicitly disable all buttons to prevent further clicks
            optionAButton.enabled = false
            optionBButton.enabled = false
            optionCButton.enabled = false
            optionDButton.enabled = false
            
            // Highlight buttons immediately
            // If correct: highlight selected answer in green
            // If wrong: highlight selected answer in red, then highlight correct answer in green
            highlightAnswer(selectedAnswer, correct)
            
            if (!correct) {
                // Wrong answer: also highlight correct answer in green
                // Use Qt.callLater to ensure wrong answer (red) is shown first, then correct (green)
                Qt.callLater(function() {
                    highlightAnswer(correctAnswer, true)
                })
            }
            
            if (correct) {
                // Correct answer - proceed to next question
                if (gameOver) {
                    // Game over but correct answer - will be handled by onGameOver
                    return
                }
                nextQuestionTimer.start()
            } else {
                // Wrong answer - show colors for 2 seconds, then show lose screen
                // Even if gameOver, still wait 2 seconds to show colors
                loseScreenTimer.start()
            }
        }
        
        function onQuickModeGameOver(gameSessionId, finalScore, rounds, status, win) {
            if (gameSessionId !== sessionId) return
            
            if (win) {
                // Win - show immediately
                if (stackView) {
                    stackView.replace("WinScreen.qml", {
                        "stackView": stackView,
                        "username": username,
                        "score": finalScore
                    })
                }
            } else {
                // Lose - but don't navigate immediately if we're showing answer colors
                // The loseScreenTimer will handle navigation after 2 seconds
                // Only navigate if timer is not running (edge case)
                if (!loseScreenTimer.running) {
                    // Timer not running, navigate immediately
                    if (stackView) {
                        stackView.replace("LoseScreen.qml", {
                            "stackView": stackView,
                            "username": username,
                            "questionsAnswered": currentRound - 1
                        })
                    }
                }
                // Otherwise, let loseScreenTimer handle navigation after 2 seconds
            }
        }
        
        function onQuickModeLifelineResult(gameSessionId, round, remainingOptions, removedOptions, remaining) {
            if (gameSessionId !== sessionId) return
            
            lifelineRemaining = remaining
            
            // Disable removed options
            if (removedOptions.indexOf("A") >= 0) optionA = ""
            if (removedOptions.indexOf("B") >= 0) optionB = ""
            if (removedOptions.indexOf("C") >= 0) optionC = ""
            if (removedOptions.indexOf("D") >= 0) optionD = ""
        }
        
        function onErrorOccurred(error) {
            console.log("=== onErrorOccurred ===")
            console.log("Error:", error)
            
            // If error is about round already answered, reset flags to allow next question
            if (error.indexOf("ROUND_ALREADY_ANSWERED") >= 0 || 
                error.indexOf("already answered") >= 0) {
                console.log("Round already answered - resetting flags")
                // Don't reset here, wait for next question
                // But show a message
                toastMessage.show("Câu hỏi này đã được trả lời", "#FFC107")
            } else {
                toastMessage.show("Lỗi: " + error, "#FF5252")
            }
        }
    }
    
    Component.onCompleted: {
        // Start game
        if (networkClient && networkClient.isConnected()) {
            networkClient.sendStartQuickMode()
        } else {
            toastMessage.show("Chưa kết nối đến server", "#FF5252")
        }
    }
    
    function handleAnswerClick(answer) {
        console.log("=== handleAnswerClick ===")
        console.log("answer:", answer, "waitingForAnswer:", waitingForAnswer, "answerSubmitted:", answerSubmitted, "sessionId:", sessionId)
        
        // Prevent double-click: check both waitingForAnswer and answerSubmitted
        if (waitingForAnswer || answerSubmitted || sessionId === 0) {
            console.log("Cannot handle answer: waitingForAnswer=", waitingForAnswer, "answerSubmitted=", answerSubmitted, "sessionId=", sessionId)
            return
        }
        
        // Set flags IMMEDIATELY to prevent double-click
        answerSubmitted = true
        selectedAnswer = answer
        waitingForAnswer = true  // This will disable buttons via binding
        questionTimer.stop()
        
        console.log("Answer selected, flags set to prevent double-click")
        
        // Start timeout timer
        answerTimeoutTimer.start()
        
        // Send answer to server
        console.log("Sending answer to server:", answer)
        networkClient.sendSubmitAnswer(sessionId, currentRound, answer)
    }
    
    function requestNextQuestion() {
        console.log("=== requestNextQuestion ===")
        console.log("sessionId:", sessionId, "type:", typeof sessionId)
        console.log("currentRound:", currentRound)
        
        if (sessionId === 0) {
            console.log("ERROR: sessionId is 0, cannot request question")
            return
        }
        
        // Reset state before requesting next question
        // Clear all options first to reset 50:50 lifeline state
        optionA = ""
        optionB = ""
        optionC = ""
        optionD = ""
        
        waitingForAnswer = false
        answerSubmitted = false  // Reset double-click prevention flag
        selectedAnswer = ""
        resetButtonStyles()
        
        // Note: Button enabled state will be set automatically when new question arrives
        // via binding: enabled: !waitingForAnswer && optionA.length > 0
        
        currentRound++
        console.log("Calling sendGetQuestion with sessionId:", sessionId, "round:", currentRound)
        networkClient.sendGetQuestion(sessionId, currentRound)
    }
    
    function resetButtonStyles() {
        // Reset all button styles to default state (visual only)
        // Note: enabled state is controlled by binding, don't override here
        optionAButton.resetStyle()
        optionBButton.resetStyle()
        optionCButton.resetStyle()
        optionDButton.resetStyle()
    }
    
    function highlightAnswer(answer, isCorrect) {
        var color = isCorrect ? "#4CAF50" : "#E53935"
        if (answer === "A") optionAButton.highlight(color)
        else if (answer === "B") optionBButton.highlight(color)
        else if (answer === "C") optionCButton.highlight(color)
        else if (answer === "D") optionDButton.highlight(color)
    }

    function handleTimeExpired() {
        if (timeExpiredHandled) return
        timeExpiredHandled = true
        questionTimer.stop()
        answerTimeoutTimer.stop()
        waitingForAnswer = true
        answerSubmitted = true
        optionAButton.enabled = false
        optionBButton.enabled = false
        optionCButton.enabled = false
        optionDButton.enabled = false

        var answeredCount = currentRound > 0 ? currentRound - 1 : 0
        if (stackView) {
            stackView.replace("LoseScreen.qml", {
                "stackView": stackView,
                "username": username,
                "questionsAnswered": answeredCount
            })
        }
    }
}

