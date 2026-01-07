import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import QtQml 6.0
import "components"

Item {
    id: oneVNMode
    objectName: "oneVNMode"
    width: parent ? parent.width : 1400
    height: parent ? parent.height : 800

    property StackView stackView
    property string username: ""
    property bool isJoiningRoom: false

    // Game state
    property int roomId: 0
    property real sessionId: 0
    property bool isOwner: false
    property int ownerId: 0
    property int currentRound: 0
    property int totalRounds: 0
    property int myScore: 0
    property bool eliminated: false
    property bool waitingForAnswer: false
    property int timeRemaining: 15
    property string selectedAnswer: ""

    // Queued question
    property var queuedQuestion: null
    property bool showingScoreMessage: false
    property bool waitingForNextQuestion: false

    // Question data
    property string questionContent: ""
    property string optionA: ""
    property string optionB: ""
    property string optionC: ""
    property string optionD: ""
    property string difficulty: ""

    // Members list
    property var membersList: []
    property var userIdToUsername: ({})

    // Chat messages model
    ListModel { id: chatMessages }

    // Friends model for invites
    ListModel { id: friendsModel }

    // Background
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#241437" }
            GradientStop { position: 1.0; color: "#2A1845" }
        }
    }

    Component.onCompleted: {
        console.log("=== OneVNMode.Component.onCompleted ===")
        console.log("isJoiningRoom:", isJoiningRoom, "roomId:", roomId, "username:", username)

        if (!isJoiningRoom) {
            screenStack.push(roomSelectionScreen)
        }
    }

    // Internal StackView for screens
    StackView {
        id: screenStack
        anchors.fill: parent
    }

    // ---------------------------
    // Room Selection Screen
    // ---------------------------
    Component {
        id: roomSelectionScreen

        ScrollView {
            clip: true

            Item {
                width: parent.width
                height: childrenRect.height

                ColumnLayout {
                    width: Math.min(parent.width - 40, 980)
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 20
                    spacing: 20

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Rectangle {
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 40
                            radius: 20
                            color: Qt.rgba(1.0, 1.0, 1.0, 0.2)

                            Image {
                                anchors.centerIn: parent
                                source: "qrc:/icons/arrow-left.svg"
                                width: 20
                                height: 20
                                sourceSize: Qt.size(20, 20)
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: if (stackView) stackView.pop()
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4

                            Text {
                                Layout.fillWidth: true
                                text: "🏆 1vN MODE 🏆"
                                font.family: "Lexend"
                                font.pixelSize: 24
                                font.bold: true
                                color: "#FFFFFF"
                                horizontalAlignment: Text.AlignHCenter
                            }

                            Text {
                                Layout.fillWidth: true
                                text: "Chế độ đối kháng"
                                font.family: "Lexend"
                                font.pixelSize: 14
                                color: Qt.rgba(1.0, 1.0, 1.0, 0.9)
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }

                        Item { Layout.preferredWidth: 40; Layout.preferredHeight: 40 }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 320
                        radius: 24
                        color: "#FFFFFF"
                        border.width: 0
                        
                        // Shadow effect using rectangle layers
                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: -4
                            radius: parent.radius + 4
                            color: "transparent"
                            border.color: "#20000000"
                            border.width: 8
                            z: -1
                        }

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 24
                            spacing: 20

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 8
                                Text { text: "➕"; font.pixelSize: 20 }
                                Text {
                                    Layout.fillWidth: true
                                    text: "Tạo phòng mới"
                                    font.family: "Lexend"
                                    font.pixelSize: 20
                                    font.bold: true
                                    color: "#667eea"
                                }
                            }

                            Text {
                                Layout.fillWidth: true
                                text: "Thiết lập số lượng câu hỏi cho từng mức độ"
                                font.family: "Lexend"
                                font.pixelSize: 12
                                color: "#666666"
                                wrapMode: Text.WordWrap
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 16

                                // EASY
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 8

                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 80
                                        radius: 10
                                        color: "#FFFFFF"
                                        border.color: "#4A3C6D"
                                        border.width: 1

                                        ColumnLayout {
                                            anchors.fill: parent
                                            anchors.margins: 8
                                            spacing: 4

                                            Text {
                                                Layout.fillWidth: true
                                                text: "Câu dễ"
                                                font.family: "Lexend"
                                                font.pixelSize: 12
                                                font.bold: true
                                                color: "#4CAF50"
                                                horizontalAlignment: Text.AlignHCenter
                                            }

                                            RowLayout {
                                                Layout.fillWidth: true
                                                spacing: 8

                                                Rectangle {
                                                    Layout.preferredWidth: 32
                                                    Layout.preferredHeight: 32
                                                    radius: 16
                                                    color: "#4CAF50"
                                                    Text { anchors.centerIn: parent; text: "-"; font.pixelSize: 18; font.bold: true; color: "#FFFFFF" }
                                                    MouseArea {
                                                        anchors.fill: parent
                                                        onClicked: if (easyCountSpin.value > easyCountSpin.from) easyCountSpin.value--
                                                    }
                                                }

                                                Text {
                                                    Layout.fillWidth: true
                                                    text: easyCountSpin.value.toString()
                                                    font.family: "Lexend"
                                                    font.pixelSize: 18
                                                    font.bold: true
                                                    color: "#4CAF50"
                                                    horizontalAlignment: Text.AlignHCenter
                                                }

                                                Rectangle {
                                                    Layout.preferredWidth: 32
                                                    Layout.preferredHeight: 32
                                                    radius: 16
                                                    color: "#4CAF50"
                                                    Text { anchors.centerIn: parent; text: "+"; font.pixelSize: 18; font.bold: true; color: "#FFFFFF" }
                                                    MouseArea {
                                                        anchors.fill: parent
                                                        onClicked: if (easyCountSpin.value < easyCountSpin.to) easyCountSpin.value++
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    SpinBox { id: easyCountSpin; from: 0; to: 10; value: 5; visible: false }
                                }

                                // MEDIUM
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 8

                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 80
                                        radius: 10
                                        color: "#FFFFFF"
                                        border.color: "#4A3C6D"
                                        border.width: 1

                                        ColumnLayout {
                                            anchors.fill: parent
                                            anchors.margins: 8
                                            spacing: 4

                                            Text {
                                                Layout.fillWidth: true
                                                text: "Câu trung bình"
                                                font.family: "Lexend"
                                                font.pixelSize: 12
                                                font.bold: true
                                                color: "#FFC107"
                                                horizontalAlignment: Text.AlignHCenter
                                            }

                                            RowLayout {
                                                Layout.fillWidth: true
                                                spacing: 8

                                                Rectangle {
                                                    Layout.preferredWidth: 32
                                                    Layout.preferredHeight: 32
                                                    radius: 16
                                                    color: "#FF9800"
                                                    Text { anchors.centerIn: parent; text: "-"; font.pixelSize: 18; font.bold: true; color: "#FFFFFF" }
                                                    MouseArea {
                                                        anchors.fill: parent
                                                        onClicked: if (mediumCountSpin.value > mediumCountSpin.from) mediumCountSpin.value--
                                                    }
                                                }

                                                Text {
                                                    Layout.fillWidth: true
                                                    text: mediumCountSpin.value.toString()
                                                    font.family: "Lexend"
                                                    font.pixelSize: 18
                                                    font.bold: true
                                                    color: "#FFC107"
                                                    horizontalAlignment: Text.AlignHCenter
                                                }

                                                Rectangle {
                                                    Layout.preferredWidth: 32
                                                    Layout.preferredHeight: 32
                                                    radius: 16
                                                    color: "#FF9800"
                                                    Text { anchors.centerIn: parent; text: "+"; font.pixelSize: 18; font.bold: true; color: "#FFFFFF" }
                                                    MouseArea {
                                                        anchors.fill: parent
                                                        onClicked: if (mediumCountSpin.value < mediumCountSpin.to) mediumCountSpin.value++
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    SpinBox { id: mediumCountSpin; from: 0; to: 10; value: 5; visible: false }
                                }

                                // HARD
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 8

                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 80
                                        radius: 10
                                        color: "#FFFFFF"
                                        border.color: "#4A3C6D"
                                        border.width: 1

                                        ColumnLayout {
                                            anchors.fill: parent
                                            anchors.margins: 8
                                            spacing: 4

                                            Text {
                                                Layout.fillWidth: true
                                                text: "Câu khó"
                                                font.family: "Lexend"
                                                font.pixelSize: 12
                                                font.bold: true
                                                color: "#D32F2F"
                                                horizontalAlignment: Text.AlignHCenter
                                            }

                                            RowLayout {
                                                Layout.fillWidth: true
                                                spacing: 8

                                                Rectangle {
                                                    Layout.preferredWidth: 32
                                                    Layout.preferredHeight: 32
                                                    radius: 16
                                                    color: "#F44336"
                                                    Text { anchors.centerIn: parent; text: "-"; font.pixelSize: 18; font.bold: true; color: "#FFFFFF" }
                                                    MouseArea {
                                                        anchors.fill: parent
                                                        onClicked: if (hardCountSpin.value > hardCountSpin.from) hardCountSpin.value--
                                                    }
                                                }

                                                Text {
                                                    Layout.fillWidth: true
                                                    text: hardCountSpin.value.toString()
                                                    font.family: "Lexend"
                                                    font.pixelSize: 18
                                                    font.bold: true
                                                    color: "#D32F2F"
                                                    horizontalAlignment: Text.AlignHCenter
                                                }

                                                Rectangle {
                                                    Layout.preferredWidth: 32
                                                    Layout.preferredHeight: 32
                                                    radius: 16
                                                    color: "#F44336"
                                                    Text { anchors.centerIn: parent; text: "+"; font.pixelSize: 18; font.bold: true; color: "#FFFFFF" }
                                                    MouseArea {
                                                        anchors.fill: parent
                                                        onClicked: if (hardCountSpin.value < hardCountSpin.to) hardCountSpin.value++
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    SpinBox { id: hardCountSpin; from: 0; to: 10; value: 5; visible: false }
                                }
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 50
                                radius: 12
                                color: "#FFC107"
                                Text {
                                    anchors.centerIn: parent
                                    text: "Tạo phòng"
                                    font.family: "Lexend"
                                    font.pixelSize: 16
                                    font.bold: true
                                    color: "#1D0F2E"
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: networkClient.sendCreateRoom(easyCountSpin.value, mediumCountSpin.value, hardCountSpin.value)
                                }
                            }
                        }
                    }
                }
            }
        }
    }

 // ---------------------------
// Waiting Room Screen (RESPONSIVE + NO MISSING BUTTON)
// ---------------------------
Component {
    id: waitingRoomScreen

    Item {
        anchors.fill: parent

        property bool isNarrow: oneVNMode.width < 650
        property int outerMargin: Math.max(12, Math.round(Math.min(oneVNMode.width, oneVNMode.height) * 0.04))
        property int gap: isNarrow ? 12 : 18
        property int headerGap: isNarrow ? 6 : 10
        property int panelRadius: 16

        // ✅ If narrow -> allow vertical scrolling so "Rời phòng" never disappears
        Loader {
            anchors.fill: parent
            sourceComponent: isNarrow ? narrowScrollableLayout : wideLayout
        }

        // ---------- WIDE (2 columns, no scroll) ----------
        Component {
            id: wideLayout

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: outerMargin
                spacing: gap

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: headerGap

                    Text {
                        Layout.fillWidth: true
                        text: "Đang chờ trong phòng..."
                        font.family: "Lexend"
                        font.pixelSize: 26
                        font.bold: true
                        color: "#FFFFFF"
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        Layout.fillWidth: true
                        text: "Room ID: " + (roomId || "???")
                        font.family: "Lexend"
                        font.pixelSize: 16
                        color: Qt.rgba(1.0, 1.0, 1.0, 0.92)
                        horizontalAlignment: Text.AlignHCenter
                    }
                }

                GridLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    columns: 2
                    columnSpacing: gap
                    rowSpacing: gap

                    // Players
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        radius: panelRadius
                        color: "#3A2A5E"
                        border.color: "#4A3C6D"
                        border.width: 1

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 18
                            spacing: 10

                            Text {
                                Layout.fillWidth: true
                                text: "Thành viên trong phòng:"
                                font.family: "Lexend"
                                font.pixelSize: 16
                                font.bold: true
                                color: "#FFFFFF"
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                radius: 10
                                color: "#2D2047"
                                border.color: "#4A3C6D"
                                border.width: 1

                                ListView {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    clip: true
                                    model: membersList.length

                                    delegate: Rectangle {
                                        width: ListView.view.width
                                        height: 42
                                        radius: 8
                                        color: index % 2 === 0 ? "#362958" : "#2D2047"

                                        property var member: membersList[index] || {}

                                        RowLayout {
                                            anchors.fill: parent
                                            anchors.margins: 10
                                            spacing: 10

                                            Rectangle {
                                                Layout.preferredWidth: 28
                                                Layout.preferredHeight: 28
                                                radius: 14
                                                color: "#667eea"
                                                Text {
                                                    anchors.centerIn: parent
                                                    text: (member.username && member.username.length>0) ? member.username.charAt(0).toUpperCase() : "U"
                                                    font.family: "Lexend"
                                                    font.pixelSize: 14
                                                    font.bold: true
                                                    color: "white"
                                                }
                                            }

                                            Text {
                                                Layout.fillWidth: true
                                                text: {
                                                    var name = member.username || ("Người chơi " + (index + 1))
                                                    if (ownerId > 0 && member.userId === ownerId) return name + " (owner)"
                                                    return name
                                                }
                                                font.family: "Lexend"
                                                font.pixelSize: 14
                                                color: "#E0E0E0"
                                                elide: Text.ElideRight
                                            }
                                        }
                                    }

                                    ScrollBar.vertical: ScrollBar { active: true }
                                }
                            }
                        }
                    }

                    // Chat
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        radius: panelRadius
                        color: "#3A2A5E"

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 18
                            spacing: 10

                            Text {
                                Layout.fillWidth: true
                                text: "💬 Chat phòng chờ"
                                font.family: "Lexend"
                                font.pixelSize: 16
                                font.bold: true
                                color: "#FFFFFF"
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                radius: 10
                                color: "#2D2047"
                                border.color: "#4A3C6D"
                                border.width: 1

                                ListView {
                                    id: chatListViewWide
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    clip: true
                                    spacing: 6
                                    model: chatMessages

                                    delegate: Item {
                                        width: ListView.view.width
                                        height: msgText.implicitHeight + 6

                                        Text {
                                            id: msgText
                                            width: parent.width
                                            text: "<b>" + model.sender + ":</b> " + model.message
                                            textFormat: Text.RichText
                                            font.family: "Lexend"
                                            font.pixelSize: 12
                                            color: "#E0E0E0"
                                            wrapMode: Text.WordWrap
                                        }
                                    }

                                    onCountChanged: if (count > 0) positionViewAtEnd()
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 8

                                TextField {
                                    id: chatInputWide
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 38
                                    placeholderText: "Nhập tin nhắn..."
                                    font.family: "Lexend"
                                    font.pixelSize: 12
                                    color: "#FFFFFF"

                                    background: Rectangle {
                                        radius: 19
                                        color: "#362958"
                                        border.color: "#4A3C6D"
                                        border.width: 1
                                    }

                                    onAccepted: sendChatButtonWide.clicked()
                                }

                                Button {
                                    id: sendChatButtonWide
                                    Layout.preferredWidth: 70
                                    Layout.preferredHeight: 38
                                    text: "Gửi"
                                    enabled: chatInputWide.text.trim().length > 0

                                    background: Rectangle {
                                        color: parent.enabled ? "#667eea" : "#CCCCCC"
                                        radius: 10
                                    }

                                    contentItem: Text {
                                        text: parent.text
                                        font.family: "Lexend"
                                        font.pixelSize: 12
                                        font.bold: true
                                        color: "#FFFFFF"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }

                                    onClicked: {
                                        var msg = chatInputWide.text.trim()
                                        if (msg.length > 0 && roomId > 0) {
                                            networkClient.sendRoomChat(roomId, msg)
                                            chatInputWide.text = ""
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // ✅ Wide buttons row (always visible)
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Button {
                        Layout.preferredWidth: 170
                        Layout.preferredHeight: 50
                        text: "Mời bạn bè"
                        font.family: "Lexend"
                        font.pixelSize: 14
                        font.bold: true

                        background: Rectangle {
                            color: "#6A4BA5"
                            radius: 10
                        }

                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: "#FFFFFF"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: {
                            friendsListDialog.roomId = roomId
                            networkClient.sendListFriends()
                            friendsListDialog.open()
                        }
                    }

                    Button {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 50
                        text: isOwner ? "Bắt đầu game" : "Đang chờ chủ phòng..."
                        enabled: isOwner
                        opacity: enabled ? 1.0 : 0.55
                        font.family: "Lexend"
                        font.pixelSize: 14
                        font.bold: true

                        background: Rectangle {
                            color: "#FFC107"
                            radius: 10
                        }

                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: "#1D0F2E"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: if (roomId > 0) networkClient.sendStartGame1VN(roomId)
                    }

                    Button {
                        Layout.preferredWidth: 140
                        Layout.preferredHeight: 50
                        text: "Rời phòng"
                        font.family: "Lexend"
                        font.pixelSize: 14
                        font.bold: true

                        background: Rectangle {
                            color: "#D32F2F"
                            radius: 10
                        }

                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: "#FFFFFF"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: {
                            if (roomId > 0) {
                                networkClient.sendLeaveRoom(roomId)
                                if (stackView) stackView.pop(null)
                                roomId = 0
                                isOwner = false
                                ownerId = 0
                            }
                        }
                    }
                }
            }
        }

        // ---------- NARROW (stacked, SCROLLABLE) ----------
        Component {
            id: narrowScrollableLayout

            ScrollView {
                anchors.fill: parent
                clip: true

                // Important: allow the content to be taller than viewport
                Item {
                    width: ScrollView.view ? ScrollView.view.width : oneVNMode.width
                    height: contentCol.implicitHeight + outerMargin * 2

                    ColumnLayout {
                        id: contentCol
                        width: parent.width
                        anchors.top: parent.top
                        anchors.topMargin: outerMargin
                        anchors.left: parent.left
                        anchors.leftMargin: outerMargin
                        anchors.right: parent.right
                        anchors.rightMargin: outerMargin
                        spacing: gap

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: headerGap

                            Text {
                                Layout.fillWidth: true
                                text: "Đang chờ trong phòng..."
                                font.family: "Lexend"
                                font.pixelSize: 20
                                font.bold: true
                                color: "#FFFFFF"
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                            }

                            Text {
                                Layout.fillWidth: true
                                text: "Room ID: " + (roomId || "???")
                                font.family: "Lexend"
                                font.pixelSize: 14
                                color: Qt.rgba(1.0, 1.0, 1.0, 0.92)
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }

                        // Players panel (fixed-ish height on narrow)
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: Math.max(220, oneVNMode.height * 0.28)
                            radius: panelRadius
                            color: "#3A2A5E"
                            border.color: "#4A3C6D"
                            border.width: 1

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 14
                                spacing: 10

                                Text {
                                    Layout.fillWidth: true
                                    text: "Thành viên trong phòng:"
                                    font.family: "Lexend"
                                    font.pixelSize: 16
                                    font.bold: true
                                    color: "#FFFFFF"
                                }

                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    radius: 10
                                    color: "#2D2047"
                                    border.color: "#4A3C6D"
                                    border.width: 1

                                    ListView {
                                        anchors.fill: parent
                                        anchors.margins: 8
                                        clip: true
                                        model: membersList.length

                                        delegate: Rectangle {
                                            width: ListView.view.width
                                            height: 42
                                            radius: 8
                                            color: index % 2 === 0 ? "#362958" : "#2D2047"

                                            property var member: membersList[index] || {}

                                            RowLayout {
                                                anchors.fill: parent
                                                anchors.margins: 10
                                                spacing: 10

                                                Rectangle {
                                                    Layout.preferredWidth: 28
                                                    Layout.preferredHeight: 28
                                                    radius: 14
                                                    color: "#667eea"
                                                    Text {
                                                        anchors.centerIn: parent
                                                        text: (member.username && member.username.length>0) ? member.username.charAt(0).toUpperCase() : "U"
                                                        font.family: "Lexend"
                                                        font.pixelSize: 14
                                                        font.bold: true
                                                        color: "white"
                                                    }
                                                }

                                                Text {
                                                    Layout.fillWidth: true
                                                    text: {
                                                        var name = member.username || ("Người chơi " + (index + 1))
                                                        if (ownerId > 0 && member.userId === ownerId) return name + " (owner)"
                                                        return name
                                                    }
                                                    font.family: "Lexend"
                                                    font.pixelSize: 14
                                                    color: "#E0E0E0"
                                                    elide: Text.ElideRight
                                                }
                                            }
                                        }

                                        ScrollBar.vertical: ScrollBar { active: true }
                                    }
                                }
                            }
                        }

                        // Chat panel
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: Math.max(260, oneVNMode.height * 0.34)
                            radius: panelRadius
                            color: "#3A2A5E"

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 14
                                spacing: 10

                                Text {
                                    Layout.fillWidth: true
                                    text: "💬 Chat phòng chờ"
                                    font.family: "Lexend"
                                    font.pixelSize: 16
                                    font.bold: true
                                    color: "#FFFFFF"
                                }

                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    radius: 10
                                    color: "#2D2047"
                                    border.color: "#4A3C6D"
                                    border.width: 1

                                    ListView {
                                        id: chatListViewNarrow
                                        anchors.fill: parent
                                        anchors.margins: 8
                                        clip: true
                                        spacing: 6
                                        model: chatMessages

                                        delegate: Item {
                                            width: ListView.view.width
                                            height: msgText2.implicitHeight + 6

                                            Text {
                                                id: msgText2
                                                width: parent.width
                                                text: "<b>" + model.sender + ":</b> " + model.message
                                                textFormat: Text.RichText
                                                font.family: "Lexend"
                                                font.pixelSize: 12
                                                color: "#E0E0E0"
                                                wrapMode: Text.WordWrap
                                            }
                                        }

                                        onCountChanged: if (count > 0) positionViewAtEnd()
                                    }
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 8

                                    TextField {
                                        id: chatInputNarrow
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 38
                                        placeholderText: "Nhập tin nhắn..."
                                        font.family: "Lexend"
                                        font.pixelSize: 12
                                        color: "#FFFFFF"

                                        background: Rectangle {
                                            radius: 19
                                            color: "#362958"
                                            border.color: "#4A3C6D"
                                            border.width: 1
                                        }

                                        onAccepted: sendChatButtonNarrow.clicked()
                                    }

                                    Button {
                                        id: sendChatButtonNarrow
                                        Layout.preferredWidth: 70
                                        Layout.preferredHeight: 38
                                        text: "Gửi"
                                        enabled: chatInputNarrow.text.trim().length > 0

                                        background: Rectangle {
                                            color: parent.enabled ? "#667eea" : "#CCCCCC"
                                            radius: 10
                                        }

                                        contentItem: Text {
                                            text: parent.text
                                            font.family: "Lexend"
                                            font.pixelSize: 12
                                            font.bold: true
                                            color: "#FFFFFF"
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }

                                        onClicked: {
                                            var msg = chatInputNarrow.text.trim()
                                            if (msg.length > 0 && roomId > 0) {
                                                networkClient.sendRoomChat(roomId, msg)
                                                chatInputNarrow.text = ""
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // ✅ Buttons in column (now scrollable, so never "missing")
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 10

                            Button {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 50
                                text: "Mời bạn bè"
                                font.family: "Lexend"
                                font.pixelSize: 14
                                font.bold: true
                                background: Rectangle {
                                    color: "#6A4BA5"
                                    radius: 10
                                }
                                contentItem: Text {
                                    text: parent.text
                                    font: parent.font
                                    color: "#FFFFFF"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onClicked: {
                                    friendsListDialog.roomId = roomId
                                    networkClient.sendListFriends()
                                    friendsListDialog.open()
                                }
                            }

                            Button {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 50
                                text: isOwner ? "Bắt đầu game" : "Đang chờ chủ phòng..."
                                enabled: isOwner
                                opacity: enabled ? 1.0 : 0.55
                                font.family: "Lexend"
                                font.pixelSize: 14
                                font.bold: true
                                background: Rectangle {
                                    color: "#FFC107"
                                    radius: 10
                                }
                                contentItem: Text {
                                    text: parent.text
                                    font: parent.font
                                    color: "#1D0F2E"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onClicked: if (roomId > 0) networkClient.sendStartGame1VN(roomId)
                            }

                            Button {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 50
                                text: "Rời phòng"
                                font.family: "Lexend"
                                font.pixelSize: 14
                                font.bold: true
                                background: Rectangle {
                                    color: "#D32F2F"
                                    radius: 10
                                }
                                contentItem: Text {
                                    text: parent.text
                                    font: parent.font
                                    color: "#FFFFFF"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onClicked: {
                                    if (roomId > 0) {
                                        networkClient.sendLeaveRoom(roomId)
                                        if (stackView) stackView.pop(null)
                                        roomId = 0
                                        isOwner = false
                                        ownerId = 0
                                    }
                                }
                            }
                        }

                        // bottom padding for scroll
                        Item { Layout.preferredHeight: outerMargin }
                    }
                }
            }
        }
    }
}


    // ---------------------------
    // Game Playing Screen (giữ nguyên logic của bạn)
    // ---------------------------
    Component {
        id: gamePlayingScreen

        RowLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 15

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 20

                    Button {
                        text: "✕ Thoát"
                        font.family: "Lexend"
                        font.pixelSize: 14
                        Layout.preferredWidth: 100
                        Layout.preferredHeight: 35

                        background: Rectangle { color: parent.hovered ? "#e53935" : "#f44336"; radius: 8 }

                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: "white"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: exitConfirmDialog.open()
                    }

                    Item { Layout.fillWidth: true }

                    Text {
                        text: "Câu " + currentRound + "/" + totalRounds
                        font.family: "Lexend"
                        font.pixelSize: 18
                        font.bold: true
                        color: "#FFFFFF"
                    }

                    Text {
                        text: "⏱ " + timeRemaining + "s"
                        font.family: "Lexend"
                        font.pixelSize: 18
                        font.bold: true
                        color: timeRemaining <= 5 ? "#f44336" : "#FFFFFF"
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 150
                    radius: 15
                    color: "white"

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 15
                        visible: waitingForNextQuestion

                        Item {
                            Layout.alignment: Qt.AlignHCenter
                            width: 50
                            height: 50

                            Rectangle {
                                id: spinnerCircle
                                anchors.centerIn: parent
                                width: 40
                                height: 40
                                radius: 20
                                color: "transparent"
                                border.color: "#667eea"
                                border.width: 3

                                Rectangle {
                                    anchors.top: parent.top
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    width: 8
                                    height: 8
                                    radius: 4
                                    color: "#667eea"
                                }
                            }

                            RotationAnimation {
                                target: spinnerCircle
                                running: waitingForNextQuestion
                                from: 0
                                to: 360
                                duration: 1000
                                loops: Animation.Infinite
                            }
                        }

                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: "Đang chờ câu hỏi tiếp theo..."
                            font.family: "Lexend"
                            font.pixelSize: 16
                            color: "#667eea"
                        }
                    }

                    ScrollView {
                        anchors.fill: parent
                        anchors.margins: 20
                        clip: true
                        visible: !waitingForNextQuestion

                        Text {
                            width: parent.width
                            text: questionContent
                            font.family: "Lexend"
                            font.pixelSize: 18
                            font.bold: true
                            color: "#333333"
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }

                GridLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    columns: 2
                    columnSpacing: 15
                    rowSpacing: 15

                    GameOptionButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 80
                        optionText: waitingForNextQuestion ? "" : ("A: " + optionA)
                        optionLetter: "A"
                        enabled: !waitingForAnswer && !eliminated && !waitingForNextQuestion && optionA.length > 0
                        forceWhiteWhenDisabled: waitingForNextQuestion
                        onClicked: handleGameAnswer("A")
                    }

                    GameOptionButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 80
                        optionText: waitingForNextQuestion ? "" : ("B: " + optionB)
                        optionLetter: "B"
                        enabled: !waitingForAnswer && !eliminated && !waitingForNextQuestion && optionB.length > 0
                        forceWhiteWhenDisabled: waitingForNextQuestion
                        onClicked: handleGameAnswer("B")
                    }

                    GameOptionButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 80
                        optionText: waitingForNextQuestion ? "" : ("C: " + optionC)
                        optionLetter: "C"
                        enabled: !waitingForAnswer && !eliminated && !waitingForNextQuestion && optionC.length > 0
                        forceWhiteWhenDisabled: waitingForNextQuestion
                        onClicked: handleGameAnswer("C")
                    }

                    GameOptionButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 80
                        optionText: waitingForNextQuestion ? "" : ("D: " + optionD)
                        optionLetter: "D"
                        enabled: !waitingForAnswer && !eliminated && !waitingForNextQuestion && optionD.length > 0
                        forceWhiteWhenDisabled: waitingForNextQuestion
                        onClicked: handleGameAnswer("D")
                    }
                }
            }

            Rectangle {
                Layout.preferredWidth: 250
                Layout.fillHeight: true
                radius: 15
                color: Qt.rgba(1.0, 1.0, 1.0, 0.95)

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 10

                    Text {
                        text: "Bảng xếp hạng"
                        font.family: "Lexend"
                        font.pixelSize: 16
                        font.bold: true
                        color: "#333333"
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        id: leaderboardList
                        model: membersList.length

                        delegate: Rectangle {
                            width: parent.width
                            height: 50

                            property var member: membersList[index] || {}
                            property bool isEliminated: member.eliminated || false
                            property bool isMe: member.userId === networkClient.getUserId()

                            color: isEliminated ? "#FFEBEE" : (isMe ? "#C8E6C9" : "#F5F5F5")

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 10

                                Text {
                                    text: "#" + (index + 1)
                                    font.family: "Lexend"
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: isEliminated ? "#D32F2F" : "#333333"
                                    font.strikeout: isEliminated
                                }

                                Text {
                                    Layout.fillWidth: true
                                    text: member.username || ("Người chơi " + (index + 1))
                                    font.family: "Lexend"
                                    font.pixelSize: 14
                                    color: isEliminated ? "#D32F2F" : "#333333"
                                    font.strikeout: isEliminated
                                    elide: Text.ElideRight
                                }

                                Text {
                                    text: (member.score || 0) + " điểm"
                                    font.family: "Lexend"
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: isEliminated ? "#D32F2F" : "#4CAF50"
                                    font.strikeout: isEliminated
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // ---------------------------
    // Game Over Screen (giữ nguyên)
    // ---------------------------
    Component {
        id: gameOverScreen

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 40
            spacing: 20

            Text {
                Layout.fillWidth: true
                text: "KẾT THÚC GAME"
                font.family: "Lexend"
                font.pixelSize: 28
                font.bold: true
                color: "#FFFFFF"
                horizontalAlignment: Text.AlignHCenter
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 15
                color: Qt.rgba(1.0, 1.0, 1.0, 0.95)

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 10

                    Text {
                        text: "Bảng xếp hạng cuối cùng:"
                        font.family: "Lexend"
                        font.pixelSize: 18
                        font.bold: true
                        color: "#333333"
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        id: finalLeaderboardList
                        model: membersList.length

                        delegate: Rectangle {
                            width: parent.width
                            height: 60

                            property var member: membersList[index] || {}

                            color: index === 0 ? "#FFD700" : (index % 2 === 0 ? "#F5F5F5" : "#FFFFFF")

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 15

                                Text {
                                    text: index === 0 ? "🥇" : (index === 1 ? "🥈" : (index === 2 ? "🥉" : "#" + (index + 1)))
                                    font.pixelSize: 20
                                }

                                Text {
                                    Layout.fillWidth: true
                                    text: member.username || ("Người chơi " + (index + 1))
                                    font.family: "Lexend"
                                    font.pixelSize: 16
                                    font.bold: index === 0
                                    color: "#333333"
                                    elide: Text.ElideRight
                                }

                                Text {
                                    text: (member.score || 0) + " điểm"
                                    font.family: "Lexend"
                                    font.pixelSize: 16
                                    font.bold: true
                                    color: "#4CAF50"
                                }
                            }
                        }
                    }
                }
            }

            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 60
                text: "Về menu chính"
                font.family: "Lexend"
                font.pixelSize: 16
                font.bold: true

                background: Rectangle {
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#667eea" }
                        GradientStop { position: 1.0; color: "#764ba2" }
                    }
                    radius: 12
                }

                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "#FFFFFF"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    if (stackView) {
                        stackView.replace("HomeScreen.qml", {
                            "stackView": stackView,
                            "username": username
                        })
                    }
                }
            }
        }
    }

    // Toast
    ToastMessage { id: toastMessage; anchors.fill: parent }

    // Timer for question countdown
    Timer {
        id: questionTimer
        interval: 1000
        running: false
        repeat: true
        onTriggered: {
            if (timeRemaining > 0) {
                timeRemaining--
            } else {
                questionTimer.stop()
                if (sessionId > 0 && currentRound > 0 && !waitingForAnswer) {
                    networkClient.sendSubmitAnswer1VN(sessionId, currentRound, "", 0)
                }
            }
        }
    }

    // Timer to close score message
    Timer {
        id: scoreMessageTimer
        interval: 2000
        running: false
        repeat: false
        onTriggered: {
            showingScoreMessage = false
            if (queuedQuestion) {
                var q = queuedQuestion
                queuedQuestion = null
                waitingForNextQuestion = false
                showQuestion(q.round, q.rounds, q.diff, q.questionId, q.content, q.options, q.timeLimit)
            } else {
                waitingForNextQuestion = true
            }
        }
    }

    // Connect to NetworkClient signals
    Connections {
        target: networkClient

        function onOneVNRoomCreated(newRoomId) {
            roomId = newRoomId
            isOwner = true
            ownerId = networkClient.getUserId()
            if (screenStack.depth === 0) screenStack.push(waitingRoomScreen)
            else screenStack.replace(waitingRoomScreen)
            toastMessage.show("Đã tạo phòng: " + roomId, "#4CAF50")
        }

        function onOneVNRoomJoined(success, newRoomId, error) {
            if (success) {
                roomId = newRoomId
                if (screenStack.depth === 0) screenStack.push(waitingRoomScreen)
                else screenStack.replace(waitingRoomScreen)
                toastMessage.show("Đã tham gia phòng", "#4CAF50")
            } else {
                toastMessage.show("Không thể tham gia phòng: " + error, "#FF5252")
            }
        }

        function onOneVNRoomUpdate(members) {
            var existingScores = {}
            for (var j = 0; j < membersList.length; j++) {
                var existing = membersList[j]
                if (existing.userId && existing.score > 0) existingScores[existing.userId] = existing.score
            }

            var tempList = []
            var tempMap = {}

            for (var i = 0; i < members.length; i++) {
                var member = members[i]
                var userId = member.user_id || member.userId || 0
                var uname = member.username || member.nickname || ""
                var score = member.score || 0
                var elim = member.eliminated === true || member.eliminated === "true"

                if (score === 0 && existingScores[userId]) score = existingScores[userId]

                if (uname && userId) tempMap[userId] = uname
                if (i === 0 && ownerId === 0) ownerId = userId

                tempList.push({
                    userId: userId,
                    username: uname || userIdToUsername[userId] || "",
                    score: score,
                    eliminated: elim
                })
            }

            for (var key in tempMap) userIdToUsername[key] = tempMap[key]

            tempList.sort(function(a, b) {
                if (a.eliminated !== b.eliminated) return a.eliminated ? 1 : -1
                return b.score - a.score
            })
            membersList = tempList

            if (isJoiningRoom && screenStack.depth === 0 && membersList.length > 0) {
                screenStack.push(waitingRoomScreen)
            }
        }

        function onOneVNRoomClosed(roomIdClosed, reason) {
            toastMessage.show(reason === "owner_left" ? "Chủ phòng đã rời. Phòng đã đóng." : "Phòng đã đóng.", "#FF9800")

            roomId = 0
            ownerId = 0
            isOwner = false
            membersList = []
            userIdToUsername = {}

            if (stackView) stackView.pop(null)
        }

        function onOneVNGameStart1VN(gameSessionId, gameRoomId, rounds) {
            sessionId = gameSessionId
            roomId = gameRoomId
            totalRounds = rounds
            currentRound = 0
            myScore = 0
            eliminated = false
            waitingForAnswer = false
            showingScoreMessage = false
            queuedQuestion = null
            waitingForNextQuestion = false
            
            // [RECONNECT] Mark that we are now in-game for reconnect tracking
            networkClient.setInGameState(gameRoomId, true)
            console.log("[RECONNECT] Game started, setInGameState(" + gameRoomId + ", true)")
            
            screenStack.replace(gamePlayingScreen)
        }

        function onOneVNQuestion1VNReceived(round, rounds, diff, questionId, content, options, timeLimit) {
            waitingForNextQuestion = false

            if (showingScoreMessage) {
                queuedQuestion = {
                    round: round,
                    rounds: rounds,
                    diff: diff,
                    questionId: questionId,
                    content: content,
                    options: options,
                    timeLimit: timeLimit
                }
            } else {
                showQuestion(round, rounds, diff, questionId, content, options, timeLimit)
            }
        }

        function onOneVNAnswerResult1VN(correct, score, totalScore, isEliminated, timeout) {
            questionTimer.stop()
            waitingForAnswer = false
            myScore = totalScore
            eliminated = isEliminated

            var myUserId = networkClient.getUserId()
            for (var i = 0; i < membersList.length; i++) {
                if (membersList[i].userId === myUserId) {
                    membersList[i].score = totalScore
                    membersList[i].eliminated = isEliminated
                    break
                }
            }
            membersList = membersList

            showingScoreMessage = true

            if (timeout) toastMessage.show("Hết thời gian!", "#FF5252")
            else if (correct) toastMessage.show("Đúng! +" + score + " điểm", "#4CAF50")
            else toastMessage.show("Trả lời sai! Không được điểm", "#FF5252")

            scoreMessageTimer.stop()
            scoreMessageTimer.start()
        }

        function onOneVNElimination(userId, round) {
            for (var i = 0; i < membersList.length; i++) {
                if (membersList[i].userId === userId) {
                    membersList[i].eliminated = true
                    break
                }
            }
            membersList = membersList
        }

        function onOneVNGameOver1VN(winnerId, leaderboard) {
            questionTimer.stop()
            
            // [RECONNECT] Game is over, no longer in-game
            networkClient.setInGameState(0, false)
            console.log("[RECONNECT] Game over, setInGameState(0, false)")
            
            membersList = []
            for (var i = 0; i < leaderboard.length; i++) {
                var player = leaderboard[i]
                var uid = player.user_id || player.userId || 0
                var uname = player.username || player.nickname || userIdToUsername[uid] || ""
                var sc = player.score || 0
                membersList.push({ userId: uid, username: uname, score: sc, eliminated: false })
            }
            membersList = membersList
            screenStack.replace(gameOverScreen)
        }

        function onErrorOccurred(error) {
            toastMessage.show("Lỗi: " + error, "#FF5252")
        }

        function onListFriendsResult(friends) {
            friendsModel.clear()
            for (var i = 0; i < friends.length; i++) {
                var friend = friends[i]
                var onlineStatus = friend.online_status || "offline"
                if (onlineStatus === "online") {
                    friendsModel.append({ userId: friend.user_id || 0, username: friend.username || "Unknown", online: true, inRoom: false })
                } else if (onlineStatus === "in_game") {
                    friendsModel.append({ userId: friend.user_id || 0, username: friend.username || "Unknown", online: false, inRoom: true })
                }
            }
        }
    }

    function showQuestion(round, rounds, diff, questionId, content, options, timeLimit) {
        showingScoreMessage = false
        queuedQuestion = null
        waitingForNextQuestion = false

        currentRound = round
        totalRounds = rounds
        difficulty = diff

        var optA = "", optB = "", optC = "", optD = ""
        if (options) {
            optA = options.A || options["A"] || ""
            optB = options.B || options["B"] || ""
            optC = options.C || options["C"] || ""
            optD = options.D || options["D"] || ""
        }

        questionContent = content || ""
        optionA = optA
        optionB = optB
        optionC = optC
        optionD = optD
        timeRemaining = timeLimit || 15
        waitingForAnswer = false
        selectedAnswer = ""

        scoreMessageTimer.stop()
        questionTimer.stop()
        questionTimer.start()
    }

    function handleGameAnswer(answer) {
        if (waitingForAnswer || eliminated || sessionId === 0) return
        selectedAnswer = answer
        waitingForAnswer = true
        questionTimer.stop()
        networkClient.sendSubmitAnswer1VN(sessionId, currentRound, answer, timeRemaining)
    }

    // Friends List Dialog for inviting
    Dialog {
        id: friendsListDialog
        title: "Chọn bạn bè để mời"
        modal: true
        anchors.centerIn: parent
        width: 450
        height: 550

        property int roomId: 0

        background: Rectangle {
            color: "#2a2a2a"
            radius: 10
            border.color: "#4a4a4a"
            border.width: 1
        }

        header: Rectangle {
            width: parent.width
            height: 50
            color: "#1e88e5"
            radius: 10

            Label {
                anchors.centerIn: parent
                text: "👥 Danh sách bạn bè"
                font.pixelSize: 18
                font.bold: true
                color: "white"
            }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 15

            Label { text: "Chọn bạn bè để mời vào phòng:"; font.pixelSize: 14; color: "#cccccc" }

            ListView {
                id: friendsListView
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: friendsModel

                delegate: Rectangle {
                    width: friendsListView.width
                    height: 60
                    color: index % 2 === 0 ? "#3a3a3a" : "#2a2a2a"
                    radius: 5

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 15

                        Rectangle {
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 40
                            radius: 20
                            color: "#667eea"
                            Label {
                                anchors.centerIn: parent
                                text: model.username ? model.username.charAt(0).toUpperCase() : "?"
                                font.pixelSize: 18
                                font.bold: true
                                color: "white"
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            Label { text: model.username || "Unknown"; font.pixelSize: 14; font.bold: true; color: "white" }
                            Label {
                                text: model.inRoom ? "🎮 In-game" : (model.online ? "● Online" : "○ Offline")
                                font.pixelSize: 12
                                color: model.inRoom ? "#FF9800" : (model.online ? "#4CAF50" : "#888888")
                            }
                        }

                        Button {
                            Layout.preferredWidth: 80
                            Layout.preferredHeight: 35
                            text: "Mời"
                            enabled: model.online && !model.inRoom

                            background: Rectangle {
                                color: parent.enabled ? (parent.down ? "#1565c0" : "#1e88e5") : "#555555"
                                radius: 6
                            }

                            contentItem: Text {
                                text: parent.text
                                font.pixelSize: 12
                                font.bold: true
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }

                            onClicked: {
                                networkClient.sendInviteFriend(friendsListDialog.roomId, model.userId)
                                toastMessage.show("Đã gửi lời mời đến " + model.username, "#4CAF50")
                                friendsListDialog.close()
                            }
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar { active: true }
            }

            Button {
                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: 120
                text: "Đóng"

                background: Rectangle { color: parent.down ? "#c62828" : "#f44336"; radius: 6 }

                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 12
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: friendsListDialog.close()
            }
        }
    }

    // Exit Confirmation Dialog
    Dialog {
        id: exitConfirmDialog
        title: "Xác nhận thoát"
        modal: true
        anchors.centerIn: parent
        width: 400
        standardButtons: Dialog.Yes | Dialog.No

        Label {
            text: "Bạn có chắc muốn thoát khỏi trận đấu?\n\nBạn sẽ bị loại và tính là thua."
            font.family: "Lexend"
            font.pixelSize: 14
            wrapMode: Text.WordWrap
            width: parent.width
        }

        onAccepted: {
            toastMessage.show("Đang rời khỏi trận đấu...", "#FF9800")
            
            // [RECONNECT] Leaving room, no longer in-game
            networkClient.setInGameState(0, false)
            console.log("[RECONNECT] Left room, setInGameState(0, false)")
            
            networkClient.sendLeaveRoom(roomId)

            questionTimer.stop()
            scoreMessageTimer.stop()

            sessionId = 0
            roomId = 0
            ownerId = 0
            isOwner = false
            eliminated = true
            membersList = []
            userIdToUsername = {}

            if (stackView) stackView.pop(null)
        }
    }

    // Room chat received
    Connections {
        target: networkClient
        function onRoomChatReceived(userId, uname, message, timestamp) {
            chatMessages.append({ "sender": uname, "message": message })
        }
    }
}
