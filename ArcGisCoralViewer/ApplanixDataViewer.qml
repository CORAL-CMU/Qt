import QtQuick 2.5
import QtQuick.Controls 1.4

TableView{
    anchors.fill: parent
    model: applanixdata
    TableViewColumn{
        role: "timestamp"
        title: "timeStamp"
        delegate: Rectangle{
            anchors.fill: parent
            border.color: "lightgrey"
            Text{
                text: model.modelData.timestamp
            }
        }
    }
    TableViewColumn{
        role: "p"
        title: "pose.x"
        delegate: Rectangle{
            anchors.fill: parent
            border.color: "lightgrey"
            Text{
                text: model.modelData.posex
            }
        }
    }
    TableViewColumn{
        role: "posey"
        title: "pose.y"
        delegate: Rectangle{
            anchors.fill: parent
            border.color: "lightgrey"
            Text{
                text: model.modelData.posey
            }
        }
    }
    TableViewColumn{
        role: "posez"
        title: "pose.z"
        delegate: Rectangle{
            anchors.fill: parent
            border.color: "lightgrey"
            Text{
                text: model.modelData.posez
            }
        }
    }
    TableViewColumn{
        role: "rot1"
        title: "pose.rot1"
        delegate: Rectangle{
            anchors.fill: parent
            border.color: "lightgrey"
            Text{
                text: model.modelData.rot1
            }
        }
    }
    TableViewColumn{
        role: "rot2"
        title: "pose.rot2"
        delegate: Rectangle{
            anchors.fill: parent
            border.color: "lightgrey"
            Text{
                text: model.modelData.rot2
            }
        }
    }
    TableViewColumn{
        role: "rot3"
        title: "pose.rot3"
        delegate: Rectangle{
            anchors.fill: parent
            border.color: "lightgrey"
            Text{
                text: model.modelData.rot3
            }
        }
    }

}
