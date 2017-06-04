#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_trajviewer = new TrajectoryViewer();
    ui->tabWidget->addTab(m_trajviewer, "Trajectory");

    m_applanixviewer = new QQuickWidget();
    m_applanixviewer->setSource(QUrl("qrc:/UI/ApplanixViewer.qml"));
    m_applanixviewer->setResizeMode(QQuickWidget::SizeRootObjectToView);
    ui->tabWidget->addTab(m_applanixviewer, "Applanix");
}

MainWindow::~MainWindow()
{
    delete ui;
}
