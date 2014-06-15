#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "configloader.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _configLoader = new ConfigLoader("assets/config.xml", this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
