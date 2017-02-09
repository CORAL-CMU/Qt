#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNmeaPositionInfoSource>
#include <QGeoPositionInfo>
#include <qgmapinterface.h>
#include <QtWebEngineWidgets>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QVector>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_Open_triggered();

    void on_pushButton_clicked();

    void on_action_Play_Positions_triggered();

private:
    Ui::MainWindow *ui;

    QGMapInterface * interface;

    QFile * file;
    QFile * savefile;
    QTextStream stream;
    QNmeaPositionInfoSource * source;
    QVector<QGeoPositionInfo> positions;

    QGMapPolyline polyline;
    QGMapPolylineConfig polylineconfig;
    QVector<QGMapMarker> markers;
    QGMapMarkerConfig markerconfig;

    QTimer * timer;
    int posid;
public Q_SLOTS:
    void slotPositionUpdated(const QGeoPositionInfo & update);
    void slotFinishLoading();
    void slotPlayPosition();
Q_SIGNALS:
    void signalFinishLoading();
};

#endif // MAINWINDOW_H
