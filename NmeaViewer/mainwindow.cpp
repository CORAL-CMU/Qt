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

    mapzoom=15;
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

            QLineSeries * hdopseries = new QLineSeries();
            QLineSeries * satnumseries = new QLineSeries();
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
                        hdopseries->append(polyline.vertices.size(),info.HDOP);
                        polyline.vertices.push_back(QGMapPointF(convertNDEGToDegree(info.lat),convertNDEGToDegree(info.lon)));
                    }
                    else if(sentence.startsWith("$GPGSA"))
                    {
                        hdopseries->append(polyline.vertices.size(),info.HDOP);
                    }
                    else if(sentence.startsWith("$GLGSA"))
                    {
                        hdopseries->append(polyline.vertices.size(),info.HDOP);
                    }
                    else if(sentence.startsWith("$GPGSV"))
                    {
                        satnumseries->append(polyline.vertices.size(),info.satinfo.inuse);
                    }
                    else if(sentence.startsWith("$GLGSV"))
                    {
                        satnumseries->append(polyline.vertices.size(),info.satinfo.inuse);
                    }
                }
            }
            nmea_parser_destroy(&parser);
            file.close();

            QChart * hdopchart = new QChart();
            hdopchart->legend()->hide();
            hdopchart->addSeries(hdopseries);
            hdopchart->createDefaultAxes();
            hdopchart->setTitle("HDOP");

            HdopChartView *hdopchartView = new HdopChartView(hdopchart);
            hdopchartView->setInteractive(true);
            hdopchartView->setRenderHint(QPainter::Antialiasing);
            ui->hdop->addTab(hdopchartView,QString::number(polylines.size()+1));
            connect(hdopchartView,SIGNAL(signalPreviewValue(QPointF,QPointF)),this,SLOT(slotPreviewHdopValue(QPointF,QPointF)));
            connect(hdopchartView,SIGNAL(signalMouseWheel(bool)),this,SLOT(slotZoom(bool)));

            hdopxlines.push_back(hdopchartView->scene()->addLine(QLine()));
            hdopylines.push_back(hdopchartView->scene()->addLine(QLine()));

            QChart * satnumchart = new QChart();
            satnumchart->legend()->hide();
            satnumchart->addSeries(satnumseries);
            satnumchart->createDefaultAxes();
            satnumchart->setTitle("SatNum");

            HdopChartView *satnumchartView = new HdopChartView(satnumchart);
            satnumchartView->setInteractive(true);
            satnumchartView->setRenderHint(QPainter::Antialiasing);
            ui->satnum->addTab(satnumchartView,QString::number(polylines.size()+1));
            connect(satnumchartView,SIGNAL(signalPreviewValue(QPointF,QPointF)),this,SLOT(slotPreviewSatnumValue(QPointF,QPointF)));
            connect(satnumchartView,SIGNAL(signalMouseWheel(bool)),this,SLOT(slotZoom(bool)));

            satnumxlines.push_back(satnumchartView->scene()->addLine(QLine()));
            satnumylines.push_back(satnumchartView->scene()->addLine(QLine()));

            QColor color=QColorDialog::getColor(Qt::blue,this,"Set Path Color");
            polylineconfig.strokeColor=color.name();

            ui->nmeas->addItem(filename);
            polylines.push_back(polyline);
            polylineconfigs.push_back(polylineconfig);

            QJsonObject object=polyline.toJsonObject();
            QGMapPolyline tmppolyline;
            tmppolyline.fromJsonObject(object);

            interface->setPolyline(polyline,polylineconfig,"");
            showflags.push_back(false);
            dataids.push_back(1);
            interface->setPolylineVisible(polyline.id,1,false,"");
        }
    }
}

void MainWindow::slotClientIdConfirmed(QString clientId)
{
    interface->setPolyline(polylines,polylineconfigs,clientId);
    for(int i=0;i<showflags.size();i++)
    {
        interface->setPolylineVisible(polylines[i].id,1,showflags[i],clientId);
    }
    interface->setMapZoom(mapzoom,clientId);
}

