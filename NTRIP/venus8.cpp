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
//    connect(this,SIGNAL(signalInitializeVenus8(int)),this,SLOT(slotInitializeVenus8(int)),Qt::QueuedConnection);
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

//void Venus8::initVenus8(int portId)
//{
//    QRegExp rx("(\\d+)");
//    int pos=rx.indexIn(portlist[portId].portName(),0);
//    if(pos>=0)
//    {
//        int port=rx.cap(1).toInt()-1;
//        QString hexmessage=QString("%1%2%3%4")
//                .arg("05")
//                .arg(port,2,16,QChar('0'))
//                .arg(maxvenus8baudrateid,2,16,QChar('0'))
//                .arg("00");
//        sendMessage(hexmessage.toUtf8());
//        emit signalInitializeVenus8(portId);
//    }
//}

//void Venus8::slotInitializeVenus8(int portId)
//{
//    this->setPort(portlist[portId]);
//    baudrate=defaultbaudrate;
//    this->setBaudRate(baudrate);
//    if(this->open(QIODevice::WriteOnly))
//    {
//        if(checkSendFlag())
//        {
//            this->write(message);
//            if(this->waitForBytesWritten(VENUS8WAIT))
//            {
//                emit signalMessageSent();
//            }
//            else
//            {
//                emit signalMessageNotSent();
//            }
//            mutex.lock();
//            sendflag=false;
//            mutex.unlock();
//        }
//        this->close();
//        baudrate=maxbaudrate;
//        emit signalVenus8Initialized();
//    }
//}

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




