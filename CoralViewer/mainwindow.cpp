#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFile>
#include <QString>
#include <QList>

#include <QQmlContext>
#include <QVariant>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_database = Q_NULLPTR;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openapplanix_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Applanix Log", QString(), "CSV (*.csv)");
    if(!filename.isEmpty())
    {
        QFile file(filename);
        if(file.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            if(m_database != Q_NULLPTR)
            {
                delete m_database;
            }
            m_database = new QCoralDataBase(this);
            file.readLine();
            while(!file.atEnd())
            {
                QCoralData * data = m_database->createData();

                QString dataline = file.readLine();
                QList<QString> values = dataline.split(",", QString::SkipEmptyParts);

                qint64 msecs = qint64(values[1].toDouble()*1000);
                QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(msecs);
                data->timestamp()->setQDateTime(timestamp);

                data->position()->setWkid(6346);
                data->position()->setPosition(QVector3D(values[3].toFloat(), values[2].toFloat(), values[4].toFloat()));

                data->orientation()->setEulerAnglesRad(QVector3D(values[7].toFloat(), values[6].toFloat(), values[5].toFloat()));

                data->linearVelocity()->setVelocity(QVector3D(values[8].toFloat(), values[9].toFloat(), values[10].toFloat()));

                data->angularVelocity()->setEulerAnglesRad(QVector3D(values[13].toFloat(), values[12].toFloat(), values[11].toFloat()));

                data->acceleration()->setAcceleration(QVector3D(values[14].toFloat(), values[15].toFloat(), values[16].toFloat()));
            }
            file.close();
        }
    }
}

void MainWindow::on_showapplanix_clicked()
{
    m_view.setResizeMode(QQuickView::SizeRootObjectToView);
    QQmlContext * ctxt = m_view.rootContext();
    ctxt->setContextProperty("myModel", QVariant::fromValue(m_database->qObjectData()));

    m_view.setSource(QUrl("qrc:///CoralDataViewer.qml"));
    m_view.show();
}
