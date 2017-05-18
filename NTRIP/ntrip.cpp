#include "ntrip.h"

#define NTRIPWAIT 30000

Ntrip::Ntrip()
    : QTcpSocket(Q_NULLPTR)
{
    receiveflag=false;
    thread=new QThread();
    this->moveToThread(thread);
    connect(this,SIGNAL(signalLoadCasterList()),this,SLOT(slotLoadCasterList()),Qt::QueuedConnection);
    connect(this,SIGNAL(signalConnectRtk()),this,SLOT(slotConnectRtk()),Qt::QueuedConnection);
    connect(this,SIGNAL(signalDisconnectRtk()),this,SLOT(slotDisconnectRtk()),Qt::QueuedConnection);
    connect(this,SIGNAL(signalRequestRtk(int,QString)),this,SLOT(slotRequestRtk(int,QString)),Qt::QueuedConnection);
    thread->start();
}

Ntrip::~Ntrip()
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
}

void Ntrip::decodeNtripPath(QString path)
{
    QRegExp rx;
    QStringList texts;
    int pos;
    if(path.contains('@'))
    {
        rx.setPattern("^(\\w+):?(\\w*)@([\\w|.]+):?(\\w*)/?(\\w*):?(\\w*)$");
        pos=rx.indexIn(path);
        texts=rx.capturedTexts();
        user=texts.at(1);
        passwd=texts.at(2);
        addr=texts.at(3);
        port=texts.at(4);
        mntpnt=texts.at(5);
        mntstr=texts.at(6);
    }
    else
    {
        rx.setPattern("^([\\w|.]+):?(\\w*)/?(\\w*):?(\\w*)$");
        pos=rx.indexIn(path);
        texts=rx.capturedTexts();
        user="";
        passwd="";
        addr=texts.at(1);
        port=texts.at(2);
        mntpnt=texts.at(3);
        mntstr=texts.at(4);
    }
    encodeNtripPath();
}

void Ntrip::encodeNtripPath()
{
    ntrippath=QString("%1%2%3%4%5%6%7")
            .arg(user.isEmpty()?QString(""):user)
            .arg(passwd.isEmpty()?QString(""):(QString(":")+passwd))
            .arg(user.isEmpty()?QString(""):QString("@"))
            .arg(addr)
            .arg(port.isEmpty()?QString(""):(QString(":")+port))
            .arg(mntpnt.isEmpty()?QString(""):(QString("/")+mntpnt))
            .arg(mntstr.isEmpty()?QString(""):(QString(":")+mntstr));
}

void Ntrip::loadCasterList()
{
    emit signalLoadCasterList();
}

void Ntrip::slotLoadCasterList()
{
    casterlist.clear();
    connectToHost(addr,port.toUInt());
    bool flag=this->waitForConnected(NTRIPWAIT);
    if(flag)
    {
        QString message=QString("GET %1/%2 HTTP/1.1\r\n"
                                "Ntrip-Version: Ntrip/2.0\r\n"
                                "User-Agent: NTRIP %3\r\n"
                                "Connection: close\r\n\r\n")
                .arg("").arg(mntpnt).arg("CMU-CORAL");
        this->write(message.toUtf8());
        if(this->waitForReadyRead(NTRIPWAIT))
        {
            while(this->canReadLine())
            {
                QString rawdata=this->readLine();
                qInfo(rawdata.toUtf8().data());
                if(rawdata.startsWith("STR"))
                {
                    QList<QString> data=rawdata.split(";");
                    NtripCasterStr caster;
                    int i=0;
                    caster.type=data.at(i++);
                    caster.mountpoint=data.at(i++);
                    caster.identifier=data.at(i++);
                    caster.format=data.at(i++);
                    caster.formatdetails=data.at(i++);
                    caster.carrier=data.at(i++).toInt();
                    caster.navsystem=data.at(i++);
                    caster.network=data.at(i++);
                    caster.country=data.at(i++);
                    caster.latitude=data.at(i++).toDouble();
                    caster.longitude=data.at(i++).toDouble();
                    caster.nmea=data.at(i++).toInt();
                    caster.solution=data.at(i++).toInt();
                    caster.generator=data.at(i++);
                    caster.compression=data.at(i++);
                    caster.authentication=data.at(i++);
                    caster.fee=data.at(i++);
                    caster.bitrate=data.at(i++).toInt();
                    caster.misc=data.at(i++);

                    casterlist.push_back(caster);
                }
            }
        }
        this->disconnectFromHost();
        emit signalCasterListLoaded();
    }
    else
    {
        qWarning(QString("Cannot connect to %1:%2").arg(addr).arg(port).toUtf8().data());
    }
}

void Ntrip::connectRtk()
{
    emit signalConnectRtk();
}

void Ntrip::disconnectRtk()
{
    emit signalDisconnectRtk();
}

void Ntrip::slotConnectRtk()
{
    if(this->state()==QTcpSocket::UnconnectedState)
    {
        connectToHost(addr,port.toUInt());
        this->waitForConnected(NTRIPWAIT);
    }
}

void Ntrip::slotDisconnectRtk()
{
    if(this->state()==QTcpSocket::ConnectedState)
    {
        disconnectFromHost();
    }
}

void Ntrip::startReceiveRtk(int casterId, QString GPGGA)
{
    mutex.lock();
    receiveflag=true;
    mutex.unlock();
    connectRtk();
    emit signalRequestRtk(casterId,GPGGA);
}

void Ntrip::stopReceiveRtk()
{
    mutex.lock();
    receiveflag=false;
    mutex.unlock();
    disconnectRtk();
}

bool Ntrip::checkReceiveFlag()
{
    bool result;
    mutex.lock();
    result=receiveflag;
    mutex.unlock();
    return result;
}

void Ntrip::slotRequestRtk(int casterId, QString GPGGA)
{
    QString userpwd=QString("%1:%2").arg(user).arg(passwd);
    QString base64=userpwd.toUtf8().toBase64();
    QString message=QString("GET %1/%2 HTTP/1.1\r\n"
                            "Host: 209.255.196.164\r\n"
                            "Ntrip-Version: Ntrip/2.0\r\n"
                            "User-Agent: NTRIP %3\r\n"
                            "Authorization: Basic %4\r\n"
                            "Connection: close\r\n"
                            "\r\n"
                            "%5\r\n"
                            )
            .arg("").arg(casterlist[casterId].mountpoint).arg("CMU-CORAL").arg(base64).arg(GPGGA);
    this->write(message.toUtf8());

    while(checkReceiveFlag()&&this->waitForReadyRead(NTRIPWAIT))
    {
        QByteArray rtk=this->readAll();
        emit signalRtkReceived(rtk);
    }
    if(checkReceiveFlag())
    {
        emit signalRtkEnd();
    }
}
