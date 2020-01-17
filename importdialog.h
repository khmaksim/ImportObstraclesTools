#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>

namespace Ui {
    class ImportDialog;
}

class ImportDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit ImportDialog(QWidget *parent = nullptr);
        ~ImportDialog();
        QString getFileName() const;
        QString getAirfield() const;
        QString getCodeICAO() const;

    private:
        Ui::ImportDialog *ui;

    private slots:
        void selectFile();
        void enableButton();
};

#endif // IMPORTDIALOG_H
