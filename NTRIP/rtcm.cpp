#include "rtcm.h"

#define RTCM3PREAMBLE "D3"

Rtcm::Rtcm()
    : QObject(Q_NULLPTR)
{
    thread=new QThread();
    this->moveToThread(thread);
    thread->start();
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
    QList<QByteArray> messages;
    while(data.startsWith(char(0xD3)))
    {
        qint16 len=0;
        //        len|=(qint16(data.at(1)))<<8;
        //        len|=(qint16(data.at(2)));
        //const qint8 * src=(const qint8*)(data.data()+1);
        //len=qFromBigEndian();
        memcpy(&len,data.data()+1,2);
        len=qFromBigEndian(len);
        QByteArray message=data.mid(3,len);
        messages.push_back(message);
        data=data.mid(3+len+3);
    }
}
