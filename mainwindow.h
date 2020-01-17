#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class Database;
class ObstraclesHandler;
class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    public slots:
        void showObstracles();

    private:
        Ui::MainWindow *ui;

        ObstraclesHandler *obstraclesHandler;
        Database *db;

        void readSettings();
        void writeSettings();
};

#endif // MAINWINDOW_H
