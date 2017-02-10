#include "rtcm.h"

#define RTCM3PREAMBLE "D3"

Rtcm::Rtcm()
    : QObject(Q_NULLPTR)
{
    thread=new QThread();
    this->moveToThread(thread);
    thread->start();

    qRegisterMetaType<RtcmGPSData>("RtcmGPSData");
    qRegisterMetaType<RtcmStationaryAntennaData>("RtcmStationaryAntennaData");
    qRegisterMetaType<RtcmAntennaDescriptor>("RtcmAntennaDescriptor");
}

Rtcm::~Rtcm()
{
    thread->exit();
    thread->wait();
    delete thread;
}

void Rtcm::slotDecodeRtcm(QByteArray rtcm)
{
    int chunksizeid=rtcm.indexOf(QByteArray("\r\n"));
    int chunksize=rtcm.left(chunksizeid).toInt(Q_NULLPTR,16);
    if(chunksizeid+chunksize+4!=rtcm.size())
    {
        qWarning(QString("%1: RTCM message chunk size is wrong.").arg(__func__).toUtf8().data());
        return;
    }
    QByteArray data=rtcm.mid(chunksizeid+2,chunksize);
    while(data.startsWith(char(0xD3)))
    {
        int len=fetchValue<int>(data,14,10);
        QByteArray rtcmdata=data.mid(3,len);
        decodeRtcmData(rtcmdata);
        data=data.mid(3+len+3);
    }
}

void Rtcm::decodeRtcmData(QByteArray & data)
{
    int type=decodeDataType(data);
    switch(type)
    {
    case 1001:
    case 1002:
    case 1003:
    case 1004:
        {
            RtcmGPSData gpsdata;
            gpsdata.header=decodeGpsHeader(data);
            gpsdata.message=decodeGpsMessage(gpsdata.header,data);
            emit signalRtcmGpsData(gpsdata);
        }
        break;
    case 1005:
    case 1006:
        {
            RtcmStationaryAntennaData stationaryantennadata;
            stationaryantennadata=decodeStationaryAntennaData(data);
            emit signalRtcmStationaryAntennaData(stationaryantennadata);
        }
        break;
    case 1007:
        {
            RtcmAntennaDescriptor antennadescriptor;
            antennadescriptor=decodeAntennaDescriptor(data);
            emit signalRtcmAntennaDescriptor(antennadescriptor);
        }
        break;
    case 1033:
        {
            RtcmReceiverAntennaDescriptor receiverAntennaDescriptor;
            receiverAntennaDescriptor=decodeReceiverAntennaDescriptor(data);
            emit signalRtcmReceiverAntennaDescriptor(receiverAntennaDescriptor);
        }
        break;
    }
}

int Rtcm::decodeDataType(QByteArray & data)
{
    return fetchValue<int>(data,0, 12);
}

#define INITFETCHIDS(initStartBitId) int __startBitId=initStartBitId,__bitNum=0;
#define FETCHVALUE(value,type,data,bitNum) \
    __startBitId+=__bitNum;__bitNum=bitNum; \
    value=fetchValue<type>(data,__startBitId,__bitNum);

#define FETCHSTRING(string,len,type,data,bitNum) \
    FETCHVALUE(len,type,data,bitNum); \
    string.resize(len); \
    for(uint i=0;i<len;i++) {FETCHVALUE(string[i],char,data,8);}

RtcmGPSHeader Rtcm::decodeGpsHeader(QByteArray & data)
{
    RtcmGPSHeader header;
    INITFETCHIDS(0);
    FETCHVALUE(header.type,uint,data,12);
    FETCHVALUE(header.staid,uint,data,12);
    FETCHVALUE(header.tow,uint,data,30);
    FETCHVALUE(header.sync,bool,data,1);
    FETCHVALUE(header.nsat,uint,data,5);
    FETCHVALUE(header.sm,bool,data,1);
    FETCHVALUE(header.smint,char,data,3);
    return header;
}

