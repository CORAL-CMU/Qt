#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHostAddress>
#include <QNetworkInterface>
#include <QAbstractSocket>

#define MAX

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString ip;
    int port=22345;
    foreach(const QHostAddress & address, QNetworkInterface::allAddresses())
    {
        if(address.protocol()==QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
        {
            ip=address.toString();
            break;
        }
    }
    ip=QString("128.237.203.30");
    QLabel * serverip=new QLabel(QString("Server IP Address & Port: %1:%2").arg(ip).arg(port));
    ui->statusBar->addWidget(serverip);
    interface=new QGMapInterface("RNDF Server",QHostAddress(ip),port,this);

    QWebEngineView *view = new QWebEngineView();
    ui->tabWidget->addTab(view,"Client");
    view->load(QUrl("qrc:///html/qgmapinterface_embedded.html"));
    view->show();

    source=new QNmeaPositionInfoSource(QNmeaPositionInfoSource::SimulationMode);
    file=new QFile();
    savefile=new QFile();

    connect(source,SIGNAL(positionUpdated(QGeoPositionInfo)),this,SLOT(slotPositionUpdated(QGeoPositionInfo)));
    connect(this,SIGNAL(signalFinishLoading()),this,SLOT(slotFinishLoading()));

    polyline.id=0;
    polylineconfig=QGMapPolylineConfig("red",3,0.5);
    markerconfig=QGMapMarkerConfig(3,"black",0,"blue",0.5);

    ui->pushButton->setDisabled(true);

    timer=new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(slotPlayPosition()));
}

MainWindow::~MainWindow()
{
    source->stopUpdates();
    delete source;
    if(file->isOpen())
    {
        file->close();
    }
    delete file;
    if(savefile->isOpen())
    {
        savefile->close();
    }
    delete savefile;
    timer->stop();
    delete timer;
    delete ui;
}

void MainWindow::on_action_Open_triggered()
{
    if(file->isOpen())
    {
        return;
    }
    QString filename=QFileDialog::getOpenFileName(this,"Load NMEA File",QString(),QString("NMEA (*.nmea)"));
    if(!filename.isEmpty())
    {
        file->setFileName(filename);
        source->setDevice(file);
        source->startUpdates();
        //ui->pushButton->setDisabled(false);

        savefile->setFileName(filename+".txt");
        savefile->open(QIODevice::WriteOnly|QIODevice::Text);
        stream.setDevice(savefile);
    }
}

void MainWindow::slotPositionUpdated(const QGeoPositionInfo &update)
{
    if(update.isValid())
    {
        positions.push_back(update);
        QGMapMarker marker(markers.size(),"",update.timestamp().toString("yyyy-MM-dd HH:mm:ss:zzz"),QGMapPointF(update.coordinate().latitude(),update.coordinate().longitude()));
        markers.push_back(marker);
        polyline.vertices.push_back(QGMapPointF(update.coordinate().latitude(),update.coordinate().longitude()));
        stream<<update.timestamp().toString("yyyy-MM-dd_HH:mm:ss:zzz")<<"\t"
             <<QString::number(update.coordinate().latitude(),'f',15)<<"\t"
             <<QString::number(update.coordinate().longitude(),'f',15)<<"\t"
             <<QString::number(update.coordinate().altitude(),'f',15)<<"\t"
            <<QString::number(update.attribute(QGeoPositionInfo::HorizontalAccuracy),'f',15)<<"\t"
             <<QString::number(update.attribute(QGeoPositionInfo::VerticalAccuracy),'f',15)<<"\n";
    }
}

void MainWindow::slotFinishLoading()
{
    QString filename=QFileDialog::getSaveFileName(this,"Save Position File");
    if(!filename.isEmpty())
    {
        file->setFileName(filename);
        if(file->open(QIODevice::WriteOnly|QIODevice::Text))
        {
            QTextStream stream(file);
            for(int i=0;i<positions.size();i++)
            {
                stream<<positions[i].timestamp().toString("yyyy-MM-dd HH:mm:ss:zzz")<<"\t"
                     <<QString::number(positions[i].coordinate().latitude(),'f',15)<<"\t"
                     <<QString::number(positions[i].coordinate().longitude(),'f',15)<<"\t"
                     <<QString::number(positions[i].coordinate().altitude(),'f',15)<<"\n";
            }
            file->close();
        }
    }
}

void MainWindow::slotPlayPosition()
{

}

void MainWindow::on_pushButton_clicked()
{
    if(ui->pushButton->text()=="Stop")
    {
        source->stopUpdates();
        ui->pushButton->setText("Start");
    }
    else
    {
        source->startUpdates();
        ui->pushButton->setText("Stop");
    }
}

void MainWindow::on_action_Play_Positions_triggered()
{
    //timer->start(10);
    //posid=0;
    //interface->setMarker(markers,markerconfig,"");
    interface->setPolyline(polyline,polylineconfig,"");
}
