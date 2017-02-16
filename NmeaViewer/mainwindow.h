#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qgmapinterface.h>
#include <nmea.h>

#include <QChartView>
#include <QChart>
#include <QLineSeries>

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
private slots:
    void on_open_clicked();
    void on_hide_clicked();
    void on_show_clicked();
protected slots:
    void slotClientIdConfirmed(QString clientId);
    void slotPreviewValue(QPointF value);
};

class HdopChartView : public QChartView
{
    Q_OBJECT
public:
    explicit HdopChartView(QWidget * parent=Q_NULLPTR);
    explicit HdopChartView(QChart * chart, QWidget * parent=Q_NULLPTR);
Q_SIGNALS:
    void signalPreviewValue(QPointF value);
    void signalSelectedValue(QPointF value);
protected:
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
};

#endif // MAINWINDOW_H
