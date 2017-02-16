#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include "ntrip.h"
#include "venus8.h"
#include "rtcm.h"
#include <qgmapinterface.h>
#include <QNetworkInterface>

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
    void on_setlogfile_clicked();

    void on_startlog_clicked();

private:
    Ui::MainWindow *ui;
    Ntrip ntrip;
    Venus8 venus8;
    Venus8Logger logger;
    Rtcm rtcm;
    QGMapInterface * interface;
    QGMapMarker marker;
    QGMapMarkerConfig markerconfig;
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
public Q_SLOTS:
    void slotLogFilenameSet();
public Q_SLOTS:
    void slotRtcmGpsData(RtcmGPSData gpsData);
    void slotRtcmStationaryAntennaData(RtcmStationaryAntennaData stationaryAntennaData);
    void slotRtcmAntennaDescriptor(RtcmAntennaDescriptor antennaDescriptor);
    void slotRtcmReceiverAntennaDescriptor(RtcmReceiverAntennaDescriptor receiverAntennaDescriptor);
public Q_SLOTS:
    void slotClientIdConfirmed(QString clientId);
};

#endif // MAINWINDOW_H
