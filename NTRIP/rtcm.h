#ifndef RTCM_H
#define RTCM_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QMutex>
#include <QList>
#include <QByteArray>
#include <QtEndian>
#include <QBitArray>

//#include "rtklib.h"

//====================================================
//GPS

struct RtcmGPSHeader    //bit(64)
{
    uint type;    //DF002: 0-4095, uint12
    uint staid;  //DF003: 0-4095, uint12
    uint tow; //DF004: GPS Epoch Time (TOW), Resolution 1ms, uint30, begins at midnight GMT on Saturday night/Sunday morning.
    bool sync;   //DF005: Synchronous GNSS Message Flag, bit(1)
    uint nsat;   //DF006: No. of GPS Satellite Signals Processed, uint5
    bool sm;    //DF007: bit(1), 0 divergence-free smoothing not used, 1 used
    char smint; //DF008: bit(3), integration period over which reference station pseudorange code phase measurements are averaged using carrier phase information.
};

struct RtcmGPSL1
{
    char code1;  //DF010: GPS L1 Code Indicator, bit(1),
    uint pr1;    //DF011: GPS L1 Pseudorange, uint24,
    int ppr11;   //DF012: GPS L1 Phaserange - L1 Pseudorange, int20
    uint lock1;  //DF013: GPS L1 Lock time indicator, uint7
    uint amb;    //DF014: GPS integer L1 Pseudorange modulus ambiguity, uint8
    uint cnr1;   //DF015: GPS L1 CNR, uint8
};

struct RtcmGPSL2
{
    char code2;   //DF016: GPS L2 Code Indicator, bit(2)
    int pr21;   //DF017: GPS L2 - L1 Pseudorange Difference, int14
    int ppr21;   //DF018: GPS L2 Phaserange - L1 Pseudorange, int20
    uint lock2;  //DF019: GPS L2 Lock time Indicator, uint7
    uint cnr2;   //DF020: GPS L2 CNR, uint8
};

struct RtcmGPSMessage
{
    uint satId;  //DF009: GPS Satellite ID, uint6,
    RtcmGPSL1 gpsl1;
    RtcmGPSL2 gpsl2;
};

struct RtcmGPSData
{
    RtcmGPSHeader header;
    RtcmGPSMessage message;
};

//====================================================
//Stationary Antenna

struct RtcmStationaryAntennaData
{
    uint type;    //DF002: 0-4095, uint12
    uint staid;  //DF003: 0-4095, uint12
    uint itrf;   //DF021: Reserved for IRTF Realization Year, uint6
    bool gpsind;
    bool glonassind;
    bool galileoind;
    bool refstaind;
    qint64 ecefx;   //DF025： Antenna Reference Point ECEF-X, int38
    bool sroind;
    bool reserved;
    qint64 ecefy;   //DF026: Antenna Reference Point ECEF-Y, int38
    char qcind;
    qint64 ecefz;   //DF027: Antenna Reference Point ECEF-Z, int38
    uint antheight;
};

//====================================================
//Antenna Descriptor

struct RtcmAntennaDescriptor
{
    uint type;
    uint staid;
    uint descn;
    QString desc;
    uint antsetupid;
};

//====================================================
//Receiver and Antenna Descriptor
struct RtcmReceiverAntennaDescriptor
{
    RtcmAntennaDescriptor antdesc;
    uint antserialn;
    QString antserial;
    uint recvtypen;
    QString recvtype;
    uint recvfirmwaren;
    QString recvfirmware;
    uint recvserialn;
    QString recvserial;
};

class Rtcm : public QObject
{
    Q_OBJECT
public:
    explicit Rtcm();
    ~Rtcm();

protected:
    QThread * thread;

protected:
    template<typename T>
    T fetchValue(QByteArray & data, int startBitId, int bitNum)
    {
        T value;
        int len=sizeof(T);
        memset(&value,0,len);
        if(len*8>=bitNum)
        {
            int startByte=startBitId/8;
            int startBit=7-startBitId%8;
            int byteNum=bitNum/8+(bitNum%8==0?0:1);
            for(int i=0;i<byteNum;i++)
            {
                for(int j=(i==0?startBit:7);j>=0;j--)
                {
                    T bit=(data[startByte+i]&(1<<j))>>j;
                    if(bitNum-->0)
                    {
                        value|=T(bit<<bitNum);
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        return value;
    }

public Q_SLOTS:
    void slotDecodeRtcm(QByteArray rtcm);    
protected:
    void decodeRtcmData(QByteArray & data);
Q_SIGNALS:
    void signalRtcmGpsData(RtcmGPSData gpsData);
    void signalRtcmStationaryAntennaData(RtcmStationaryAntennaData stationaryAntennaData);
    void signalRtcmAntennaDescriptor(RtcmAntennaDescriptor antennaDescriptor);
    void signalRtcmReceiverAntennaDescriptor(RtcmReceiverAntennaDescriptor receiverAntennaDescriptor);
protected:
    int decodeDataType(QByteArray & data);
    //1001-1004
    RtcmGPSHeader decodeGpsHeader(QByteArray & data);
    RtcmGPSMessage decodeGpsMessage(RtcmGPSHeader & header, QByteArray & data);
    //1005-1006
    RtcmStationaryAntennaData decodeStationaryAntennaData(QByteArray & data);
    //1007
    RtcmAntennaDescriptor decodeAntennaDescriptor(QByteArray & data);
    //1033
    RtcmReceiverAntennaDescriptor decodeReceiverAntennaDescriptor(QByteArray & data);
};

#endif // RTCM_H
