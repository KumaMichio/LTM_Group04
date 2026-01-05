import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0

Item {
    id: winScreen
    width: parent ? parent.width : 500
    height: parent ? parent.height : 600
    
    property StackView stackView
    property string username: ""
    
    // Background gradient
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#241437" }
            GradientStop { position: 1.0; color: "#2A1845" }
        }
    }
    
    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width - 80, 500)
        height: parent.height
        spacing: 30
        
        Item { Layout.fillHeight: true }
        
        // Title
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            radius: 15
            color: "#3A2A5E"
            border.color: "#FFC107"
            border.width: 2
            
            // Glow effect using stacked text
            Item {
                anchors.centerIn: parent
                width: titleText.width
                height: titleText.height
                
                // Glow layers
                Text {
                    anchors.centerIn: parent
                    text: "CHÚC MỪNG!"
                    font.family: "Lexend"
                    font.pixelSize: 36
                    font.bold: true
                    color: "#FFC107"
                    opacity: 0.3
                    scale: 1.05
                }
                Text {
                    anchors.centerIn: parent
                    text: "CHÚC MỪNG!"
                    font.family: "Lexend"
                    font.pixelSize: 36
                    font.bold: true
                    color: "#FFC107"
                    opacity: 0.5
                    scale: 1.02
                }
                // Main text
                Text {
                    id: titleText
                    anchors.centerIn: parent
                    text: "CHÚC MỪNG!"
                    font.family: "Lexend"
                    font.pixelSize: 36
                    font.bold: true
                    color: "#FFC107"
                }
            }
        }
        
        // Message
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 140
            radius: 10
            color: "#3A2A5E"
            
            ColumnLayout {
                anchors.centerIn: parent
                spacing: 16
                
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "Hoàn thành 15/15 câu hỏi!"
                    font.family: "Lexend"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#4CAF50"
                }
                
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "Bạn là một triệu phú thực thụ!"
                    font.family: "Lexend"
                    font.pixelSize: 16
                    color: "#FFFFFF"
                }
            }
        }
        
        // Play Again Button
        Button {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            text: "Chơi lại"
            font.family: "Lexend"
            font.pixelSize: 14
            font.bold: true
            
            background: Rectangle {
                color: "#FFC107"
                radius: 12
            }
            
            contentItem: Text {
                text: parent.text
                font: parent.font
                color: "#1D0F2E"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            
            onClicked: {
                if (stackView) {
                    stackView.replace("QuickModeGame.qml", {
                        "stackView": stackView,
                        "username": username
                    })
                }
            }
        }
        
        // Back to Menu Button
        Button {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            text: "Về menu chính"
            font.family: "Lexend"
            font.pixelSize: 14
            font.bold: true
            
            background: Rectangle {
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#5D4586" }
                    GradientStop { position: 1.0; color: "#3A2A5E" }
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
        
        Item { Layout.fillHeight: true }
    }
}

