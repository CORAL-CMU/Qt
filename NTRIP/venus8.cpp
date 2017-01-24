#include "venus8.h"

#define VENUS8WAIT 3000

Venus8::Venus8()
    : QSerialPort(Q_NULLPTR)
{
    receiveflag=false;
    thread=new QThread();
    this->moveToThread(thread);
    connect(this,SIGNAL(signalLoadPortList()),this,SLOT(slotLoadPortList()),Qt::QueuedConnection);
    connect(this,SIGNAL(signalStartVenus8(int)),this,SLOT(slotStartVenus8(int)),Qt::QueuedConnection);
    thread->start();

    baudrate=Baud9600;
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

    qRegisterMetaType<nmeaINFO>("nmeaINFO");
}

Venus8::~Venus8()
{
    if(receiveflag)
    {
        mutex.lock();
        receiveflag=false;
        mutex.unlock();
    }
    thread->exit();
    thread->wait();
    delete thread;

    nmea_parser_destroy(&parser);
}

void Venus8::loadPortList()
{
    emit signalLoadPortList();
}

void Venus8::slotLoadPortList()
{
    portlist=QSerialPortInfo::availablePorts();
    emit signalPortListLoaded();
}

void Venus8::startReceiveNmea(int portId)
{
    mutex.lock();
    receiveflag=true;
    mutex.unlock();
    emit signalStartVenus8(portId);
}

void Venus8::stopReceiveNmea()
{
    mutex.lock();
    receiveflag=false;
    mutex.unlock();
}

bool Venus8::checkReceiveFlag()
{
    bool result;
    mutex.lock();
    result=receiveflag;
    mutex.unlock();
    return result;
}

void Venus8::slotStartVenus8(int portId)
{
    this->setPort(portlist[portId]);
    this->setBaudRate(baudrate);
    if(this->open(QIODevice::ReadWrite))
    {
        while(checkReceiveFlag()&&this->waitForReadyRead(VENUS8WAIT))
        {
            while(this->canReadLine())
            {
                QByteArray nmea=this->readLine();
                nmea_parse(&parser,nmea.data(),nmea.size(),&info);
                emit signalNmeaReceived(nmea);
                emit signalNmeaParsed(info);
            }
        }
        this->close();
        if(checkReceiveFlag())
        {
            emit signalVenus8Stopped();
        }
    }
    else
    {
        mutex.lock();
        receiveflag=false;
        mutex.unlock();
        emit signalVenus8ConnectionError();
    }
}
