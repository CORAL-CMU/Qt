import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import Coral.Viewer 1.0

TableView {
    anchors.fill: parent

    model: myModel

    TableViewColumn {
        role: "timestamp"
        title: "TimeStamp (ms)"
        width: 150
        delegate: Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "black"
            Text {
                anchors.fill: parent
                text: styleData.value.msecsSinceStartOfDay
            }
        }
    }
    TableViewColumn {
        role: "position"
        title: "Position"
        width: 500
        delegate: Row {
            anchors.fill: parent
            Rectangle {
                width: parent.width/3
                height: parent.height
                color: "transparent"
                border.color: "lightgrey"
                Text {
                    anchors.fill: parent
                    text: styleData.value.x
                }
            }
            Rectangle {
                width: parent.width/3
                height: parent.height
                color: "transparent"
                border.color: "lightgrey"
                Text {
                    anchors.fill: parent
                    text: styleData.value.y
                }
            }
            Rectangle {
                width: parent.width/3
                height: parent.height
                color: "transparent"
                border.color: "lightgrey"
                Text {
                    anchors.fill: parent
                    text: styleData.value.z
                }
            }
        }
    }

    TableViewColumn {
        role: "position"
        title: "WGS84"
        width: 500
        delegate: Row {
            anchors.fill: parent
            Rectangle {
                width: parent.width/3
                height: parent.height
                color: "transparent"
                border.color: "lightgrey"
                Text {
                    anchors.fill: parent
                    text: styleData.value.latitude
                }
            }
            Rectangle {
                width: parent.width/3
                height: parent.height
                color: "transparent"
                border.color: "lightgrey"
                Text {
                    anchors.fill: parent
                    text: styleData.value.longitude
                }
            }
            Rectangle {
                width: parent.width/3
                height: parent.height
                color: "transparent"
                border.color: "lightgrey"
                Text {
                    anchors.fill: parent
                    text: styleData.value.altitude
                }
            }
        }
    }

    TableViewColumn {
        role: "orientation"
        title: "Orientation"
        width: 500
        delegate: Row {
            anchors.fill: parent
            Rectangle {
                width: parent.width/3
                height: parent.height
                color: "transparent"
                border.color: "lightgrey"
                Text {
                    anchors.fill: parent
                    text: styleData.value.rollDeg
                }
            }
            Rectangle {
                width: parent.width/3
                height: parent.height
                color: "transparent"
                border.color: "lightgrey"
                Text {
                    anchors.fill: parent
                    text: styleData.value.pitchDeg
                }
            }
            Rectangle {
                width: parent.width/3
                height: parent.height
                color: "transparent"
                border.color: "lightgrey"
                Text {
                    anchors.fill: parent
                    text: styleData.value.yawDeg
                }
            }
        }
    }

    TableViewColumn {
        role: "linearVelocity"
        title: "Linear Velocity"
        width: 500
        delegate: Row {
            anchors.fill: parent
            Rectangle {
                width: parent.width/3
                height: parent.height
                color: "transparent"
                border.color: "lightgrey"
                Text {
                    anchors.fill: parent
                    text: styleData.value.x
                }
            }
            Rectangle {
                width: parent.width/3
                height: parent.height
                color: "transparent"
                border.color: "lightgrey"
                Text {
                    anchors.fill: parent
                    text: styleData.value.y
                }
            }
            Rectangle {
                width: parent.width/3
                height: parent.height
                color: "transparent"
                border.color: "lightgrey"
                Text {
                    anchors.fill: parent
                    text: styleData.value.z
                }
            }
        }
    }

    TableViewColumn {
        role: "linearVelocity"
        title: "Speed (m/s)"
        width: 150
        delegate: Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "black"
            Text {
                anchors.fill: parent
                text: styleData.value.speed
            }
        }
    }

    TableViewColumn {
        role: "angularVelocity"
        title: "Angular Velocity"
        width: 500
        delegate: Row {
            anchors.fill: parent
            Rectangle {
                width: parent.width/3
                height: parent.height
                color: "transparent"
                border.color: "lightgrey"
                Text {
                    anchors.fill: parent
                    text: styleData.value.rollDeg
                }
            }
            Rectangle {
                width: parent.width/3
                height: parent.height
                color: "transparent"
                border.color: "lightgrey"
                Text {
                    anchors.fill: parent
                    text: styleData.value.pitchDeg
                }
            }
            Rectangle {
                width: parent.width/3
                height: parent.height
                color: "transparent"
                border.color: "lightgrey"
                Text {
                    anchors.fill: parent
                    text: styleData.value.yawDeg
                }
            }
        }
    }
}
