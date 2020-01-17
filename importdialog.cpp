#include "importdialog.h"
#include "ui_importdialog.h"
#include <QDebug>
#include <QFileDialog>

ImportDialog::ImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);

    connect(ui->selectFileButton, SIGNAL(clicked()), this, SLOT(selectFile()));
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->importButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->fileLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(enableButton()));
}

ImportDialog::~ImportDialog()
{
    delete ui;
}

void ImportDialog::selectFile()
{
    QString nameFile = QFileDialog::getOpenFileName(this, tr("Import from Excel file"), QDir::homePath(), tr("Excel files (*.xlsx)"));
    if (nameFile.isEmpty()) {
        qDebug() << "Empty name open file";
        return;
    }
    ui->fileLineEdit->setText(nameFile);
}

void ImportDialog::enableButton()
{
    ui->importButton->setEnabled(!ui->fileLineEdit->text().isEmpty() && !ui->nameAirfieldLineEdit->text().isEmpty());
}

QString ImportDialog::getFileName() const
{
    return ui->fileLineEdit->text().simplified();
}

QString ImportDialog::getAirfield() const
{
    return ui->nameAirfieldLineEdit->text().simplified();
}

QString ImportDialog::getCodeICAO() const
{
    return ui->codeIcaoLineEdit->text().simplified();
}
