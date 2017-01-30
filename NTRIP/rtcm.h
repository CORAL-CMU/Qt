#ifndef RTCM_H
#define RTCM_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QMutex>
#include <QList>
#include <QByteArray>
#include <QtEndian>

//#include "rtklib.h"

//====================================================
//GPS

struct RtcmGPSHeader    //bit(64)
{
    QByteArray type;    //DF002: 0-4095, uint12
    QByteArray staid;  //DF003: 0-4095, uint12
    QByteArray tow; //DF004: GPS Epoch Time (TOW), Resolution 1ms, uint30, begins at midnight GMT on Saturday night/Sunday morning.
    QByteArray sync;   //DF005: Synchronous GNSS Message Flag, bit(1)
    QByteArray nsat;   //DF006: No. of GPS Satellite Signals Processed, uint5
    QByteArray sm;    //DF007: bit(1), 0 divergence-free smoothing not used, 1 used
    QByteArray smint; //DF008: bit(3), integration period over which reference station pseudorange code phase measurements are averaged using carrier phase information.
};

struct RtcmGPSL1
{
    QByteArray code1;  //DF010: GPS L1 Code Indicator, bit(1),
    QByteArray pr1;    //DF011: GPS L1 Pseudorange, uint24,
    QByteArray ppr1;   //DF012: GPS L1 Phaserange - L1 Pseudorange, int20
    QByteArray lock1;  //DF013: GPS L1 Lock time indicator, uint7
    QByteArray amb;    //DF014: GPS integer L1 Pseudorange modulus ambiguity, uint8
    QByteArray cnr1;   //DF015: GPS L1 CNR, uint8
};

struct RtcmGPSL2
{
    QByteArray code2;   //DF016: GPS L2 Code Indicator, bit(2)
    QByteArray pr21;   //DF017: GPS L2 Code Indicator, int14
    QByteArray ppr2;   //DF018: GPS L2 Phaserange - L1 Pseudorange, int20
    QByteArray lock2;  //DF019: GPS L2 Lock time Indicator, uint7
    QByteArray cnr2;   //DF020: GPS L2 CNR, uint8
};

struct RtcmGPSMessage
{
    QByteArray satId;  //DF009: GPS Satellite ID, uint6,
    RtcmGPSL1 gpsl1;
    RtcmGPSL2 gpsl2;
};

struct RtcmGPSData
{
    RtcmGPSHeader header;
    RtcmGPSMessage gps;
};

//====================================================
//Stationary Antenna

struct RtcmStationaryAntennaData
{
    QByteArray type;    //DF002: 0-4095, uint12
    QByteArray staid;  //DF003: 0-4095, uint12
    QByteArray itrf;   //DF021: Reserved for IRTF Realization Year, uint6
    QByteArray ecefx;   //DF025： Antenna Reference Point ECEF-X, int38
    QByteArray ecefy;   //DF026: Antenna Reference Point ECEF-Y, int38
    QByteArray ecefz;   //DF027: Antenna Reference Point ECEF-Z, int38
};

class Rtcm : public QObject
{
    Q_OBJECT
public:
    explicit Rtcm();
    ~Rtcm();

protected:
    QThread * thread;

public Q_SLOTS:
    void slotDecodeRtcm(QByteArray rtcm);
protected:
    int decodeMessageType(QByteArray message);

};

#endif // RTCM_H
