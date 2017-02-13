#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qgmapinterface.h>
#include <nmea.h>
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

private:
    Ui::MainWindow *ui;

protected:
    QGMapInterface * interface;
    QGMapPolyline polyline;
    QGMapPolylineConfig polylineconfig;
    QVector<QGMapPolyline> polylines;
    QVector<QGMapPolylineConfig> polylineconfigs;

    QTimer timer;
    QGMapPolyline tmppolyline;
    int playid;
    int frameid;
Q_SIGNALS:
    void signalStartTimer(int msec);
    void signalStopTimer();
public Q_SLOTS:
    void slotTimeout();
private slots:
    void on_open_clicked();

    void on_hide_clicked();

    void on_show_clicked();

    void on_play_clicked();

protected slots:
    void slotClientIdConfirmed(QString clientId);
};

#endif // MAINWINDOW_H