void MainWindow::slotPreviewHdopValue(QPointF value, QPointF position)
{
    ui->value->setText(QString("%1, %2, %3, %4").arg(value.x()).arg(value.y()).arg(position.x()).arg(position.y()));
    int pointid=int(value.x()+0.5);
    int dataid=ui->hdop->currentIndex();
    if(pointid<0) pointid=0;
    if(pointid>=polylines[dataid].vertices.size()) pointid=polylines[dataid].vertices.size()-1;

    QChartView * view=dynamic_cast<QChartView *>(ui->hdop->currentWidget());
    QPointF py1=QPointF(0,position.y());
    QPointF py2=QPointF(view->width(),position.y());
    hdopylines[dataid]->setLine(QLineF(py1,py2));

//    ui->timeline->setValue(pointid);
}

void MainWindow::slotPreviewSatnumValue(QPointF value, QPointF position)
{
    ui->value->setText(QString("%1, %2, %3, %4").arg(value.x()).arg(value.y()).arg(position.x()).arg(position.y()));
    int pointid=int(value.x()+0.5);
    int dataid=ui->satnum->currentIndex();
    if(pointid<0) pointid=0;
    if(pointid>=polylines[dataid].vertices.size()) pointid=polylines[dataid].vertices.size()-1;

    QChartView * view=dynamic_cast<QChartView *>(ui->satnum->currentWidget());
    QPointF py1=QPointF(0,position.y());
    QPointF py2=QPointF(view->width(),position.y());
    satnumylines[dataid]->setLine(QLineF(py1,py2));

//    ui->timeline->setValue(pointid);
}

void MainWindow::slotZoom(bool directionflag)
{
    if(directionflag)
    {
        mapzoom=mapzoom<20?mapzoom+1:mapzoom;
    }
    else
    {
        mapzoom=mapzoom>1?mapzoom-1:mapzoom;
    }
    interface->setMapZoom(mapzoom,"");
}

void MainWindow::on_hide_clicked()
{
    int index=ui->nmeas->currentRow();
    if(index>=0)
    {
        interface->setPolylineVisible(polylines[index].id,1,false,"");
        showflags[index]=false;
    }
}

void MainWindow::on_show_clicked()
{
    int index=ui->nmeas->currentRow();
    if(index>=0)
    {
        interface->setPolylineVisible(polylines[index].id,1,true,"");
        showflags[index]=true;
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
    emit signalPreviewValue(value,event->pos());
    QChartView::mouseMoveEvent(event);
}

void HdopChartView::mousePressEvent(QMouseEvent *event)
{
    QPointF value=chart()->mapToValue(event->pos());
    emit signalSelectedValue(value,event->pos());
    QChartView::mousePressEvent(event);
}

void HdopChartView::wheelEvent(QWheelEvent *event)
{
    emit signalMouseWheel(event->angleDelta().y()>0);
}


void MainWindow::on_nmeas_currentRowChanged(int currentRow)
{
    ui->hdop->setCurrentIndex(currentRow);
    ui->satnum->setCurrentIndex(currentRow);
    for(int i=0;i<showflags.size();i++)
    {
        if(i!=currentRow)
        {
            interface->setPolylineVisible(polylines[i].id,1,showflags[i],"");
        }
        else
        {
            interface->setPolylineVisible(polylines[i].id,1,true,"");
        }
    }
    int dataidback=dataids[currentRow];
    ui->timeline->setValue(1);
    ui->timeline->setRange(1,polylines[currentRow].vertices.size());
    ui->dataid->setRange(1,polylines[currentRow].vertices.size());
    dataids[currentRow]=dataidback;
    ui->timeline->setValue(dataids[currentRow]);
}

void MainWindow::on_dataid_valueChanged(int position)
{
    if(ui->timeline->value()!=position)
    {
        ui->timeline->setValue(position);
    }
}

void MainWindow::on_timeline_valueChanged(int position)
{
    int dataid=ui->satnum->currentIndex();
    dataids[dataid]=position;
    marker.position=polylines[dataid].vertices[position];
    interface->setMarker(marker,markerconfig,"");
    interface->setMapCenter(marker.position,false,"");

    QChartView * view=dynamic_cast<QChartView *>(ui->hdop->currentWidget());
    QPointF px=view->chart()->mapToPosition(QPointF(position,0));
    QPointF px1=QPointF(px.x(),0);
    QPointF px2=QPointF(px.x(),view->height());
    satnumxlines[dataid]->setLine(QLineF(px1,px2));
    hdopxlines[dataid]->setLine(QLineF(px1,px2));

    if(ui->dataid->value()!=position)
    {
        ui->dataid->setValue(position);
    }
}
