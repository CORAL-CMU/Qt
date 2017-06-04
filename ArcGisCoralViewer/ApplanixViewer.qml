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
                        role: "timestamp"
                        title: "TimeStamp (s)"
                    }
                    TableViewColumn{
                        role: "pose_x"
                        title: "pose.x (m)"
                    }
                    TableViewColumn{
                        role: "pose_y"
                        title: "pose.y (m)"
                    }
                    TableViewColumn{
                        role: "pose_z"
                        title: "pose.z (m)"
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

    ListModel{
        id: positiondata
        ListElement{
            timestamp: 1494374906.56721
            pose_x: 4477753.45187315
            pose_y: 588765.528685924
            pose_z: -246.2187476945
        }
    }
}
