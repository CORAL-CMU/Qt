#include "rtcm.h"

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
    int seg=rtcm.indexOf(QByteArray("\r\n"));

}
