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

#include <QVector3D>
#include <QQuaternion>
#include <QMatrix3x3>


#include <Map.h>
#include <MapGraphicsView.h>
#include <GraphicsOverlay.h>
#include <GeometryEngine.h>

#include <PointCollection.h>
#include <MultipointBuilder.h>
#include <Multipoint.h>
#include <SimpleMarkerSymbol.h>


class ApplanixPosition : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(float y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(float z READ z WRITE setZ NOTIFY zChanged)
    Q_PROPERTY(int wkid READ wkid WRITE setWkid NOTIFY wkidChanged)

public:
    ApplanixPosition(QObject *parent = Q_NULLPTR);
    ApplanixPosition(float &x, float &y, float &z, QObject *parent = Q_NULLPTR);
    ApplanixPosition(float &x, float &y, float &z, int &wkid, QObject *parent = Q_NULLPTR);

    float x() const;
    void setX(const float &x);

    float y() const;
    void setY(const float &y);

    float z() const;
    void setZ(const float &z);

    int wkid() const;
    void setWkid(const int &wkid);

Q_SIGNALS:
    void xChanged(float);
    void yChanged(float);
    void zChanged(float);
    void wkidChanged(int);

private:
    float m_x;
    float m_y;
    float m_z;
    int m_wkid;

public:
    Esri::ArcGISRuntime::Point position() const;
};


class ApplanixOrientation : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float roll READ roll WRITE setRoll NOTIFY rollChanged)
    Q_PROPERTY(float pitch READ pitch WRITE setPitch NOTIFY pitchChanged)
    Q_PROPERTY(float yaw READ yaw WRITE setYaw NOTIFY yawChanged)

public:
    ApplanixOrientation(QObject * parent = Q_NULLPTR);
    ApplanixOrientation(float &roll, float &pitch, float &yaw, QObject * parent = Q_NULLPTR);
    ApplanixOrientation(float &qw, float &qx, float &qy, float &qz, QObject * parent = Q_NULLPTR);

    float roll() const;
    void setRoll(const float &roll);

    float pitch() const;
    void setPitch(const float &pitch);

    float yaw() const;
    void setYaw(const float &yaw);

Q_SIGNALS:
    void rollChanged(float);
    void pitchChanged(float);
    void yawChanged(float);

private:
    float m_roll;
    float m_pitch;
    float m_yaw;

public:
    QVector3D toEulerAngles() const;
    QQuaternion toQuaternion() const;
    QMatrix3x3 toRotationMatrix() const;
};

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

    QVector<Eigen::Vector3d> accelerations;
    QVector<Eigen::Vector3d> linearvelocities;

    QVector<Eigen::Vector3d> angularvelocities;

    QVector<float> speeds;
};

#endif // APPLANIXVIEWER_H
