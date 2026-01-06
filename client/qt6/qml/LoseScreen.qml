import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 6.0

Item {
    id: loseScreen
    width: parent ? parent.width : 500
    height: parent ? parent.height : 600
    
    property StackView stackView
    property string username: ""
    property int questionsAnswered: 0
    
    // Background gradient
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#241437" }
            GradientStop { position: 1.0; color: "#2A1845" }
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 40
        spacing: 30
        
        Item { Layout.fillHeight: true }
        
        // Title
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            radius: 15
            color: "#3A2A5E"
            border.color: "#D32F2F"
            border.width: 1
            
            Text {
                anchors.centerIn: parent
                text: "Rất tiếc!"
                font.family: "Lexend"
                font.pixelSize: 36
                font.bold: true
                color: "#D32F2F"
            }
        }
        
        // Message
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            radius: 10
            color: "#3A2A5E"
            
            ColumnLayout {
                anchors.centerIn: parent
                spacing: 12
                
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "Trả lời sai"
                    font.family: "Lexend"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#FFFFFF"
                }
                
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "Đúng: " + questionsAnswered + " / 15"
                    font.family: "Lexend"
                    font.pixelSize: 18
                    color: "#B9A7D9"
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

