#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_encode_clicked();

    QList<QString> hostiplist;
    foreach(const QHostAddress & address, QNetworkInterface::allAddresses())
    {
        if(address.protocol()==QAbstractSocket::IPv4Protocol && address!=QHostAddress(QHostAddress::LocalHost))
        {
            hostiplist.push_back(address.toString());
        }
    }
    QString ip=QInputDialog::getItem(this,"Select Host IP Address","Host IP Address",hostiplist);
    if(ip.isEmpty())
    {
        exit(0);
    }
    int port=QInputDialog::getInt(this,"Set Server Port","Port",12345,1025,65535);

    QLineEdit * serverip=new QLineEdit(QString("%1").arg(ip));
    QLineEdit * serverport=new QLineEdit(QString("%1").arg(port));
    serverip->setReadOnly(true);
    serverport->setReadOnly(true);
    ui->statusBar->addWidget(serverip);
    ui->statusBar->addWidget(serverport);
    interface=new QGMapInterface("NMEA Viewer", QHostAddress(ip),port,this);

    connect(&ntrip,SIGNAL(signalCasterListLoaded()),this,SLOT(slotCasterListLoaded()));
    connect(&ntrip,SIGNAL(signalRtkReceived(QByteArray)),this,SLOT(slotRtkReceived(QByteArray)));
    connect(&ntrip,SIGNAL(signalRtkEnd()),this,SLOT(slotRtkEnd()));
    connect(&ntrip,SIGNAL(signalRtkReceived(QByteArray)),&rtcm,SLOT(slotDecodeRtcm(QByteArray)));

    connect(&venus8,SIGNAL(signalPortListLoaded()),this,SLOT(slotPortListLoaded()));
    connect(&venus8,SIGNAL(signalNmeaReceived(QByteArray)),this,SLOT(slotNmeaReceived(QByteArray)));
    connect(&venus8,SIGNAL(signalNmeaParsed(nmeaINFO)),this,SLOT(slotNmeaParsed(nmeaINFO)));
    connect(&venus8,SIGNAL(signalVenus8Stopped()),this,SLOT(slotVenus8Stopped()));
    connect(&venus8,SIGNAL(signalVenus8ConnectionError()),this,SLOT(slotVenus8ConnectionError()));

    connect(&logger,SIGNAL(signalLogFilenameSet()),this,SLOT(slotLogFilenameSet()));
    connect(&venus8,SIGNAL(signalNmeaReceived(QByteArray)),&logger,SLOT(slotLogNmea(QByteArray)));

    connect(&venus8,SIGNAL(signalMessageSent()),this,SLOT(slotMessageSent()));
    connect(&venus8,SIGNAL(signalMessageNotSent()),this,SLOT(slotMessageNotSent()));
    connect(&venus8,SIGNAL(signalMessageReceived(QByteArray)),this,SLOT(slotMessageReceived(QByteArray)));

    connect(&rtcm,SIGNAL(signalRtcmGpsData(RtcmGPSData)),this,SLOT(slotRtcmGpsData(RtcmGPSData)));
    connect(&rtcm,SIGNAL(signalRtcmStationaryAntennaData(RtcmStationaryAntennaData)),this,SLOT(slotRtcmStationaryAntennaData(RtcmStationaryAntennaData)));
    connect(&rtcm,SIGNAL(signalRtcmAntennaDescriptor(RtcmAntennaDescriptor)),this,SLOT(slotRtcmAntennaDescriptor(RtcmAntennaDescriptor)));
    connect(&rtcm,SIGNAL(signalRtcmReceiverAntennaDescriptor(RtcmReceiverAntennaDescriptor)),this,SLOT(slotRtcmReceiverAntennaDescriptor(RtcmReceiverAntennaDescriptor)));

    marker.id=0;
    markerconfig.fillColor="red";
    markerconfig.fillOpacity=0.7;
    markerconfig.scale=3;
    markerconfig.strokeColor="black";
    markerconfig.strokeWeight=1;
    connect(interface,SIGNAL(signalClientIdConfirmed(QString)),this,SLOT(slotClientIdConfirmed(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_decode_clicked()
{
    ntrip.decodeNtripPath(ui->ntripaddr->text());
    ui->addr->setText(ntrip.addr);
    ui->port->setText(ntrip.port);
    ui->mntpnt->setText(ntrip.mntpnt);
    ui->mntstr->setText(ntrip.mntstr);
    ui->user->setText(ntrip.user);
    ui->pwd->setText(ntrip.passwd);
    ui->ntripaddr->setText(ntrip.ntrippath);
}

void MainWindow::on_encode_clicked()
{
    ntrip.addr=ui->addr->text();
    ntrip.port=ui->port->text();
    ntrip.mntpnt=ui->mntpnt->text();
    ntrip.mntstr=ui->mntstr->text();
    ntrip.user=ui->user->text();
    ntrip.passwd=ui->pwd->text();
    ntrip.encodeNtripPath();
    ui->ntripaddr->setText(ntrip.ntrippath);
}

void MainWindow::on_loadcasters_clicked()
{
    ntrip.loadCasterList();
}

void MainWindow::on_requestrtk_clicked()
{
    if(ui->requestrtk->text()==QString("Request RTK"))
    {
        int casterid=ui->casters->currentRow();
        if(casterid>=0)
        {
            ui->mntpnt->setText(ntrip.casterlist[casterid].mountpoint);
            ntrip.startReceiveRtk(casterid,ui->GPGGA->text());
            ui->requestrtk->setText("Stop RTK");
        }
    }
    else
    {
        ntrip.stopReceiveRtk();
        ui->requestrtk->setText("Request RTK");
    }
}

void MainWindow::on_clear_clicked()
{
    ui->rawshow->clear();
    ui->rtcminfo->clear();
}

void MainWindow::on_loadports_clicked()
{
    venus8.loadPortList();
}

void MainWindow::on_startvenus8_clicked()
{
    if(ui->startvenus8->text()==QString("Start Venus8"))
    {
        venus8.baudrate=QSerialPort::BaudRate(ui->baudrate->text().toInt());
        int portid=ui->portlist->currentRow();
        if(portid>=0)
        {
            venus8.startReceiveNmea(portid);
            ui->startvenus8->setText("Stop Venus8");
        }
    }
    else
    {
        venus8.stopReceiveNmea();
        ui->startvenus8->setText("Start Venus8");
    }
}

void MainWindow::on_clearnmea_clicked()
{
    ui->nmea->clear();
}

void MainWindow::on_sendmessage_clicked()
{
    QByteArray hexMessage=ui->message->text().toUtf8();
    venus8.sendMessage(hexMessage);
}

void MainWindow::slotCasterListLoaded()
{
    ui->casters->clearContents();
    ui->casters->setRowCount(ntrip.casterlist.size());
    for(int i=0;i<ntrip.casterlist.size();i++)
    {
        int j=0;
        ui->casters->setItem(i,j++,new QTableWidgetItem(ntrip.casterlist[i].type));
        ui->casters->setItem(i,j++,new QTableWidgetItem(ntrip.casterlist[i].mountpoint));
        ui->casters->setItem(i,j++,new QTableWidgetItem(ntrip.casterlist[i].identifier));
        ui->casters->setItem(i,j++,new QTableWidgetItem(ntrip.casterlist[i].format));
        ui->casters->setItem(i,j++,new QTableWidgetItem(ntrip.casterlist[i].formatdetails));
        ui->casters->setItem(i,j++,new QTableWidgetItem(QString::number(ntrip.casterlist[i].carrier)));
        ui->casters->setItem(i,j++,new QTableWidgetItem(ntrip.casterlist[i].navsystem));
        ui->casters->setItem(i,j++,new QTableWidgetItem(ntrip.casterlist[i].network));
        ui->casters->setItem(i,j++,new QTableWidgetItem(ntrip.casterlist[i].country));
        ui->casters->setItem(i,j++,new QTableWidgetItem(QString::number(ntrip.casterlist[i].latitude)));
        ui->casters->setItem(i,j++,new QTableWidgetItem(QString::number(ntrip.casterlist[i].longitude)));
        ui->casters->setItem(i,j++,new QTableWidgetItem(QString::number(ntrip.casterlist[i].nmea)));
        ui->casters->setItem(i,j++,new QTableWidgetItem(QString::number(ntrip.casterlist[i].solution)));
        ui->casters->setItem(i,j++,new QTableWidgetItem(ntrip.casterlist[i].generator));
        ui->casters->setItem(i,j++,new QTableWidgetItem(ntrip.casterlist[i].compression));
        ui->casters->setItem(i,j++,new QTableWidgetItem(ntrip.casterlist[i].authentication));
        ui->casters->setItem(i,j++,new QTableWidgetItem(ntrip.casterlist[i].fee));
        ui->casters->setItem(i,j++,new QTableWidgetItem(QString::number(ntrip.casterlist[i].bitrate)));
        ui->casters->setItem(i,j++,new QTableWidgetItem(ntrip.casterlist[i].misc));
    }
}


void MainWindow::slotRtkReceived(QByteArray rtk)
{
    QString content=QString("[%1]\n%2")
            .arg(QTime::currentTime().toString("HH:mm:ss:zzz"))
            .arg(ui->hex->isChecked()?QString(rtk.toHex()):QString(rtk));
    ui->rawshow->append(content);
}

void MainWindow::slotRtkEnd()
{
    ntrip.stopReceiveRtk();
    ui->requestrtk->setText("Request RTK");
}

void MainWindow::slotPortListLoaded()
{
    ui->portlist->clearContents();
    ui->portlist->setRowCount(venus8.portlist.size());
    for(int i=0;i<venus8.portlist.size();i++)
    {
        int j=0;
        ui->portlist->setItem(i,j++,new QTableWidgetItem(venus8.portlist[i].portName()));
        ui->portlist->setItem(i,j++,new QTableWidgetItem(venus8.portlist[i].description()));
        ui->portlist->setItem(i,j++,new QTableWidgetItem(venus8.portlist[i].manufacturer()));
        ui->portlist->setItem(i,j++,new QTableWidgetItem(venus8.portlist[i].serialNumber()));
        ui->portlist->setItem(i,j++,new QTableWidgetItem(venus8.portlist[i].systemLocation()));
    }
}

void MainWindow::slotNmeaReceived(QByteArray nmea)
{
    QString content=QString("[%1]\n%2")
            .arg(QTime::currentTime().toString("HH:mm:ss:zzz"))
            .arg(QString(nmea));
    ui->nmea->append(content);
}

void MainWindow::slotNmeaParsed(nmeaINFO info)
{
    QString content=QString("smask:\t%1\n"
                            "utc:\t%2\n"
                            "sig:\t%3\n"
                            "fix:\t%3\n"
                            "PDOP:\t%4\n"
                            "HDOP:\t%5\n"
                            "VDOP:\t%6\n"
                            "lat:\t%7\n"
                            "lon:\t%8\n"
                            "elv:\t%9\n"
                            "speed:\t%10\n"
                            "direc:\t%11\n"
                            "decli:\t%12\n"
                            "inuse:\t%13\n"
                            "inview:\t%14\n")
                            .arg(info.smask)
                            .arg(QString("%1-%2-%3 %4:%5:%6:%7")
                                 .arg(info.utc.year+1900)
                                 .arg(info.utc.mon)
                                 .arg(info.utc.day)
                                 .arg(info.utc.hour)
                                 .arg(info.utc.min)
                                 .arg(info.utc.sec)
                                 .arg(info.utc.hsec))
                            .arg(info.sig)
                            .arg(info.PDOP)
                            .arg(info.HDOP)
                            .arg(info.VDOP)
                            .arg(info.lat)
                            .arg(info.lon)
                            .arg(info.elv)
                            .arg(info.speed)
                            .arg(info.direction)
                            .arg(info.declination)
                            .arg(info.satinfo.inuse)
                            .arg(info.satinfo.inview);
    ui->parsednmea->setText(content);
    for(int i=0;i<info.satinfo.inview;i++)
    {
        QString satinfo=QString("Sat#%1:\t%2\t%3\t%4\t%5\t%6\n")
                .arg(i)
                .arg(info.satinfo.sat[i].id)
                .arg(info.satinfo.sat[i].in_use)
                .arg(info.satinfo.sat[i].elv)
                .arg(info.satinfo.sat[i].azimuth)
                .arg(info.satinfo.sat[i].sig);
        ui->parsednmea->append(satinfo);
    }

    marker.position=QGMapPointF(convertNDEGToDegree(info.lat),convertNDEGToDegree(info.lon));
    interface->setMarker(marker,markerconfig,"");
}

void MainWindow::slotVenus8Stopped()
{
    venus8.stopReceiveNmea();
    ui->nmea->append("Venus8 Stopped!");
    ui->startvenus8->setText("Start Venus8");
}

void MainWindow::slotVenus8ConnectionError()
{
    ui->nmea->append("Venus8 Connection Error!");
    ui->startvenus8->setText("Start Venus8");
}

void MainWindow::slotMessageSent()
{
    ui->messageshow->append("Venus8 Message Sent");
}

void MainWindow::slotMessageNotSent()
{
    ui->messageshow->append("Venus8 Message Not Sent");
}

void MainWindow::slotMessageReceived(QByteArray message)
{
    ui->messageshow->append(message.toHex());
}

void MainWindow::slotLogFilenameSet()
{
    ui->logfile->setText(logger.filename);
}

void MainWindow::slotRtcmGpsData(RtcmGPSData gpsData)
{
    QString content=QString("type:\t%1\n"
                           "staid:\t%2\n"
                           "tow:\t%3\n"
                           "sync:\t%4\n"
                           "nsat:\t%5\n"
                           "sm:\t%6\n"
                           "smint:\t%7\n"
                           "satid:\t%8\n"
                           "GPS1\n"
                           "code1:\t%9\n"
                           "pr1:\t%10\n"
                           "ppr11:\t%11\n"
                           "lock1:\t%12\n"
                           "amb:\t%13\n"
                           "cnr1:\t%14\n"
                           "GPS2\n"
                           "code2:\t%15\n"
                           "pr21:\t%16\n"
                           "ppr21:\t%17\n"
                           "lock2:\t%18\n"
                           "cnr2:\t%19\n")
            .arg(gpsData.header.type).arg(gpsData.header.staid).arg(gpsData.header.tow).arg(gpsData.header.sync).arg(gpsData.header.nsat).arg(gpsData.header.sm).arg(int(gpsData.header.smint))
            .arg(gpsData.message.satId)
            .arg(int(gpsData.message.gpsl1.code1)).arg(gpsData.message.gpsl1.pr1).arg(gpsData.message.gpsl1.ppr11).arg(gpsData.message.gpsl1.lock1).arg(gpsData.message.gpsl1.amb).arg(gpsData.message.gpsl1.cnr1)
            .arg(int(gpsData.message.gpsl2.code2)).arg(gpsData.message.gpsl2.pr21).arg(gpsData.message.gpsl2.ppr21).arg(gpsData.message.gpsl2.lock2).arg(gpsData.message.gpsl2.cnr2);
    ui->rtcminfo->append(content);
}

void MainWindow::slotRtcmStationaryAntennaData(RtcmStationaryAntennaData stationaryAntennaData)
{
    QString content=QString("type:\t%1\n"
                            "staid:\t%2\n"
                            "itrf:\t%3\n"
                            "ecefx:\t%4\n"
                            "ecefy:\t%5\n"
                            "ecefz:\t%6\n")
            .arg(stationaryAntennaData.type).arg(stationaryAntennaData.staid).arg(stationaryAntennaData.itrf)
            .arg(stationaryAntennaData.ecefx).arg(stationaryAntennaData.ecefy).arg(stationaryAntennaData.ecefz);
    ui->rtcminfo->append(content);
}

void MainWindow::slotRtcmAntennaDescriptor(RtcmAntennaDescriptor antennaDescriptor)
{
    QString content=QString("type:\t%1\n"
                            "staid:\t%2\n"
                            "desc:\t%3\n"
                            "antsetupid:\t%4\n")
            .arg(antennaDescriptor.type).arg(antennaDescriptor.staid)
            .arg(antennaDescriptor.desc).arg(antennaDescriptor.antsetupid);
    ui->rtcminfo->append(content);
}

void MainWindow::slotRtcmReceiverAntennaDescriptor(RtcmReceiverAntennaDescriptor receiverAntennaDescriptor)
{
    QString content=QString("type:\t%1\n"
                            "staid:\t%2\n"
                            "desc:\t%3\n"
                            "antsetupid:\t%4\n"
                            "antserial:\t%5\n"
                            "recvtype:\t%6\n"
                            "recvfirmware:\t%7\n"
                            "recvserial:\t%8\n")
            .arg(receiverAntennaDescriptor.antdesc.type)
            .arg(receiverAntennaDescriptor.antdesc.staid)
            .arg(receiverAntennaDescriptor.antdesc.desc)
            .arg(receiverAntennaDescriptor.antdesc.antsetupid)
            .arg(receiverAntennaDescriptor.antserial)
            .arg(receiverAntennaDescriptor.recvtype)
            .arg(receiverAntennaDescriptor.recvfirmware)
            .arg(receiverAntennaDescriptor.recvserial);
    ui->rtcminfo->append(content);
}

void MainWindow::on_setlogfile_clicked()
{
    logger.setLogFilename();
}

void MainWindow::on_startlog_clicked()
{
    if(ui->startlog->text()==QString("Start"))
    {
        ui->startlog->setText("Stop");
        logger.startLogNmea();
    }
    else
    {
        ui->startlog->setText("Start");
        logger.stopLogNmea();
    }
}

void MainWindow::slotClientIdConfirmed(QString clientId)
{
    interface->setMarker(marker,markerconfig,clientId);
}
