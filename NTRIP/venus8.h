#ifndef VENUS8_H
#define VENUS8_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMutex>
#include <QThread>
#include <QRegExp>

#include <nmea.h>

class Venus8 : public QSerialPort
{
    Q_OBJECT
public:
    explicit Venus8();
    ~Venus8();

public:
    QList<QSerialPortInfo> portlist;
    QSerialPort::BaudRate baudrate;

protected:
    QMutex mutex;
    bool receiveflag;
    bool sendflag;
    QThread * thread;
    nmeaINFO info;
    nmeaPARSER parser;
    static QByteArray messagestart;
    static QByteArray messageend;
    QByteArray message;

public:
    void loadPortList();
Q_SIGNALS:
    void signalLoadPortList();
    void signalPortListLoaded();
protected Q_SLOTS:
    void slotLoadPortList();

//public:
//    void initVenus8(int portId);
//Q_SIGNALS:
//    void signalInitializeVenus8(int portId);
//    void signalVenus8Initialized();
//protected Q_SLOTS:
//    void slotInitializeVenus8(int portId);

public:
    void startReceiveNmea(int portId);
    void stopReceiveNmea();
    void sendMessage(QByteArray hexMessage);
Q_SIGNALS:
    void signalStartVenus8(int portId);
    void signalNmeaReceived(QByteArray nmea);
    void signalNmeaParsed(nmeaINFO info);
    void signalVenus8Stopped();
    void signalVenus8ConnectionError();
    void signalMessageSent();
    void signalMessageNotSent();
    void signalMessageReceived(QByteArray message);
protected:
    bool checkReceiveFlag();
    bool checkSendFlag();
protected Q_SLOTS:
    void slotStartVenus8(int portId);
};

#endif // VENUS8_H
