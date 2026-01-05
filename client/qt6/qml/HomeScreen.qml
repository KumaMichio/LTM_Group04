import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0

Item {
    id: homeScreen
    width: parent ? parent.width : 360
    height: parent ? parent.height : 640

    property StackView stackView
    property string username: ""
    property int userId: 0

    // Unread messages tracking
    property bool hasUnreadMessages: false

    // Avatar data
    property string avatarPath: ""
    property string avatarSource: {
        if (avatarPath && avatarPath !== "") {
            var imgPath = avatarPath
            if (!imgPath.startsWith("file://") && !imgPath.startsWith("http://") && !imgPath.startsWith("https://") && !imgPath.startsWith("qrc://")) {
                imgPath = imgPath.replace(/\\/g, "/")
                if (imgPath.length > 1 && imgPath[1] === ':') {
                    imgPath = "file:///" + imgPath
                } else if (imgPath.length > 0 && imgPath[0] === '/') {
                    imgPath = "file://" + imgPath
                } else {
                    imgPath = "file:///" + imgPath
                }
            }
            return imgPath
        }
        return ""
    }

    // ===== Responsive tuning (chỉ cần chỉnh các giá trị này nếu muốn) =====
    // scale theo chiều rộng/chiều cao, có clamp để không quá nhỏ/quá to
    readonly property real uiScale: {
        var s = Math.min(width / 360.0, height / 640.0)
        if (s < 0.85) s = 0.85
        if (s > 1.25) s = 1.25
        return s
    }

    readonly property int pagePad: Math.round(24 * uiScale)
    readonly property int topBarH: Math.round(56 * uiScale)
    readonly property int bottomNavH: Math.round(64 * uiScale)

    readonly property int cardRadius: Math.round(16 * uiScale)
    readonly property int cardPad: Math.round(20 * uiScale)
    readonly property int gap: Math.round(16 * uiScale)

    readonly property int titleSize: Math.round(20 * uiScale)
    readonly property int descSize: Math.round(14 * uiScale)
    readonly property int btnTextSize: Math.round(16 * uiScale)
    readonly property int btnH: Math.round(48 * uiScale)

    // tránh card quá rộng trên màn hình rất lớn → canh giữa
    readonly property int maxContentWidth: 520   // bạn có thể tăng/giảm

    // Background color
    Rectangle {
        anchors.fill: parent
        color: "#2E1A47"
    }

    ColumnLayout {
        id: mainColumn
        anchors.fill: parent
        anchors.leftMargin: pagePad
        anchors.rightMargin: pagePad
        anchors.topMargin: pagePad
        anchors.bottomMargin: pagePad
        spacing: 0

        // ===================== Top Bar =====================
        Rectangle {
            id: topBar
            Layout.fillWidth: true
            Layout.preferredHeight: topBarH
            color: "transparent"

            RowLayout {
                anchors.fill: parent
                spacing: Math.round(12 * uiScale)

                // Avatar
                Rectangle {
                    Layout.preferredWidth: Math.round(36 * uiScale)
                    Layout.preferredHeight: Math.round(36 * uiScale)
                    radius: Math.round(18 * uiScale)
                    color: "#5D4586"
                    clip: true

                    Image {
                        anchors.fill: parent
                        source: avatarSource
                        fillMode: Image.PreserveAspectCrop
                        visible: avatarSource !== ""
                    }

                    Text {
                        anchors.centerIn: parent
                        text: username.length > 0 ? username.charAt(0).toUpperCase() : "U"
                        font.family: "Lexend"
                        font.pixelSize: Math.round(18 * uiScale)
                        font.bold: true
                        color: "#FFFFFF"
                        visible: avatarSource === ""
                    }
                }

                // Username only (đã bỏ pts/rank)
                Text {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter
                    text: username
                    font.family: "Lexend"
                    font.pixelSize: Math.round(14 * uiScale)
                    font.bold: true
                    color: "#FFFFFF"
                    elide: Text.ElideRight
                }

                // ✅ Bỏ icon chuông noti
                // Logout icon (giữ)
                Rectangle {
                    Layout.preferredWidth: Math.round(40 * uiScale)
                    Layout.preferredHeight: Math.round(40 * uiScale)
                    color: "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: "🚪"
                        font.pixelSize: Math.round(20 * uiScale)
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (networkClient.isLoggedIn()) {
                                networkClient.sendLogout()
                            }
                        }
                    }
                }
            }
        }

        // Spacing after top bar
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: Math.round(24 * uiScale)
        }

        // ===================== Content Area =====================
        Item {
            id: contentArea
            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollView {
                anchors.fill: parent
                clip: true

                // Wrap để giới hạn max width và canh giữa
                Item {
                    width: Math.min(parent.width, maxContentWidth)
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: cardsColumn.implicitHeight

                    Column {
                        id: cardsColumn
                        width: parent.width
                        spacing: gap

                        // ---------- Card 1: Quick Mode ----------
                        Rectangle {
                            id: quickModeCard
                            width: parent.width
                            radius: cardRadius
                            color: "#3D2B56"
                            border.color: "#5D4586"
                            border.width: 1

                            // auto height theo content, có min để nhìn đẹp
                            implicitHeight: Math.max( Math.round(168 * uiScale), quickCol.implicitHeight + cardPad * 2 )

                            Column {
                                id: quickCol
                                anchors.fill: parent
                                anchors.margins: cardPad
                                spacing: Math.round(12 * uiScale)

                                // ✅ Bỏ icon trước chữ
                                Text {
                                    text: "Quick Mode"
                                    font.family: "Lexend"
                                    font.pixelSize: titleSize
                                    font.bold: true
                                    color: "#FFFFFF"
                                    elide: Text.ElideRight
                                }

                                Text {
                                    width: parent.width
                                    text: "Chế độ nhanh - 15 câu hỏi"
                                    font.family: "Lexend"
                                    font.pixelSize: descSize
                                    color: "#B0B0B0"
                                    wrapMode: Text.WordWrap
                                }

                                Item { width: parent.width; height: 1 }

                                Button {
                                    width: parent.width
                                    height: btnH
                                    text: "Chơi ngay"
                                    font.family: "Lexend"
                                    font.pixelSize: btnTextSize
                                    font.bold: true

                                    background: Rectangle {
                                        color: "#FFC107"
                                        radius: Math.round(12 * uiScale)
                                    }

                                    contentItem: Text {
                                        text: parent.text
                                        font: parent.font
                                        color: "#000000"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                        elide: Text.ElideRight
                                    }

                                    onClicked: {
                                        if (stackView) {
                                            stackView.push("QuickModeGame.qml", {
                                                "stackView": stackView,
                                                "username": username
                                            })
                                        }
                                    }
                                }
                            }
                        }

                        // ---------- Card 2: 1vN ----------
                        Rectangle {
                            id: oneVNCard
                            width: parent.width
                            radius: cardRadius
                            color: "#3D2B56"
                            border.color: "#5D4586"
                            border.width: 1

                            // auto height theo content
                            implicitHeight: Math.max( Math.round(168 * uiScale), oneVNCol.implicitHeight + cardPad * 2 )

                            // nếu card hẹp, đổi 2 nút sang dọc để không bị Th.../Ta...
                            readonly property bool compactButtons: width < 360

                            Column {
                                id: oneVNCol
                                anchors.fill: parent
                                anchors.margins: cardPad
                                spacing: Math.round(12 * uiScale)

                                // ✅ Bỏ icon trước chữ
                                Text {
                                    text: "1vN"
                                    font.family: "Lexend"
                                    font.pixelSize: titleSize
                                    font.bold: true
                                    color: "#FFFFFF"
                                    elide: Text.ElideRight
                                }

                                Text {
                                    width: parent.width
                                    text: "Chế độ đối kháng"
                                    font.family: "Lexend"
                                    font.pixelSize: descSize
                                    color: "#B0B0B0"
                                    wrapMode: Text.WordWrap
                                }

                                Item { width: parent.width; height: 1 }

                                Loader {
                                    width: parent.width
                                    sourceComponent: oneVNCard.compactButtons ? oneVNButtonsColumn : oneVNButtonsRow
                                }
                            }
                        }

                        // Đệm dưới để không bị bottom nav che khi scroll
                        Item {
                            width: parent.width
                            height: Math.round(8 * uiScale)
                        }
                    }
                }
            }
        }

        // ===================== Bottom Navigation =====================
        Rectangle {
            id: bottomNav
            Layout.fillWidth: true
            Layout.preferredHeight: bottomNavH
            color: "#3D2B56"

            Row {
                anchors.fill: parent
                anchors.margins: Math.round(8 * uiScale)

                // Home tab (active)
                Rectangle {
                    width: parent.width / 4
                    height: parent.height
                    color: "transparent"

                    Column {
                        anchors.centerIn: parent
                        spacing: Math.round(4 * uiScale)

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "🏠"
                            font.pixelSize: Math.round(24 * uiScale)
                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "Home"
                            font.family: "Lexend"
                            font.pixelSize: Math.round(11 * uiScale)
                            color: "#FFC107"
                        }
                    }
                }

                // Friends tab
                Rectangle {
                    width: parent.width / 4
                    height: parent.height
                    color: "transparent"

                    Column {
                        anchors.centerIn: parent
                        spacing: Math.round(4 * uiScale)

                        Item {
                            width: Math.round(24 * uiScale)
                            height: Math.round(24 * uiScale)
                            anchors.horizontalCenter: parent.horizontalCenter

                            Text {
                                anchors.centerIn: parent
                                text: "👥"
                                font.pixelSize: Math.round(24 * uiScale)
                            }

                            Rectangle {
                                anchors.right: parent.right
                                anchors.top: parent.top
                                anchors.rightMargin: Math.round(-4 * uiScale)
                                anchors.topMargin: Math.round(-4 * uiScale)
                                width: Math.round(12 * uiScale)
                                height: Math.round(12 * uiScale)
                                radius: Math.round(6 * uiScale)
                                color: "#FF5252"
                                visible: hasUnreadMessages

                                Text {
                                    anchors.centerIn: parent
                                    text: "!"
                                    font.family: "Lexend"
                                    font.pixelSize: Math.round(8 * uiScale)
                                    font.bold: true
                                    color: "#FFFFFF"
                                }
                            }
                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "Friends"
                            font.family: "Lexend"
                            font.pixelSize: Math.round(11 * uiScale)
                            color: "#B0B0B0"
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            hasUnreadMessages = false
                            if (stackView) {
                                stackView.push("FriendsList.qml", {
                                    "stackView": stackView,
                                    "username": username
                                })
                            }
                        }
                    }
                }

                // Leaderboard tab
                Rectangle {
                    width: parent.width / 4
                    height: parent.height
                    color: "transparent"

                    Column {
                        anchors.centerIn: parent
                        spacing: Math.round(4 * uiScale)

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "🏆"
                            font.pixelSize: Math.round(24 * uiScale)
                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "BXH"
                            font.family: "Lexend"
                            font.pixelSize: Math.round(11 * uiScale)
                            color: "#B0B0B0"
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (stackView) {
                                stackView.push("LeaderboardScreen.qml", {
                                    "stackView": stackView,
                                    "username": username
                                })
                            }
                        }
                    }
                }

                // Profile tab
                Rectangle {
                    width: parent.width / 4
                    height: parent.height
                    color: "transparent"

                    Column {
                        anchors.centerIn: parent
                        spacing: Math.round(4 * uiScale)

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "👤"
                            font.pixelSize: Math.round(24 * uiScale)
                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "Profile"
                            font.family: "Lexend"
                            font.pixelSize: Math.round(11 * uiScale)
                            color: "#B0B0B0"
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (stackView) {
                                stackView.push("ProfileScreen.qml", {
                                    "stackView": stackView,
                                    "username": username,
                                    "userId": userId
                                })
                            }
                        }
                    }
                }
            }
        }
    }

    // ===================== Components for 1vN buttons =====================
    Component {
        id: oneVNButtonsRow
        Row {
            width: parent.width
            height: btnH
            spacing: Math.round(12 * uiScale)

            Button {
                width: (parent.width - parent.spacing) / 2
                height: btnH
                text: "Tham gia"
                font.family: "Lexend"
                font.pixelSize: btnTextSize
                font.bold: true

                background: Rectangle {
                    color: "#3D2B56"
                    border.color: "#5D4586"
                    border.width: 1
                    radius: Math.round(12 * uiScale)
                }

                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "#FFFFFF"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                onClicked: {
                    if (stackView) {
                        stackView.push("OneVNRoomListScreen.qml", {
                            "stackView": stackView,
                            "username": username
                        })
                    }
                }
            }

            Button {
                width: (parent.width - parent.spacing) / 2
                height: btnH
                text: "Tạo phòng"
                font.family: "Lexend"
                font.pixelSize: btnTextSize
                font.bold: true

                background: Rectangle {
                    color: "#FFC107"
                    radius: Math.round(12 * uiScale)
                }

                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "#000000"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                onClicked: {
                    if (stackView) {
                        stackView.push("OneVNMode.qml", {
                            "stackView": stackView,
                            "username": username
                        })
                    }
                }
            }
        }
    }

    Component {
        id: oneVNButtonsColumn
        Column {
            width: parent.width
            spacing: Math.round(10 * uiScale)

            Button {
                width: parent.width
                height: btnH
                text: "Tham gia"
                font.family: "Lexend"
                font.pixelSize: btnTextSize
                font.bold: true

                background: Rectangle {
                    color: "#3D2B56"
                    border.color: "#5D4586"
                    border.width: 1
                    radius: Math.round(12 * uiScale)
                }

                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "#FFFFFF"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                onClicked: {
                    if (stackView) {
                        stackView.push("OneVNRoomListScreen.qml", {
                            "stackView": stackView,
                            "username": username
                        })
                    }
                }
            }

            Button {
                width: parent.width
                height: btnH
                text: "Tạo phòng"
                font.family: "Lexend"
                font.pixelSize: btnTextSize
                font.bold: true

                background: Rectangle {
                    color: "#FFC107"
                    radius: Math.round(12 * uiScale)
                }

                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "#000000"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                onClicked: {
                    if (stackView) {
                        stackView.push("OneVNMode.qml", {
                            "stackView": stackView,
                            "username": username
                        })
                    }
                }
            }
        }
    }

    // ===================== Network hooks =====================
    Connections {
        target: networkClient

        function onProfileReceived(profile) {
            if (profile.avatar_img && profile.avatar_img !== "") {
                avatarPath = profile.avatar_img
            }
        }

        function onDmReceived(fromUserId, fromUsername, message, timestamp) {
            hasUnreadMessages = true
        }

        function onLogoutResponse(success) {
            if (success && stackView) {
                stackView.clear()
                stackView.push("Signin.qml", {"stackView": stackView})
            }
        }
    }

    Component.onCompleted: {
        if (userId > 0) {
            networkClient.sendGetProfile()
        }
    }
}
