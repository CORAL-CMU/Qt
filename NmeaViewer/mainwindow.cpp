#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QNetworkInterface>
#include <QLineEdit>
#include <QFileDialog>
#include <QColorDialog>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    connect(interface,SIGNAL(signalClientIdConfirmed(QString)),this,SLOT(slotClientIdConfirmed(QString)));

    marker.id=0;
    markerconfig.fillColor="red";
    markerconfig.fillOpacity=0.7;
    markerconfig.scale=5;
    markerconfig.strokeColor="black";
    markerconfig.strokeWeight=1;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_open_clicked()
{
    QString filename=QFileDialog::getOpenFileName(this,"Open NMEA File",QString(),QString("NMEA (*.nmea)"));
    if(!filename.isEmpty())
    {
        QFile file(filename);
        if(file.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            polyline.id=polylines.size();
            polyline.vertices.clear();

            nmeaPARSER parser;
            nmea_parser_init(&parser);
            nmeaINFO info;
            nmea_zero_INFO(&info);

            QLineSeries * series = new QLineSeries();
            while(!file.atEnd())
            {
                QByteArray sentence=file.readLine();
                sentence=sentence.trimmed();
                sentence.append(QByteArray("\r\n"));
                nmea_parse(&parser,sentence.data(),sentence.size(),&info);
                if(!(info.lat==0&&info.lon==0))
                {
                    if(sentence.startsWith("$GPGGA"))
                    {
                        series->append(polyline.vertices.size(),info.HDOP);
                        polyline.vertices.push_back(QGMapPointF(convertNDEGToDegree(info.lat),convertNDEGToDegree(info.lon)));
                    }
                }
            }
            nmea_parser_destroy(&parser);
            file.close();

            QChart *chart = new QChart();
            chart->legend()->hide();
            chart->addSeries(series);
            chart->createDefaultAxes();
            chart->setTitle("Simple line chart example");

            HdopChartView *chartView = new HdopChartView(chart);
            chartView->setRubberBand(QChartView::HorizontalRubberBand);
            chartView->setRenderHint(QPainter::Antialiasing);
            ui->tabWidget->addTab(chartView,QString::number(polylines.size()+1));
            connect(chartView,SIGNAL(signalPreviewValue(QPointF)),this,SLOT(slotPreviewValue(QPointF)));

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
    interface->setPolyline(polylines,polylineconfigs,clientId);
}

void MainWindow::slotPreviewValue(QPointF value)
{
    ui->value->setText(QString("%1,%2").arg(value.x()).arg(value.y()));
    int pointid=int(value.x()+0.5);
    int dataid=ui->tabWidget->currentIndex();
    if(pointid<0) pointid=0;
    if(pointid>=polylines[dataid].vertices.size()) pointid=polylines[dataid].vertices.size()-1;
    marker.position=polylines[dataid].vertices[pointid];
    interface->setMarker(marker,markerconfig,"");
}

void MainWindow::on_hide_clicked()
{
    int index=ui->nmeas->currentRow();
    if(index>=0)
    {
        interface->setPolylineVisible(index,1,false,"");
    }
}

void MainWindow::on_show_clicked()
{
    int index=ui->nmeas->currentRow();
    if(index>=0)
    {
        interface->setPolylineVisible(index,1,true,"");
    }
}


HdopChartView::HdopChartView(QWidget *parent)
    : QChartView(parent)
{

}

HdopChartView::HdopChartView(QChart *chart, QWidget *parent)
    : QChartView(chart,parent)
{

}

void HdopChartView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF value=chart()->mapToValue(event->pos());
    emit signalPreviewValue(value);
    QChartView::mouseMoveEvent(event);
}

void HdopChartView::mousePressEvent(QMouseEvent *event)
{
    QPointF value=chart()->mapToValue(event->pos());
    emit signalSelectedValue(value);
    QChartView::mousePressEvent(event);
}

