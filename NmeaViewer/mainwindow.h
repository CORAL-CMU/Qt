#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qgmapinterface.h>
#include <nmea.h>

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
private slots:
    void on_open_clicked();

protected slots:
    void slotClientIdConfirmed(QString clientId);
};

#endif // MAINWINDOW_H
