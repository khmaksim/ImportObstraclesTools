#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QDebug>
#include <QSslSocket>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Aviacominfo");
    QCoreApplication::setOrganizationDomain("aviacominfo.com");
    QCoreApplication::setApplicationName("ImportObstraclesTools");
    QApplication::setStyle(QStyleFactory::create("windowsvista"));
//    qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();

    QTranslator translator;
    if (translator.load(QLocale(), QLatin1String("aviacominfotools"), QLatin1String("_"), QLatin1String(":/translations/res/translations")))
        a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
