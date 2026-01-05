import QtQuick 6.0
import QtQuick.Controls 6.0

Button {
    id: optionButton
    property string optionText: ""
    property string optionLetter: ""
    property bool forceWhiteWhenDisabled: false  // If true, show white color even when disabled
    
    text: optionText
    font.family: "Lexend"
    font.pixelSize: 16
    font.bold: true
    
    // Disable hover effect when highlighted
    hoverEnabled: !isHighlighted
    
    // Debug: Log when optionText changes
    onOptionTextChanged: {
        console.log("GameOptionButton optionText changed:", optionText, "button text:", text)
    }
    
    background: Rectangle {
        id: bgRect
        color: {
            // Highlighted state
            if (optionButton.isHighlighted) {
                return optionButton.highlightColor
            }
            // Hover (enabled)
            if (optionButton.hovered && optionButton.enabled) {
                return "#4F3A75"  // Slightly lighter hover on base
            }
            // Disabled but force white text
            if (!optionButton.enabled && optionButton.forceWhiteWhenDisabled) {
                return "#46306B"
            }
            // Default unselected
            return optionButton.enabled ? "#46306B" : "#46306B"
        }
        radius: 12
        border.color: {
            if (optionButton.isHighlighted) {
                return optionButton.highlightColor
            }
            return "#46306B"
        }
        border.width: 2
        
        // Force update color when highlighted state changes
        Connections {
            target: optionButton
            function onIsHighlightedChanged() {
                if (optionButton.isHighlighted) {
                    bgRect.color = optionButton.highlightColor
                    bgRect.border.color = optionButton.highlightColor
                }
            }
            function onHoveredChanged() {
                if (optionButton.isHighlighted) {
                    // Force highlight color even when hovered
                    bgRect.color = optionButton.highlightColor
                    bgRect.border.color = optionButton.highlightColor
                }
            }
        }
    }
    
    contentItem: Text {
        id: textItem
        text: optionButton.optionText  // Use optionText directly instead of optionButton.text
        textFormat: Text.PlainText  // Force plain text, don't interpret HTML
        font: optionButton.font
        color: {
            if (optionButton.isHighlighted) {
                return "#FFFFFF"
            }
            if (!optionButton.enabled && optionButton.forceWhiteWhenDisabled) {
                return "#EDE7F6"
            }
            return optionButton.enabled ? "#EDE7F6" : "#B9A7D9"
        }
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        leftPadding: 15
        rightPadding: 15
        wrapMode: Text.WordWrap
        elide: Text.ElideNone  // Don't elide text
        
        // Debug: Log when text changes
        onTextChanged: {
            console.log("GameOptionButton textItem text changed:", text, "length:", text.length)
        }
    }
    
    function resetStyle() {
        isHighlighted = false
        bgRect.color = "#46306B"
        bgRect.border.color = "#46306B"
        textItem.color = "#EDE7F6"
    }
    
    property color highlightColor: "#FFC107"  // Selected answer color (vàng đậm)
    property bool isHighlighted: false
    
    function highlight(color) {
        // Mark as highlighted and store color
        isHighlighted = true
        highlightColor = color
        
        // Force highlight immediately
        bgRect.color = color
        bgRect.border.color = color
        textItem.color = "#FFFFFF"
    }
    
    // Force update colors when highlighted state changes
    onIsHighlightedChanged: {
        if (isHighlighted) {
            // Force update to highlight color
            Qt.callLater(function() {
                bgRect.color = highlightColor
                bgRect.border.color = highlightColor
                textItem.color = "#FFFFFF"
            })
        }
    }
    
    // Force maintain highlight color on any state change
    onHoveredChanged: {
        if (isHighlighted) {
            // Force highlight color immediately, ignore hover
            bgRect.color = highlightColor
            bgRect.border.color = highlightColor
            textItem.color = "#FFFFFF"
        }
    }
    
    // Also handle pressed state
    onPressedChanged: {
        if (isHighlighted) {
            // Force highlight color immediately
            bgRect.color = highlightColor
            bgRect.border.color = highlightColor
            textItem.color = "#FFFFFF"
        }
    }
    
    // Use Timer to continuously maintain highlight color when highlighted
    Timer {
        id: highlightMaintainTimer
        interval: 50
        running: optionButton.isHighlighted
        repeat: true
        onTriggered: {
            if (optionButton.isHighlighted) {
                // Continuously force highlight color to override any hover/pressed effects
                bgRect.color = optionButton.highlightColor
                bgRect.border.color = optionButton.highlightColor
                textItem.color = "#FFFFFF"
            }
        }
    }
    
    // Force highlight state using states
    states: [
        State {
            name: "highlighted"
            when: isHighlighted
            PropertyChanges {
                target: bgRect
                color: highlightColor
            }
            PropertyChanges {
                target: bgRect
                border.color: highlightColor
            }
            PropertyChanges {
                target: textItem
                color: "#FFFFFF"
            }
        }
    ]
}

