#ifndef NTRIP_H
#define NTRIP_H

#include <QObject>
#include <QTcpSocket>
#include <QRegExp>
#include <QVector>
#include <QList>
#include <QString>
#include <QThread>
#include <QMutex>

struct NtripCasterStr
{
    QString type;
    QString mountpoint;
    QString identifier;
    QString format;
    QString formatdetails;  //update rates in parenthesis in seconds
    int carrier;    //0: No; 1: Yes, L1; 2: Yes, L1&L2
    QString navsystem;
    QString network;
    QString country;
    double latitude;
    double longitude;
    int nmea;   //0: Client must not send NMEA message with approximate position to Caster; 1: Client must send NMEA GGA message with approximate position to Caster
    int solution;   //0: Single base; 1: Network
    QString generator;
    QString compression;
    QString authentication; //N: None; B: Basic; D: Digest
    QString fee;  //N: No user fee; Y: Usage is charged
    int bitrate;
    QString misc;
};

class Ntrip : public QTcpSocket
{
    Q_OBJECT
public:
    explicit Ntrip();
    ~Ntrip();

public:
    QString addr;
    QString port;
    QString user;
    QString passwd;
    QString mntpnt;
    QString mntstr;
    QString ntrippath;
    QList<NtripCasterStr> casterlist;
protected:
    QMutex mutex;
    bool receiveflag;
    QThread * thread;

public:
    void decodeNtripPath(QString path);
    void encodeNtripPath();

public:
    void loadCasterList();
Q_SIGNALS:
    void signalLoadCasterList();
    void signalCasterListLoaded();
protected Q_SLOTS:
    void slotLoadCasterList();

protected:
    void connectRtk();
    void disconnectRtk();
Q_SIGNALS:
    void signalConnectRtk();
    void signalDisconnectRtk();
protected Q_SLOTS:
    void slotConnectRtk();
    void slotDisconnectRtk();

public:
    void startReceiveRtk(int casterId, QString GPGGA);
    void stopReceiveRtk();
Q_SIGNALS:
    void signalRequestRtk(int casterId, QString GPGGA);
    void signalRtkReceived(QByteArray rtk);
    void signalRtkEnd();
protected:
    bool checkReceiveFlag();
protected Q_SLOTS:
    void slotRequestRtk(int casterId, QString GPGGA);
};

#endif // NTRIP_H
