#include "applanixviewer.h"

//*********************************************
//ApplanixPosition

using namespace Esri::ArcGISRuntime;

ApplanixPosition::ApplanixPosition(QObject *parent)
    : QObject(parent), m_x(0), m_y(0), m_z(0), m_wkid(4326)
{
}

ApplanixPosition::ApplanixPosition(double &x, double &y, double &z, QObject *parent)
    : QObject(parent), m_x(x), m_y(y), m_z(z), m_wkid(4326)
{
}

ApplanixPosition::ApplanixPosition(double &x, double &y, double &z, int &wkid, QObject *parent)
    : QObject(parent), m_x(x), m_y(y), m_z(z), m_wkid(wkid)
{
}

double ApplanixPosition::x() const
{
    return m_x;
}

void ApplanixPosition::setX(const double &x)
{
    m_x = x;
    emit xChanged(m_x);
}

double ApplanixPosition::y() const
{
    return m_y;
}

void ApplanixPosition::setY(const double &y)
{
    m_y = y;
    emit yChanged(m_y);
}

double ApplanixPosition::z() const
{
    return m_z;
}

void ApplanixPosition::setZ(const double &z)
{
    m_z = z;
    emit zChanged(m_z);
}

int ApplanixPosition::wkid() const
{
    return m_wkid;
}

void ApplanixPosition::setWkid(const int &wkid)
{
    Point point(m_x, m_y, m_z, SpatialReference(m_wkid));
    Point point_proj = GeometryEngine::project(point, SpatialReference(wkid));
    m_x = point_proj.x();
    m_y = point_proj.y();
    m_z = point_proj.z();
    m_wkid = wkid;
    emit wkidChanged(m_wkid);
}

Point ApplanixPosition::position() const
{
    return Point(m_x, m_y, m_z, SpatialReference(m_wkid));
}

//*********************************************
//ApplanixOrientation

ApplanixOrientation::ApplanixOrientation(QObject *parent)
    : QObject(parent), m_roll(0), m_pitch(0), m_yaw(0)
{
}

ApplanixOrientation::ApplanixOrientation(float &roll, float &pitch, float &yaw, QObject *parent)
    : QObject(parent), m_roll(roll), m_pitch(pitch), m_yaw(yaw)
{
}

ApplanixOrientation::ApplanixOrientation(float &qw, float &qx, float &qy, float &qz, QObject *parent)
    :
{

}




ApplanixViewer::ApplanixViewer(QWidget *parent) : QWidget(parent)
{

}
