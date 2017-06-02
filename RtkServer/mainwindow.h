#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QList>
#include <QString>
#include <QInputDialog>

#include <qntripclient.h>
#include <qntripserver.h>

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
    QNtripClient * client;
    QNtripServer * server;
protected:
    int casterid=-1;
private slots:
    void on_setcaster_clicked();
    void on_triggerrtk_clicked();
    void on_triggerserver_clicked();

public slots:
    void slotRtkReceived(QByteArray rtk);
    void slotNewConnection(QString peer);
    void slotUpdateGPGGA(QString GPGGA);
    void slotClientDisconnect(QString peer);
};

#endif // MAINWINDOW_H
