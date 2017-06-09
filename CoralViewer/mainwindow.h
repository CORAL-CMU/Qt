#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QQuickView>

#include <qcoraldatabase.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_openapplanix_clicked();

    void on_showapplanix_clicked();

private:
    Ui::MainWindow *ui;

private:
    QQuickView m_view;
    QCoralDataBase * m_database;
};

#endif // MAINWINDOW_H
