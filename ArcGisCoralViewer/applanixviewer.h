#ifndef APPLANIXVIEWER_H
#define APPLANIXVIEWER_H

#include <QWidget>
#include <QFile>
#include <QVector>
#include <QMap>
#include <QLayout>
#include <QTableWidget>
#include <QDateTime>

#include <Eigen/Eigen>

#include <Map.h>
#include <MapGraphicsView.h>
#include <GraphicsOverlay.h>
#include <GeometryEngine.h>

#include <PointCollection.h>
#include <MultipointBuilder.h>
#include <Multipoint.h>
#include <SimpleMarkerSymbol.h>


class ApplanixViewer : public QWidget
{
    Q_OBJECT
public:
    explicit ApplanixViewer(QWidget *parent = Q_NULLPTR);
protected:
    Esri::ArcGISRuntime::MapGraphicsView * m_mapview;
    Esri::ArcGISRuntime::Map * m_map;
    Esri::ArcGISRuntime::GraphicsOverlay * m_overlay;
    QVector<Esri::ArcGISRuntime::MultipointBuilder *> m_mptbuilder;
protected:
    QVector<QDateTime> timestamps;
    QVector<Eigen::Vector3d> positions;
    QVector<Eigen::Quaterniond> orientations;
    QVector<Eigen::Vector3d> linearvelocities;
    QVector<Eigen::Vector3d> angularvelocities;
    QVector<Eigen::Vector3d> accelerations;
    QVector<double> speeds;
};

#endif // APPLANIXVIEWER_H
