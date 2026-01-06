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
    
    // [HEARTBEAT] Handle disconnect event from server
    Connections {
        target: networkClient
        
        function onDisconnected() {
            console.log("[QML] Disconnected from server - showing dialog")
            disconnectDialog.open()
        }
    }
    
    // Disconnect notification dialog
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