RtcmGPSMessage Rtcm::decodeGpsMessage(RtcmGPSHeader & header, QByteArray & data)
{
    RtcmGPSMessage message;
    INITFETCHIDS(64);
    if(header.type>=1001)
    {
        FETCHVALUE(message.satId,uint,data,6);
        FETCHVALUE(message.gpsl1.code1,char,data,1);
        FETCHVALUE(message.gpsl1.pr1,uint,data,24);
        FETCHVALUE(message.gpsl1.ppr11,int,data,20);
        FETCHVALUE(message.gpsl1.lock1,uint,data,7);
    }
    if(header.type==1002||header.type==1004)
    {
        FETCHVALUE(message.gpsl1.amb,uint,data,8);
        FETCHVALUE(message.gpsl1.cnr1,uint,data,8);
    }
    if(header.type>=1003)
    {
        FETCHVALUE(message.gpsl2.code2,char,data,2);
        FETCHVALUE(message.gpsl2.pr21,int,data,14);
        FETCHVALUE(message.gpsl2.ppr21,int,data,20);
        FETCHVALUE(message.gpsl2.lock2,uint,data,7);
    }
    if(header.type==1004)
    {
        FETCHVALUE(message.gpsl2.cnr2,uint,data,8);
    }
    return message;
}

RtcmStationaryAntennaData Rtcm::decodeStationaryAntennaData(QByteArray &data)
{
    RtcmStationaryAntennaData stationaryantennadata;
    INITFETCHIDS(0);
    FETCHVALUE(stationaryantennadata.type,uint,data,12);
    FETCHVALUE(stationaryantennadata.staid,uint,data,12);
    FETCHVALUE(stationaryantennadata.itrf,uint,data,6);
    FETCHVALUE(stationaryantennadata.gpsind,bool,data,1);
    FETCHVALUE(stationaryantennadata.glonassind,bool,data,1);
    FETCHVALUE(stationaryantennadata.galileoind,bool,data,1);
    FETCHVALUE(stationaryantennadata.refstaind,bool,data,1);
    FETCHVALUE(stationaryantennadata.ecefx,qint64,data,38);
    FETCHVALUE(stationaryantennadata.sroind,bool,data,1);
    FETCHVALUE(stationaryantennadata.reserved,bool,data,1);
    FETCHVALUE(stationaryantennadata.ecefy,qint64,data,38);
    FETCHVALUE(stationaryantennadata.qcind,char,data,2);
    FETCHVALUE(stationaryantennadata.ecefz,qint64,data,38);
    if(stationaryantennadata.type==1006)
    {
        FETCHVALUE(stationaryantennadata.antheight,uint,data,16);
    }
    return stationaryantennadata;
}

RtcmAntennaDescriptor Rtcm::decodeAntennaDescriptor(QByteArray &data)
{
    RtcmAntennaDescriptor antennadescriptor;
    INITFETCHIDS(0);
    FETCHVALUE(antennadescriptor.type,uint,data,12);
    FETCHVALUE(antennadescriptor.staid,uint,data,12);
    FETCHSTRING(antennadescriptor.desc,antennadescriptor.descn,uint,data,8);
    FETCHVALUE(antennadescriptor.antsetupid,uint,data,8);
    return antennadescriptor;
}

RtcmReceiverAntennaDescriptor Rtcm::decodeReceiverAntennaDescriptor(QByteArray &data)
{
    RtcmReceiverAntennaDescriptor receiverAntennaDescriptor;
    INITFETCHIDS(0);
    FETCHVALUE(receiverAntennaDescriptor.antdesc.type,uint,data,12);
    FETCHVALUE(receiverAntennaDescriptor.antdesc.staid,uint,data,12);
    FETCHSTRING(receiverAntennaDescriptor.antdesc.desc,receiverAntennaDescriptor.antdesc.descn,uint,data,8);
    FETCHVALUE(receiverAntennaDescriptor.antdesc.antsetupid,uint,data,8);

    FETCHSTRING(receiverAntennaDescriptor.antserial,receiverAntennaDescriptor.antserialn,uint,data,8);
    FETCHSTRING(receiverAntennaDescriptor.recvtype,receiverAntennaDescriptor.recvtypen,uint,data,8);
    FETCHSTRING(receiverAntennaDescriptor.recvfirmware,receiverAntennaDescriptor.recvfirmwaren,uint,data,8);
    FETCHSTRING(receiverAntennaDescriptor.recvserial,receiverAntennaDescriptor.recvserialn,uint,data,8);
    return receiverAntennaDescriptor;
}


