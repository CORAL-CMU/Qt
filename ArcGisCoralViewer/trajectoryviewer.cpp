#include "trajectoryviewer.h"

#include <QLayout>
#include <QPushButton>
#include <QComboBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QColorDialog>
#include <QFile>

TrajectoryViewer::TrajectoryViewer(QWidget * parent)
    : QWidget(parent)
{
    m_mapview = new Esri::ArcGISRuntime::MapGraphicsView(this);
    m_mapview->setWrapAroundMode(Esri::ArcGISRuntime::WrapAroundMode::Disabled);

    m_map = new Esri::ArcGISRuntime::Map(Esri::ArcGISRuntime::BasemapType::ImageryWithLabelsVector,40.444057, -79.946452, 15, this);

    m_mapview->setMap(m_map);

    m_overlay = new Esri::ArcGISRuntime::GraphicsOverlay(this);
    m_mapview->graphicsOverlays()->append(m_overlay);

    m_pc = new Esri::ArcGISRuntime::PointCollection(Esri::ArcGISRuntime::SpatialReference(4326), this);
    m_mptbuilder = new Esri::ArcGISRuntime::MultipointBuilder(Esri::ArcGISRuntime::SpatialReference(4326), this);

    QHBoxLayout * hlayout = new QHBoxLayout();
    QPushButton * load = new QPushButton("Load Trajectory...");
    hlayout->addWidget(load);
    hlayout->addStretch();
    connect(load, SIGNAL(clicked()), this, SLOT(slotLoadTrajectory()));

    QVBoxLayout * layout = new QVBoxLayout();
    layout->addLayout(hlayout);
    layout->addWidget(m_mapview);

    this->setLayout(layout);
}

void TrajectoryViewer::loadTrajectory(QString filename, int wkid)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        m_pc->removeAll();
        while(!file.atEnd())
        {
            QString pos = file.readLine();
            QList<QString> values = pos.split("\t", QString::SkipEmptyParts);
            if(values.size()>=2)
            {
                double v1 = values[0].trimmed().toDouble();
                double v2 = values[1].trimmed().toDouble();
                if(values.size()==2)
                {
                    Esri::ArcGISRuntime::Point point(v2, v1, Esri::ArcGISRuntime::SpatialReference(wkid));
                    Esri::ArcGISRuntime::Point point_proj = Esri::ArcGISRuntime::GeometryEngine::project(point, Esri::ArcGISRuntime::SpatialReference(4326));
                    //qInfo("%f, %f -> %f, %f", point.x(), point.y(), point_proj.x(), point_proj.y());
                    int err = m_pc->addPoint(point_proj);
                    //qInfo("%d", err);
                }
                else
                {
                    double v3 = values[2].trimmed().toDouble();
                    Esri::ArcGISRuntime::Point point(v2, v1, v3, Esri::ArcGISRuntime::SpatialReference(wkid));
                    Esri::ArcGISRuntime::Point point_proj = Esri::ArcGISRuntime::GeometryEngine::project(point, Esri::ArcGISRuntime::SpatialReference(4326));
                    m_pc->addPoint(point_proj);
                }
            }
        }
        file.close();
        //qInfo("%d", m_pc->size());
        m_mptbuilder->setPoints(m_pc);
        m_overlay->graphics()->clear();
        Esri::ArcGISRuntime::Multipoint mpoint(m_mptbuilder->toGeometry());

        QColor trajcolor = QColorDialog::getColor(Qt::red, this, "Choose Trajectory Color", QColorDialog::ShowAlphaChannel|QColorDialog::DontUseNativeDialog);

        Esri::ArcGISRuntime::SimpleMarkerSymbol * symbol = new Esri::ArcGISRuntime::SimpleMarkerSymbol(Esri::ArcGISRuntime::SimpleMarkerSymbolStyle::Circle, trajcolor, 1);

        m_overlay->graphics()->append(new Esri::ArcGISRuntime::Graphic(mpoint, symbol, this));  
    }
}

void TrajectoryViewer::slotLoadTrajectory()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Trajectory File");
    if(!filename.isEmpty())
    {
        int wkid = QInputDialog::getInt(this, "Set Geographic Coordinate Systems", "SpatialReference ID", 6346);
        loadTrajectory(filename, wkid);
    }
}
