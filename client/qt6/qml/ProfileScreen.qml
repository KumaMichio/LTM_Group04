import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0

Item {
    id: profileScreen
    width: parent ? parent.width : 360
    height: parent ? parent.height : 640

    property StackView stackView
    property string username: ""
    property int userId: 0

    // ===== Responsive helpers =====
    function clamp(v, lo, hi) { return Math.max(lo, Math.min(hi, v)); }

    property int contentMaxWidth: 640
    property int sidePadding: clamp(Math.round(width * 0.04), 14, 28)
    property int cardWidth: Math.min(width - sidePadding * 2, contentMaxWidth)

    // Profile data from server
    property var profileData: ({
        "user_id": 0,
        "username": "",
        "avatar_img": "",
        "quickmode_games": 0,
        "onevn_games": 0,
        "quickmode_wins": 0,
        "onevn_wins": 0
    })

    property bool isLoading: false
    property string errorMessage: ""

    property string avatarSource: {
        if (profileData.avatar_img && profileData.avatar_img !== "") {
            var imgPath = profileData.avatar_img
            if (!imgPath.startsWith("file://") && !imgPath.startsWith("http://") &&
                !imgPath.startsWith("https://") && !imgPath.startsWith("qrc://")) {
                imgPath = imgPath.replace(/\\/g, "/")
                if (imgPath.length > 1 && imgPath[1] === ':') imgPath = "file:///" + imgPath
                else if (imgPath.length > 0 && imgPath[0] === '/') imgPath = "file://" + imgPath
                else imgPath = "file:///" + imgPath
            }
            return imgPath
        }
        return ""
    }

    Rectangle {
        anchors.fill: parent
        color: "#2E1A47"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: sidePadding
        anchors.rightMargin: sidePadding
        anchors.topMargin: sidePadding
        anchors.bottomMargin: sidePadding
        spacing: 0

        // ===== Top bar =====
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            spacing: 16

            Rectangle {
                Layout.preferredWidth: 40
                Layout.preferredHeight: 40
                color: "transparent"

                Image { 
                    anchors.centerIn: parent
                    source: "qrc:/icons/arrow-left.svg"
                    width: 24
                    height: 24
                    sourceSize: Qt.size(24, 24)
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: { if (stackView) stackView.pop() }
                }
            }

            Text {
                Layout.fillWidth: true
                text: "Hồ Sơ"
                font.family: "Lexend"
                font.pixelSize: 20
                font.bold: true
                color: "#FFFFFF"
                elide: Text.ElideRight
            }

            Rectangle {
                Layout.preferredWidth: 40
                Layout.preferredHeight: 40
                color: "transparent"

                Image {
                    anchors.centerIn: parent
                    source: "qrc:/icons/refresh-cw.svg"
                    width: 24
                    height: 24
                    sourceSize: Qt.size(24, 24)
                    opacity: isLoading ? 0.5 : 1.0
                }

                MouseArea {
                    anchors.fill: parent
                    enabled: !isLoading
                    onClicked: {
                        isLoading = true
                        errorMessage = ""
                        networkClient.sendGetProfile()
                    }
                }
            }
        }

        Item { Layout.fillWidth: true; Layout.preferredHeight: 18 }

        // ===== Loading =====
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "transparent"
            visible: isLoading && !errorMessage

            Column {
                anchors.centerIn: parent
                spacing: 16
                Text { anchors.horizontalCenter: parent.horizontalCenter; text: "⏳"; font.pixelSize: 48 }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Đang tải..."
                    font.family: "Lexend"
                    font.pixelSize: 16
                    color: "#B0B0B0"
                }
            }
        }

        // ===== Error =====
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "transparent"
            visible: errorMessage !== ""

            Column {
                anchors.centerIn: parent
                spacing: 16

                Text { anchors.horizontalCenter: parent.horizontalCenter; text: "❌"; font.pixelSize: 48 }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: errorMessage
                    font.family: "Lexend"
                    font.pixelSize: 16
                    color: "#FF6B6B"
                    width: Math.min(parent.parent.width - 48, contentMaxWidth)
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }

                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 120
                    height: 40
                    radius: 20
                    color: "#5D4586"

                    Text {
                        anchors.centerIn: parent
                        text: "Thử lại"
                        font.family: "Lexend"
                        font.pixelSize: 14
                        color: "#FFFFFF"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            isLoading = true
                            errorMessage = ""
                            networkClient.sendGetProfile()
                        }
                    }
                }
            }
        }

        // ===== Content =====
        ScrollView {
            id: profileScroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            visible: !isLoading && errorMessage === ""

            // IMPORTANT: dùng availableWidth thay vì parent.width
            contentWidth: profileScroll.availableWidth

            Column {
                id: scrollColumn
                width: profileScroll.availableWidth
                spacing: 20

                // Centered container (maxWidth)
                Item {
                    width: profileScroll.availableWidth
                    height: centeredColumn.implicitHeight

                    Column {
                        id: centeredColumn
                        width: Math.min(profileScroll.availableWidth, cardWidth)
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 20

                        // ===== Avatar card =====
                        Rectangle {
                            width: parent.width
                            radius: 16
                            color: "#3D2B56"
                            border.color: "#5D4586"
                            border.width: 1
                            implicitHeight: avatarColumn.implicitHeight + 28

                            Column {
                                id: avatarColumn
                                anchors.centerIn: parent
                                spacing: 12

                                Rectangle {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    width: clamp(Math.round(parent.width * 0.16), 64, 92)
                                    height: width
                                    radius: width / 2
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
                                        text: profileData.username ? profileData.username.charAt(0).toUpperCase() : "U"
                                        font.family: "Lexend"
                                        font.pixelSize: Math.round(parent.width * 0.45)
                                        font.bold: true
                                        color: "#FFFFFF"
                                        visible: avatarSource === ""
                                    }

                                    Rectangle {
                                        anchors.bottom: parent.bottom
                                        anchors.right: parent.right
                                        width: 24
                                        height: 24
                                        radius: 12
                                        color: "#FFC107"
                                        border.color: "#FFFFFF"
                                        border.width: 2

                                        Text { anchors.centerIn: parent; text: "✏"; font.pixelSize: 14; color: "#FFFFFF" }
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            var filePath = fileDialogHelper.openImageFile()
                                            if (filePath !== "") {
                                                var urlPath = filePath
                                                if (!urlPath.startsWith("file://")) {
                                                    urlPath = "file:///" + urlPath.replace(/\\/g, "/")
                                                }
                                                profileData.avatar_img = filePath
                                                avatarSource = urlPath
                                                networkClient.sendUpdateAvatar(filePath)
                                            }
                                        }
                                    }
                                }

                                Text {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: profileData.username || username || "User"
                                    font.family: "Lexend"
                                    font.pixelSize: 20
                                    font.bold: true
                                    color: "#FFFFFF"
                                }
                            }
                        }

                        // ===== Statistics card =====
                        Rectangle {
                            width: parent.width
                            radius: 16
                            color: "#3D2B56"
                            border.color: "#5D4586"
                            border.width: 1
                            implicitHeight: statsColumn.implicitHeight + 40

                            Column {
                                id: statsColumn
                                width: parent.width - 40
                                anchors.left: parent.left
                                anchors.leftMargin: 20
                                anchors.top: parent.top
                                anchors.topMargin: 20
                                spacing: 16

                                Text {
                                    text: "Thống Kê"
                                    font.family: "Lexend"
                                    font.pixelSize: 18
                                    font.bold: true
                                    color: "#FFFFFF"
                                }

                                // QuickMode
                                Rectangle {
                                    width: parent.width
                                    radius: 12
                                    color: "#2E1A47"
                                    implicitHeight: quickBlock.implicitHeight + 32

                                    Column {
                                        id: quickBlock
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.margins: 16
                                        spacing: 8

                                        Text {
                                            text: "Quick Mode"
                                            font.family: "Lexend"
                                            font.pixelSize: 16
                                            font.bold: true
                                            color: "#FFC107"
                                        }

                                        RowLayout {
                                            width: parent.width
                                            spacing: 16

                                            Column {
                                                Layout.fillWidth: true
                                                spacing: 4
                                                Text { text: "Số lần chơi"; font.family: "Lexend"; font.pixelSize: 12; color: "#B0B0B0" }
                                                Text {
                                                    text: (profileData.quickmode_games || 0).toString()
                                                    font.family: "Lexend"
                                                    font.pixelSize: 24
                                                    font.bold: true
                                                    color: "#FFFFFF"
                                                }
                                            }

                                            Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#5D4586" }

                                            Column {
                                                Layout.fillWidth: true
                                                spacing: 4
                                                Text { text: "Số lần thắng"; font.family: "Lexend"; font.pixelSize: 12; color: "#B0B0B0" }
                                                Text {
                                                    text: (profileData.quickmode_wins || 0).toString()
                                                    font.family: "Lexend"
                                                    font.pixelSize: 24
                                                    font.bold: true
                                                    color: "#4CAF50"
                                                }
                                            }

                                            Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#5D4586" }

                                            Column {
                                                Layout.fillWidth: true
                                                spacing: 4
                                                Text { text: "Tỷ lệ thắng"; font.family: "Lexend"; font.pixelSize: 12; color: "#B0B0B0" }
                                                Text {
                                                    text: {
                                                        var games = Number(profileData.quickmode_games || 0)
                                                        var wins = Number(profileData.quickmode_wins || 0)
                                                        if (games === 0) return "0%"
                                                        return Math.round((wins / games) * 100) + "%"
                                                    }
                                                    font.family: "Lexend"
                                                    font.pixelSize: 24
                                                    font.bold: true
                                                    color: "#FFC107"
                                                }
                                            }
                                        }
                                    }
                                }

                                // 1vN
                                Rectangle {
                                    width: parent.width
                                    radius: 12
                                    color: "#2E1A47"
                                    implicitHeight: onevnBlock.implicitHeight + 32

                                    Column {
                                        id: onevnBlock
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.margins: 16
                                        spacing: 8

                                        Text {
                                            text: "1vN Mode"
                                            font.family: "Lexend"
                                            font.pixelSize: 16
                                            font.bold: true
                                            color: "#FF6B6B"
                                        }

                                        RowLayout {
                                            width: parent.width
                                            spacing: 16

                                            Column {
                                                Layout.fillWidth: true
                                                spacing: 4
                                                Text { text: "Số lần chơi"; font.family: "Lexend"; font.pixelSize: 12; color: "#B0B0B0" }
                                                Text {
                                                    text: (profileData.onevn_games || 0).toString()
                                                    font.family: "Lexend"
                                                    font.pixelSize: 24
                                                    font.bold: true
                                                    color: "#FFFFFF"
                                                }
                                            }

                                            Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#5D4586" }

                                            Column {
                                                Layout.fillWidth: true
                                                spacing: 4
                                                Text { text: "Số lần thắng"; font.family: "Lexend"; font.pixelSize: 12; color: "#B0B0B0" }
                                                Text {
                                                    text: (profileData.onevn_wins || 0).toString()
                                                    font.family: "Lexend"
                                                    font.pixelSize: 24
                                                    font.bold: true
                                                    color: "#4CAF50"
                                                }
                                            }

                                            Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#5D4586" }

                                            Column {
                                                Layout.fillWidth: true
                                                spacing: 4
                                                Text { text: "Tỷ lệ thắng"; font.family: "Lexend"; font.pixelSize: 12; color: "#B0B0B0" }
                                                Text {
                                                    text: {
                                                        var games = Number(profileData.onevn_games || 0)
                                                        var wins = Number(profileData.onevn_wins || 0)
                                                        if (games === 0) return "0%"
                                                        return Math.round((wins / games) * 100) + "%"
                                                    }
                                                    font.family: "Lexend"
                                                    font.pixelSize: 24
                                                    font.bold: true
                                                    color: "#FF6B6B"
                                                }
                                            }
                                        }
                                    }
                                }

                                // Total
                                Rectangle {
                                    width: parent.width
                                    radius: 12
                                    color: "#2E1A47"
                                    implicitHeight: totalRow.implicitHeight + 24

                                    RowLayout {
                                        id: totalRow
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.margins: 12
                                        spacing: 16

                                        Column {
                                            Layout.fillWidth: true
                                            spacing: 2
                                            Text { text: "Tổng số trận"; font.family: "Lexend"; font.pixelSize: 11; color: "#B0B0B0" }
                                            Text {
                                                text: (Number(profileData.quickmode_games || 0) + Number(profileData.onevn_games || 0)).toString()
                                                font.family: "Lexend"
                                                font.pixelSize: 20
                                                font.bold: true
                                                color: "#FFFFFF"
                                            }
                                        }

                                        Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#5D4586" }

                                        Column {
                                            Layout.fillWidth: true
                                            spacing: 2
                                            Text { text: "Tổng số thắng"; font.family: "Lexend"; font.pixelSize: 11; color: "#B0B0B0" }
                                            Text {
                                                text: (Number(profileData.quickmode_wins || 0) + Number(profileData.onevn_wins || 0)).toString()
                                                font.family: "Lexend"
                                                font.pixelSize: 20
                                                font.bold: true
                                                color: "#4CAF50"
                                            }
                                        }
                                    }
                                }

                                // History button
                                Rectangle {
                                    width: parent.width
                                    height: 48
                                    radius: 12
                                    color: "#FFC107"

                                    Text {
                                        anchors.centerIn: parent
                                        text: "Xem Lịch Sử Game"
                                        font.family: "Lexend"
                                        font.pixelSize: 14
                                        font.bold: true
                                        color: "#2E1A47"
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            if (stackView) {
                                                stackView.push("OneVNHistoryScreen.qml", {
                                                    "stackView": stackView,
                                                    "userId": userId
                                                })
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        Item { width: 1; height: 12 }
                    }
                }
            }
        }
    }

    Connections {
        target: networkClient

        function onProfileReceived(profile) {
            isLoading = false
            errorMessage = ""
            profileData = profile
        }

        function onErrorOccurred(error) {
            isLoading = false
            errorMessage = error || "Không thể tải thông tin profile"
        }

        function onAvatarUpdated(success, message) {
            console.log("Avatar update result:", success, message)
        }
    }

    Component.onCompleted: {
        isLoading = true
        errorMessage = ""
        networkClient.sendGetProfile()
    }
}
