#include "mainwindow.h"
#include "ui_mainwindow.h"

#define GPGGASTR "$GPGGA,000637.10,4026.4183386,N,07956.1275614,W,5,06,1.0,308.021,M,-34.121,M,1.1,*7C"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    client = new QNtripClient();
    client->addr="209.255.196.164";
    client->port="2101";
    client->user="CMU";
    client->passwd="CMU";
    client->loadCasterList();

    connect(client,SIGNAL(signalRtkReceived(QByteArray)),this,SLOT(slotRtkReceived(QByteArray)));

    server = new QNtripServer("CMU", "CMU");
    connect(server,SIGNAL(signalNewConnection(QString)),this,SLOT(slotNewConnection(QString)));
    connect(server,SIGNAL(signalUpdateGPGGA(QString)),this,SLOT(slotUpdateGPGGA(QString)));
    connect(server,SIGNAL(signalClientDisconnect(QString)),this,SLOT(slotClientDisconnect(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_setcaster_clicked()
{
    QList<QString> casters;
    for(int i=0;i<client->casterlist.size();i++)
    {
        casters.push_back(client->casterlist[i].identifier);
    }
    QString caster = QInputDialog::getItem(this,"Select Caster","Caster Identifier:", casters);
    casterid = casters.indexOf(caster);
    client->mntpnt = client->casterlist[casterid].mountpoint;
    client->encodeNtripPath();
    ui->casterinfo->setText(client->ntrippath);
    server->caster = client->casterlist[casterid];
}

void MainWindow::on_triggerrtk_clicked()
{
    if(ui->triggerrtk->text()==QString("Start RTK"))
    {
        client->startReceiveRtk(casterid, GPGGASTR);
        ui->setcaster->setEnabled(false);
        ui->triggerrtk->setText("Stop RTK");
    }
    else if(ui->triggerrtk->text()==QString("Stop RTK"))
    {
        client->stopReceiveRtk();
        ui->setcaster->setEnabled(true);
        ui->triggerrtk->setText("Start RTK");
    }
}

void MainWindow::slotRtkReceived(QByteArray rtk)
{
    if(ui->showrtk->isChecked())
    {
        QString content=QString("[%1]\n%2")
                .arg(QTime::currentTime().toString("HH:mm:ss:zzz"))
                .arg(QString(rtk));
        ui->rtkview->append(content);
    }
    server->sendRtk(rtk);
}

void MainWindow::slotNewConnection(QString peer)
{
    QString content=QString("[%1]\n%2 Connected")
            .arg(QTime::currentTime().toString("HH:mm:ss:zzz"))
            .arg(peer);
    ui->serverlog->append(content);
}

void MainWindow::slotUpdateGPGGA(QString GPGGA)
{
    client->stopReceiveRtk();
    this->thread()->sleep(2);
    client->startReceiveRtk(casterid, GPGGA);

    QString content=QString("[%1]\n%2")
            .arg(QTime::currentTime().toString("HH:mm:ss:zzz"))
            .arg(GPGGA);
    ui->serverlog->append(content);
}

void MainWindow::slotClientDisconnect(QString peer)
{
    QString content=QString("[%1]\n%2 Disconnected")
            .arg(QTime::currentTime().toString("HH:mm:ss:zzz"))
            .arg(peer);
    ui->serverlog->append(content);
}

void MainWindow::on_triggerserver_clicked()
{
    if(ui->triggerserver->text()==QString("Open Server"))
    {
        server->openServer(QHostAddress::Any,12346);
        ui->triggerserver->setText("Close Server");
    }
    else if(ui->triggerserver->text()==QString("Close Server"))
    {
        server->closeServer();
        ui->triggerserver->setText("Open Server");
    }
}
