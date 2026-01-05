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

