#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_encode_clicked();

    connect(&ntrip,SIGNAL(signalCasterListLoaded()),this,SLOT(slotCasterListLoaded()));
    connect(&ntrip,SIGNAL(signalRtkReceived(QByteArray)),this,SLOT(slotRtkReceived(QByteArray)));
    connect(&ntrip,SIGNAL(signalRtkEnd()),this,SLOT(slotRtkEnd()));

    ui->baudrate->addItem(QString::number(int(QSerialPort::Baud1200)));
    ui->baudrate->addItem(QString::number(int(QSerialPort::Baud2400)));
    ui->baudrate->addItem(QString::number(int(QSerialPort::Baud4800)));
    ui->baudrate->addItem(QString::number(int(QSerialPort::Baud9600)));
    ui->baudrate->addItem(QString::number(int(QSerialPort::Baud19200)));
    ui->baudrate->addItem(QString::number(int(QSerialPort::Baud38400)));
    ui->baudrate->addItem(QString::number(int(QSerialPort::Baud57600)));
    ui->baudrate->addItem(QString::number(int(QSerialPort::Baud115200)));

    connect(&venus8,SIGNAL(signalPortListLoaded()),this,SLOT(slotPortListLoaded()));
    connect(&venus8,SIGNAL(signalNmeaReceived(QByteArray)),this,SLOT(slotNmeaReceived(QByteArray)));
    connect(&venus8,SIGNAL(signalNmeaParsed(nmeaINFO)),this,SLOT(slotNmeaParsed(nmeaINFO)));
    connect(&venus8,SIGNAL(signalVenus8Stopped()),this,SLOT(slotVenus8Stopped()));
    connect(&venus8,SIGNAL(signalVenus8ConnectionError()),this,SLOT(slotVenus8ConnectionError()));
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
    ui->hexshow->clear();
}

void MainWindow::on_loadports_clicked()
{
    venus8.loadPortList();
}

void MainWindow::on_startvenus8_clicked()
{
    if(ui->startvenus8->text()==QString("Start Venus8"))
    {
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
            .arg(QString(rtk));
    ui->rawshow->append(content);
    QString contenthex=QString("[%1]\n%2")
            .arg(QTime::currentTime().toString("HH:mm:ss:zzz"))
            .arg(QString(rtk.toHex()));
    ui->hexshow->append(contenthex);
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
