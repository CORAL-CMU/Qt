#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QNetworkInterface>
#include <QLineEdit>
#include <QFileDialog>
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString ip;
    int port=12345;
    foreach(const QHostAddress & address, QNetworkInterface::allAddresses())
    {
        if(address.protocol()==QAbstractSocket::IPv4Protocol && address!=QHostAddress(QHostAddress::LocalHost))
        {
            ip=address.toString();
            break;
        }
    }
    QLineEdit * serverip=new QLineEdit(QString("%1:%2").arg(ip).arg(port));
    serverip->setReadOnly(true);
    ui->statusBar->addWidget(serverip);
    interface=new QGMapInterface("NMEA Viewer", QHostAddress(ip),port,this);

    connect(interface,SIGNAL(signalClientIdConfirmed(QString)),this,SLOT(slotClientIdConfirmed(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

double convertNDEGToDegree(double ndeg)
{
    bool flag=ndeg<0;
    if(flag)
    {
        ndeg=-ndeg;
    }
    int degmin=int(ndeg);
    double sec=(ndeg-degmin)*60.0;
    int min=degmin%100;
    int deg=degmin/100;
    double result=deg+min/60.0+sec/3600.0;
    if(flag)
    {
        result=-result;
    }
    return result;
}

void MainWindow::on_open_clicked()
{
    QString filename=QFileDialog::getOpenFileName(this,"Open NMEA File",QString(),QString("NMEA (*.nmea);;LatLng (*.txt)"));
    if(!filename.isEmpty())
    {
        QFile file(filename);
        if(file.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            polyline.id=polylines.size();
            polyline.vertices.clear();
            if(filename.endsWith("nmea"))
            {
                nmeaPARSER parser;
                nmea_parser_init(&parser);
                nmeaINFO info;
                nmea_zero_INFO(&info);
                while(!file.atEnd())
                {
                    QByteArray sentence=file.readLine();
                    nmea_parse(&parser,sentence.data(),sentence.size(),&info);
                    if(!(info.lat==0&&info.lon==0))
                    {
                        polyline.vertices.push_back(QGMapPointF(convertNDEGToDegree(info.lat),convertNDEGToDegree(info.lon)));
                    }
                }

                nmea_parser_destroy(&parser);
            }
            else
            {
                while(!file.atEnd())
                {
                    QString data=file.readLine();
                    QList<QString> tmpdata=data.split("\t",QString::SkipEmptyParts);
                    if(tmpdata.size()>=3)
                    {
                        polyline.vertices.push_back(QGMapPointF(tmpdata[1].toDouble(),tmpdata[2].toDouble()));
                    }
                }
            }
            file.close();

            QColor color=QColorDialog::getColor(Qt::blue,this,"Set Path Color");
            polylineconfig.strokeColor=color.name();

            ui->nmeas->addItem(filename);
            polylines.push_back(polyline);
            polylineconfigs.push_back(polylineconfig);

            QJsonObject object=polyline.toJsonObject();
            QGMapPolyline tmppolyline;
            tmppolyline.fromJsonObject(object);

            interface->setPolyline(polyline,polylineconfig,"");
        }
    }
}

void MainWindow::slotClientIdConfirmed(QString clientId)
{
    for(int i=0;i<polylines.size();i++)
    {
        interface->setPolyline(polylines,polylineconfigs,clientId);
    }
}
