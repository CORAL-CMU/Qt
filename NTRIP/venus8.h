#ifndef VENUS8_H
#define VENUS8_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMutex>
#include <QThread>
#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>

#include <nmea/nmea.h>

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

class Venus8Logger : public QObject
{
    Q_OBJECT
public:
    explicit Venus8Logger();
    ~ Venus8Logger();

public:
    QString filename;

protected:
    QMutex mutex;
    bool receiveflag;
    QThread * thread;
    QFile file;
    QTextStream stream;

public:
    void setLogFilename();
Q_SIGNALS:
    void signalSetLogFilename(QString filename);
    void signalLogFilenameSet();
protected Q_SLOTS:
    void slotSetLogFilename(QString filename);

public:
    void startLogNmea();
    void stopLogNmea();
Q_SIGNALS:
    void signalStartLogNmea();
    void signalNmeaLogged();
    void signalNmeaLogStopped();
protected:
    bool checkFileOpenFlagAndWriteData(QByteArray nmea);
public Q_SLOTS:
    void slotLogNmea(QByteArray nmea);
};

#endif // VENUS8_H
