#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include "fisher.h"

#include <QImage>
#include <QDebug>




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    FSupdateButtonState();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateDatabaseInfo()
{
    ui->FScomboBox->clear();
    for(unsigned int i=1; i<=database.getNoFeatures(); ++i)
        ui->FScomboBox->addItem(QString::number(i));

    ui->FStextBrowserDatabaseInfo->setText("noClass: " +  QString::number(database.getNoClass()));
    ui->FStextBrowserDatabaseInfo->append("noObjects: "  +  QString::number(database.getNoObjects()));
    ui->FStextBrowserDatabaseInfo->append("noFeatures: "  +  QString::number(database.getNoFeatures()));

}

void MainWindow::FSupdateButtonState(void)
{
    if(database.getNoObjects()==0)
    {
        FSsetButtonState(false);
    }
    else
        FSsetButtonState(true);

}


void MainWindow::FSsetButtonState(bool state)
{
   ui->FScomboBox->setEnabled(state);
   ui->FSpushButtonCompute->setEnabled(state);
   ui->FSpushButtonSaveFile->setEnabled(state);
   ui->FSradioButtonFisher->setEnabled(state);
   ui->FSradioButtonSFS->setEnabled(state);
}

void MainWindow::on_FSpushButtonOpenFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open TextFile"), "", tr("Texts Files (*.txt)"));

    if ( !database.load(fileName.toStdString()) )
        QMessageBox::warning(this, "Warning", "File corrupted !!!");
    else
        QMessageBox::information(this, fileName, "File loaded !!!");

    FSupdateButtonState();
    updateDatabaseInfo();
}

void MainWindow::on_FSpushButtonCompute_clicked()
{
    uint dimension = static_cast<uint>(ui->FScomboBox->currentText().toInt());
    if( ui->FSradioButtonFisher ->isChecked())
    {
        if (dimension == 1 && database.getNoClass() == 2)
        {
            fisherPair* FP = computeFisher(database);
            ui->FStextBrowserDatabaseInfo->append("max_ind: "  +  QString::number(FP->max_ind[0]) + " " + QString::number(static_cast<double>(FP->FLD)));
        }
        else if (dimension > 1 && database.getNoClass() == 2)
        {
           fisherPair* FP = computeFisher(dimension,database);
           string inds = "(";
           for (uint ind : FP->max_ind) {
               inds += to_string(ind) + " ";
           }
           inds = inds.substr(0, inds.size()-1) + ")";
           cout<<inds;
           ui->FStextBrowserDatabaseInfo->append("max_ind: "  +  QString::fromStdString(inds) + " " + QString::number(static_cast<double>(FP->FLD)));
        }
    }
}



void MainWindow::on_FSpushButtonSaveFile_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
    tr("Open TextFile"), "D:\\Users\\Krzysiu\\Documents\\Visual Studio 2015\\Projects\\SMPD\\SMPD\\Debug\\", tr("Texts Files (*.txt)"));

        QMessageBox::information(this, "My File", fileName);
        database.save(fileName.toStdString());
}

void MainWindow::on_PpushButtonSelectFolder_clicked()
{
}

void MainWindow::on_CpushButtonOpenFile_clicked()
{

}

void MainWindow::on_CpushButtonSaveFile_clicked()
{

}

void MainWindow::on_CpushButtonTrain_clicked()
{

}

void MainWindow::on_CpushButtonExecute_clicked()
{

}
