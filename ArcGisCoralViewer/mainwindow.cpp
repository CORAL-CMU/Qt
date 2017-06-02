#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_trajviewer = new TrajectoryViewer();
    ui->tabWidget->addTab(m_trajviewer, "Trajectory");
}

MainWindow::~MainWindow()
{
    delete ui;
}
