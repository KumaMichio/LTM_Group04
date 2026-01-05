import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: inviteDialog
    title: "Lời mời tham gia phòng"
    modal: true
    anchors.centerIn: parent
    width: 400
    height: 250
    
    property string inviterName: ""
    property int roomId: 0
    property int fromUserId: 0
    property StackView stackView: null
    property string username: ""
    
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
            text: "🎮 Lời mời chơi game"
            font.pixelSize: 18
            font.bold: true
            color: "white"
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            color: "#3a3a3a"
            radius: 8
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 10
                
                Label {
                    text: "👤 " + inviterName
                    font.pixelSize: 20
                    font.bold: true
                    color: "#1e88e5"
                }
                
                Label {
                    text: "mời bạn vào phòng 1vN"
                    font.pixelSize: 14
                    color: "#cccccc"
                }
            }
        }
        
        Label {
            Layout.fillWidth: true
            text: "Bạn có muốn tham gia phòng này không?"
            font.pixelSize: 14
            color: "#aaaaaa"
            horizontalAlignment: Text.AlignHCenter
        }
        
        Item {
            Layout.fillHeight: true
        }
        
        RowLayout {
            Layout.alignment: Qt.AlignCenter
            spacing: 20
            
            Button {
                text: "✓ Tham gia"
                font.pixelSize: 14
                font.bold: true
                implicitWidth: 140
                implicitHeight: 45
                
                background: Rectangle {
                    color: parent.down ? "#1565c0" : (parent.hovered ? "#1976d2" : "#1e88e5")
                    radius: 6
                }
                
                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: {
                    console.log("[INVITE] Accepting invite, room_id=" + roomId)
                    networkClient.sendRespondInvite(roomId, true)
                    inviteDialog.close()
                }
            }
            
            Button {
                text: "✗ Từ chối"
                font.pixelSize: 14
                implicitWidth: 140
                implicitHeight: 45
                
                background: Rectangle {
                    color: parent.down ? "#c62828" : (parent.hovered ? "#d32f2f" : "#f44336")
                    radius: 6
                }
                
                contentItem: Text {
                    text: parent.text
                    font: parent.font
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: {
                    console.log("[INVITE] Declining invite, room_id=" + roomId)
                    networkClient.sendRespondInvite(roomId, false)
                    inviteDialog.close()
                }
            }
        }
    }
    
    Connections {
        target: networkClient
        
        function onOneVNRoomJoined(success, roomId, error) {
            if (success && roomId === inviteDialog.roomId) {
                console.log("[INVITE] Successfully joined room " + roomId)
                inviteDialog.close()
                
                // Navigate to OneVNMode waiting room
                if (inviteDialog.stackView) {
                    inviteDialog.stackView.push("OneVNMode.qml", {
                        "stackView": inviteDialog.stackView,
                        "username": inviteDialog.username,
                        "isJoiningRoom": true,
                        "roomId": roomId
                    })
                }
            } else if (!success) {
                console.log("[INVITE] Failed to join room: " + error)
                errorDialog.text = "Không thể vào phòng: " + error
                errorDialog.open()
            }
        }
    }
    
    // Error dialog
    Dialog {
        id: errorDialog
        title: "Lỗi"
        modal: true
        anchors.centerIn: parent
        
        property string text: ""
        
        ColumnLayout {
            Label {
                text: errorDialog.text
                color: "#f44336"
            }
            
            Button {
                text: "OK"
                onClicked: errorDialog.close()
            }
        }
    }
}
