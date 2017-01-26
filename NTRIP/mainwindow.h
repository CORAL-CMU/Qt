#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include "ntrip.h"
#include "venus8.h"

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
    void on_decode_clicked();
    void on_encode_clicked();
    void on_loadcasters_clicked();
    void on_requestrtk_clicked();
    void on_clear_clicked();
    void on_loadports_clicked();
    void on_startvenus8_clicked();
    void on_clearnmea_clicked();
    void on_sendmessage_clicked();
private:
    Ui::MainWindow *ui;
    Ntrip ntrip;
    Venus8 venus8;
public Q_SLOTS:
    void slotCasterListLoaded();
    void slotRtkReceived(QByteArray rtk);
    void slotRtkEnd();
public Q_SLOTS:
    void slotPortListLoaded();
    void slotNmeaReceived(QByteArray nmea);
    void slotNmeaParsed(nmeaINFO info);
    void slotVenus8Stopped();
    void slotVenus8ConnectionError();
public Q_SLOTS:
    void slotMessageSent();
    void slotMessageNotSent();
    void slotMessageReceived(QByteArray message);
};

#endif // MAINWINDOW_H
