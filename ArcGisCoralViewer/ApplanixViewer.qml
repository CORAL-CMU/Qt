import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtPositioning 5.3

import Esri.ArcGISRuntime 100.0

Item{
//    width: 800
//    height: 600
//    property real scaleFactor: System.displayScaleFactor

    SplitView{
        anchors.fill: parent
        orientation: Qt.Horizontal

        SplitView{
            orientation: Qt.Vertical
            Layout.minimumWidth: 400
            Layout.fillWidth: true

            MapView{
                id: mapView
                height: 400
                Layout.minimumHeight: 400
                Layout.fillHeight: true

                Map{                    
                    BasemapImageryWithLabelsVector{}
                    initialViewpoint: viewPoint
                }
            }

            TabView{
                height: 200
                Tab{
                    title: "Control Panel"
                }
            }
        }

        TabView{
            width: 400
            Tab{
                title: "Position"
                TableView{
                    TableViewColumn{
                        width: 0.25*parent.width
                        role: "timestamp"
                        title: "TimeStamp (s)"
                        delegate: Rectangle{
                            anchors.fill: parent
                            border.color: "black"
                            Text{
                                text: model.timestamp
                            }
                        }
                    }
                    TableViewColumn{
                        width: 0.25*parent.width
                        role: "pose_x"
                        title: "pose.x (m)"
                        delegate: Rectangle{
                            anchors.fill: parent
                            border.color: "black"
                            Text{
                                text: model.pose_x
                            }
                        }
                    }
                    TableViewColumn{
                        width: 0.25*parent.width
                        role: "pose_y"
                        title: "pose.y (m)"
                        delegate: Rectangle{
                            anchors.fill: parent
                            border.color: "black"
                            Text{
                                text: model.pose_y
                            }
                        }
                    }
                    TableViewColumn{
                        width: 0.25*parent.width
                        role: "pose_z"
                        title: "pose.z (m)"
                        delegate: Rectangle{
                            anchors.fill: parent
                            border.color: "black"
                            Text{
                                text: model.pose_z
                            }
                        }
                    }
                    model: positiondata
                }
            }
        }
    }

    ViewpointCenter{
        id: viewPoint
        center: Point{
            x: -79.946452
            y: 40.444057
            spatialReference: SpatialReference{
                wkid: 4326
            }
        }
        targetScale: 15000
    }
}
