#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "obstraclesform.h"
#include "databaseaccess.h"
#include <QDebug>
#include <QSettings>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ObstraclesForm *form = new ObstraclesForm(this);
    ui->stackedWidget->addWidget(form);
    ui->stackedWidget->setCurrentIndex(1);

//    connect(obstraclesHandler, SIGNAL(finished(Airfield,QVector<QVector<QString> >&)), DatabaseAccess::getInstance(), SLOT(update(Airfield,QVector<QVector<QString> >&)));
    connect(ui->obstracleButton, SIGNAL(clicked(bool)), this, SLOT(showObstracles()));
    readSettings();
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;
}

void MainWindow::showObstracles()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::writeSettings()
{
    QSettings settings;

    settings.beginGroup("geometry");
    settings.setValue("maximized", this->isMaximized());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings;

    settings.beginGroup("geometry");
    if (settings.value("maximized").toBool())
        this->showMaximized();
    settings.endGroup();
    settings.beginGroup("database");
    settings.endGroup();
}
