#ifndef TRAJECTORYVIEWER_H
#define TRAJECTORYVIEWER_H

#include <QWidget>
#include <Map.h>
#include <MapGraphicsView.h>
#include <GraphicsOverlay.h>
#include <GeometryEngine.h>

#include <PointCollection.h>
#include <MultipointBuilder.h>
#include <Multipoint.h>
#include <SimpleMarkerSymbol.h>

#include <PolylineBuilder.h>
#include <Polyline.h>


class TrajectoryViewer : public QWidget
{
    Q_OBJECT
public:
    TrajectoryViewer(QWidget * parent = Q_NULLPTR);
protected:
    Esri::ArcGISRuntime::MapGraphicsView * m_mapview;
    Esri::ArcGISRuntime::Map * m_map;
    Esri::ArcGISRuntime::GraphicsOverlay * m_overlay;
protected:
    Esri::ArcGISRuntime::PointCollection * m_pc;
    Esri::ArcGISRuntime::MultipointBuilder * m_mptbuilder;
    Esri::ArcGISRuntime::PolylineBuilder * m_pllbuilder;

protected:
    void loadTrajectory(QString filename, int wkid);
public Q_SLOTS:
    void slotLoadTrajectory();
};

#endif // TRAJECTORYVIEWER_H
