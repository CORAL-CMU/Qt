#ifndef VENUS8_H
#define VENUS8_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMutex>
#include <QThread>

#include <nmea.h>

class Venus8 : public QSerialPort
{
    Q_OBJECT
public:
    explicit Venus8();
    ~Venus8();

public:
    QSerialPort::BaudRate baudrate;
    QList<QSerialPortInfo> portlist;

protected:
    QMutex mutex;
    bool receiveflag;
    QThread * thread;
    nmeaINFO info;
    nmeaPARSER parser;

public:
    void loadPortList();
Q_SIGNALS:
    void signalLoadPortList();
    void signalPortListLoaded();
protected Q_SLOTS:
    void slotLoadPortList();

public:
    void startReceiveNmea(int portId);
    void stopReceiveNmea();
Q_SIGNALS:
    void signalStartVenus8(int portId);
    void signalNmeaReceived(QByteArray nmea);
    void signalNmeaParsed(nmeaINFO info);
    void signalVenus8Stopped();
    void signalVenus8ConnectionError();
protected:
    bool checkReceiveFlag();
protected Q_SLOTS:
    void slotStartVenus8(int portId);
};

#endif // VENUS8_H
