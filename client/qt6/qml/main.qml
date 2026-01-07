import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0
import QtQuick.Window 6.0

ApplicationWindow {
    id: root
    property real targetWidth: {
        if (screen) {
            var w = screen.width * 0.5
            if (w > 1100) w = 1100
            if (w < 950) w = 600
            return w
        }
        return 1000
    }
    property real targetHeight: screen ? screen.height * 0.7 : 700

    width: targetWidth
    height: targetHeight
    minimumWidth: 600
    minimumHeight: 700
    visible: true
    title: "Ai là triệu phú"
    
    // Style Guide: Background color
    color: "#2E1A47"
    
    // Reconnect state tracking
    property string savedServerHost: "localhost"
    property int savedServerPort: 9000
    property int reconnectCountdown: 30
    property bool isReconnecting: false
    
    // Timer for reconnect countdown
    Timer {
        id: reconnectTimer
        interval: 1000
        repeat: true
        onTriggered: {
            reconnectCountdown--
            if (reconnectCountdown <= 0) {
                reconnectTimer.stop()
                isReconnecting = false
                reconnectDialog.close()
                // Reconnect failed - go to login
                stackView.clear()
                stackView.push("Signin.qml", {"stackView": stackView})
            }
        }
    }
    
    // [HEARTBEAT] Handle disconnect event from server
    Connections {
        target: networkClient
        
        function onDisconnected() {
            console.log("[QML] Disconnected from server - showing dialog")
            disconnectDialog.open()
        }
        
        // [RECONNECT] Handle reconnect needed signal - triggered when disconnect during game
        function onReconnectNeeded(userId, accessToken, roomId) {
            console.log("[QML] Reconnect needed! userId=" + userId + " roomId=" + roomId)
            
            isReconnecting = true
            reconnectCountdown = 30
            reconnectDialog.close()
            reconnectingDialog.open()
            reconnectTimer.start()
            
            // Auto-connect to server and send reconnect request
            attemptReconnect()
        }
        
        // [RECONNECT] Handle reconnect response
        function onReconnectResponse(success, roomId, score, currentRound, timeRemaining, error) {
            console.log("[QML] Reconnect response: success=" + success + " roomId=" + roomId + " error=" + error)
            
            reconnectTimer.stop()
            isReconnecting = false
            reconnectingDialog.close()
            
            if (success) {
                console.log("[QML] Reconnect successful! Restoring game state...")
                // Find OneVNMode page in stack and update its state
                var oneVNPage = findOneVNModePage()
                if (oneVNPage) {
                    oneVNPage.myScore = score
                    oneVNPage.currentRound = currentRound
                    oneVNPage.timeRemaining = timeRemaining
                    console.log("[QML] Game state restored: score=" + score + " round=" + currentRound)
                }
                // Show success message
                reconnectSuccessDialog.open()
            } else {
                console.log("[QML] Reconnect failed: " + error)
                // Show error and go to login
                reconnectFailedError = error
                reconnectFailedDialog.open()
            }
        }
        
        // Save server connection info for reconnect
        function onConnected() {
            // Connection info is saved when connecting
            console.log("[QML] Connected to server")
        }
    }
    
    // Helper function to find OneVNMode page in stack
    function findOneVNModePage() {
        for (var i = 0; i < stackView.depth; i++) {
            var item = stackView.get(i)
            if (item && item.objectName === "oneVNMode") {
                return item
            }
        }
        return null
    }
    
    // Attempt to reconnect to server
    function attemptReconnect() {
        console.log("[QML] Attempting to reconnect to " + savedServerHost + ":" + savedServerPort)
        
        if (networkClient.connectToServer(savedServerHost, savedServerPort)) {
            console.log("[QML] Connected, sending reconnect request...")
            networkClient.sendReconnect()
        } else {
            console.log("[QML] Failed to connect, will retry...")
            // Retry after 2 seconds
            reconnectRetryTimer.start()
        }
    }
    
    Timer {
        id: reconnectRetryTimer
        interval: 2000
        repeat: false
        onTriggered: {
            if (isReconnecting && reconnectCountdown > 0) {
                attemptReconnect()
            }
        }
    }
    
    property string reconnectFailedError: ""
    
    // Disconnect notification dialog (normal disconnect, not during game)
    Dialog {
        id: disconnectDialog
        title: "Mất kết nối"
        modal: true
        anchors.centerIn: parent
        width: 400
        
        contentItem: ColumnLayout {
            spacing: 20
            
            Text {
                text: "Phiên làm việc đã hết hạn do không hoạt động.\nVui lòng đăng nhập lại."
                color: "#FFFFFF"
                font.pixelSize: 16
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
            
            Button {
                text: "Đăng nhập lại"
                Layout.alignment: Qt.AlignHCenter
                
                background: Rectangle {
                    color: parent.pressed ? "#7B3FF2" : "#9B59D0"
                    radius: 8
                    implicitWidth: 150
                    implicitHeight: 40
                }
                
                contentItem: Text {
                    text: parent.text
                    color: "#FFFFFF"
                    font.pixelSize: 14
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: {
                    disconnectDialog.close()
                    stackView.clear()
                    stackView.push("Signin.qml", {"stackView": stackView})
                }
            }
        }
        
        background: Rectangle {
            color: "#3D2555"
            radius: 10
            border.color: "#9B59D0"
            border.width: 2
        }
    }
    
    // [RECONNECT] Reconnecting dialog with countdown
    Dialog {
        id: reconnectingDialog
        title: "Đang kết nối lại..."
        modal: true
        anchors.centerIn: parent
        width: 400
        closePolicy: Popup.NoAutoClose
        
        contentItem: ColumnLayout {
            spacing: 20
            
            BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
                running: true
                width: 60
                height: 60
            }
            
            Text {
                text: "Mất kết nối trong game!\nĐang cố gắng kết nối lại..."
                color: "#FFFFFF"
                font.pixelSize: 16
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
            
            Text {
                text: "Thời gian còn lại: " + reconnectCountdown + " giây"
                color: reconnectCountdown <= 10 ? "#FF6B6B" : "#FFC107"
                font.pixelSize: 18
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
            
            Button {
                text: "Hủy"
                Layout.alignment: Qt.AlignHCenter
                
                background: Rectangle {
                    color: parent.pressed ? "#C62828" : "#D32F2F"
                    radius: 8
                    implicitWidth: 120
                    implicitHeight: 40
                }
                
                contentItem: Text {
                    text: parent.text
                    color: "#FFFFFF"
                    font.pixelSize: 14
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: {
                    reconnectTimer.stop()
                    isReconnecting = false
                    reconnectingDialog.close()
                    stackView.clear()
                    stackView.push("Signin.qml", {"stackView": stackView})
                }
            }
        }
        
        background: Rectangle {
            color: "#3D2555"
            radius: 10
            border.color: "#FFC107"
            border.width: 2
        }
    }
    
    // [RECONNECT] Success dialog
    Dialog {
        id: reconnectSuccessDialog
        title: "Kết nối lại thành công!"
        modal: true
        anchors.centerIn: parent
        width: 400
        
        contentItem: ColumnLayout {
            spacing: 20
            
            Text {
                text: "✓ Đã kết nối lại thành công!\nBạn có thể tiếp tục chơi."
                color: "#4CAF50"
                font.pixelSize: 16
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
            
            Button {
                text: "Tiếp tục"
                Layout.alignment: Qt.AlignHCenter
                
                background: Rectangle {
                    color: parent.pressed ? "#388E3C" : "#4CAF50"
                    radius: 8
                    implicitWidth: 120
                    implicitHeight: 40
                }
                
                contentItem: Text {
                    text: parent.text
                    color: "#FFFFFF"
                    font.pixelSize: 14
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: {
                    reconnectSuccessDialog.close()
                }
            }
        }
        
        background: Rectangle {
            color: "#3D2555"
            radius: 10
            border.color: "#4CAF50"
            border.width: 2
        }
    }
    
    // [RECONNECT] Failed dialog
    Dialog {
        id: reconnectFailedDialog
        title: "Kết nối lại thất bại"
        modal: true
        anchors.centerIn: parent
        width: 400
        
        contentItem: ColumnLayout {
            spacing: 20
            
            Text {
                text: {
                    var msg = "Không thể kết nối lại.\n"
                    if (reconnectFailedError === "GRACE_PERIOD_EXPIRED") {
                        msg += "Đã quá thời gian cho phép (30 giây)."
                    } else if (reconnectFailedError === "INVALID_TOKEN") {
                        msg += "Token không hợp lệ."
                    } else if (reconnectFailedError === "ALREADY_ELIMINATED") {
                        msg += "Bạn đã bị loại khỏi game."
                    } else {
                        msg += "Lỗi: " + reconnectFailedError
                    }
                    return msg
                }
                color: "#FF6B6B"
                font.pixelSize: 16
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
            
            Button {
                text: "Đăng nhập lại"
                Layout.alignment: Qt.AlignHCenter
                
                background: Rectangle {
                    color: parent.pressed ? "#7B3FF2" : "#9B59D0"
                    radius: 8
                    implicitWidth: 150
                    implicitHeight: 40
                }
                
                contentItem: Text {
                    text: parent.text
                    color: "#FFFFFF"
                    font.pixelSize: 14
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: {
                    reconnectFailedDialog.close()
                    stackView.clear()
                    stackView.push("Signin.qml", {"stackView": stackView})
                }
            }
        }
        
        background: Rectangle {
            color: "#3D2555"
            radius: 10
            border.color: "#D32F2F"
            border.width: 2
        }
    }
    
    StackView {
        id: stackView
        anchors.fill: parent
        
        Component.onCompleted: {
            push("SplashScreen.qml", {"stackView": stackView})
        }
        
        // Smooth transitions
        pushEnter: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: 300
            }
        }
        popExit: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: 300
            }
        }
    }

    onVisibleChanged: {
        if (visible && screen) {
            x = (screen.width - width) / 2
            y = (screen.height - height) / 2
        }
    }
}
