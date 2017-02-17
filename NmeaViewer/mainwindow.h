#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qgmapinterface.h>
#include <nmea.h>

#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QLayout>

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

protected:
    QGMapInterface * interface;
    QGMapPolyline polyline;
    QGMapPolylineConfig polylineconfig;
    QVector<QGMapPolyline> polylines;
    QVector<QGMapPolylineConfig> polylineconfigs;

    QGMapMarker marker;
    QGMapMarkerConfig markerconfig;

    QVector<QGraphicsLineItem *> hdopxlines;
    QVector<QGraphicsLineItem *> hdopylines;
    QVector<QGraphicsLineItem *> satnumxlines;
    QVector<QGraphicsLineItem *> satnumylines;

    QVector<bool> showflags;
    QVector<int> dataids;
    int mapzoom;
private slots:
    void on_open_clicked();
    void on_hide_clicked();
    void on_show_clicked();
    void on_nmeas_currentRowChanged(int currentRow);
    void on_dataid_valueChanged(int position);
    void on_timeline_valueChanged(int position);

protected slots:
    void slotClientIdConfirmed(QString clientId);
    void slotPreviewHdopValue(QPointF value, QPointF position);
    void slotPreviewSatnumValue(QPointF value, QPointF position);
    void slotZoom(bool directionflag);
};

class HdopChartView : public QChartView
{
    Q_OBJECT
public:
    explicit HdopChartView(QWidget * parent=Q_NULLPTR);
    explicit HdopChartView(QChart * chart, QWidget * parent=Q_NULLPTR);
Q_SIGNALS:
    void signalPreviewValue(QPointF value, QPointF position);
    void signalSelectedValue(QPointF value, QPointF position);
    void signalMouseWheel(bool directionflag);
protected:
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void wheelEvent(QWheelEvent * event);
};

#endif // MAINWINDOW_H
