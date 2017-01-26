#include "venus8.h"

#define VENUS8WAIT 3000

QByteArray Venus8::messagestart=QByteArray::fromHex("A0A1");
QByteArray Venus8::messageend=QByteArray::fromHex("0D0A");

Venus8::Venus8()
    : QSerialPort(Q_NULLPTR)
{
    receiveflag=false;
    sendflag=false;
    thread=new QThread();
    this->moveToThread(thread);
    connect(this,SIGNAL(signalLoadPortList()),this,SLOT(slotLoadPortList()),Qt::QueuedConnection);
    connect(this,SIGNAL(signalStartVenus8(int)),this,SLOT(slotStartVenus8(int)),Qt::QueuedConnection);
    thread->start();

    baudrate=QSerialPort::Baud9600;
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

void Venus8::sendMessage(QByteArray hexMessage)
{
    message.clear();

    message.append(messagestart);

    QByteArray raw=QByteArray::fromHex(hexMessage);
    int n=raw.size();
    message.append(char(n/0x100));
    message.append(char(n%0x100));
    message.append(raw);

    char cs=0;
    for(int i=0;i<n;i++)
    {
        char m=raw.at(i);
        cs=cs^m;
    }
    message.append(cs);

    message.append(messageend);

    mutex.lock();
    sendflag=true;
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

bool Venus8::checkSendFlag()
{
    bool result;
    mutex.lock();
    result=sendflag;
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
                QByteArray message=this->readLine();
                if(message.startsWith(messagestart))
                {
                    emit signalMessageReceived(message);
                }
                else
                {
                    emit signalNmeaReceived(message);
                    nmea_parse(&parser,message.data(),message.size(),&info);
                    emit signalNmeaParsed(info);
                }
            }
            if(checkSendFlag())
            {
                this->write(message);
                if(this->waitForBytesWritten(VENUS8WAIT))
                {
                    emit signalMessageSent();
                }
                else
                {
                    emit signalMessageNotSent();
                }
                mutex.lock();
                sendflag=false;
                mutex.unlock();
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




