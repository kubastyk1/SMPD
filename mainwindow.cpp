#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include "fisher.h"

#include <QImage>
#include <QDebug>


std::vector<Object> trainingSet, testSet;

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

    ui->CtextBrowser->setText("noClass: " +  QString::number(database.getNoClass()));
    ui->CtextBrowser->append("noObjects: "  +  QString::number(database.getNoObjects()));
    ui->CtextBrowser->append("noFeatures: "  +  QString::number(database.getNoFeatures()));
    ui->CplainTextEditTrainingPart->appendPlainText(QString::number(80));
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

void MainWindow::CupdateButtonState(void){
    std::vector<std::string> classifierMethods = {"NN", "NM", "k-NN", "k-NM" };

    for(uint i = 0; i < classifierMethods.size(); i++){
        ui -> CcomboBoxClassifiers -> addItem(QString::fromStdString(classifierMethods[i]));
    }

    //Select value for k-NN and k-NM - odd numbers, half of the number of all objects
    for(uint i = 1; i < database.getNoObjects()/2; i += 2){
        ui -> CcomboBoxK -> addItem(QString::number(i));
    }
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
            fisherPair* FP = singleFisher(database);
            ui->FStextBrowserDatabaseInfo->append("[S]max_ind: "  +  QString::number(FP->max_ind[0]) + " fisher: " + QString::number(static_cast<double>(FP->FLD)));
        }
        else if (dimension > 1 && database.getNoClass() == 2)
        {
           fisherPair* FP = bruteForce(dimension,database);
           string inds = "(";
           for (uint ind : FP->max_ind) {
               inds += to_string(ind) + " ";
           }
           inds = inds.substr(0, inds.size()-1) + ")";
           cout<<inds;
           ui->FStextBrowserDatabaseInfo->append("[BrF]max_ind: "  +  QString::fromStdString(inds) + " fisher: " + QString::number(static_cast<double>(FP->FLD)));
        }
    }
    else if( ui -> FSradioButtonSFS -> isChecked()){
        fisherPair* FP = SFS(dimension,database);
        string inds = "(";
        for (uint ind : FP->max_ind) {
            inds += to_string(ind) + " ";
        }
        inds = inds.substr(0, inds.size()-1) + ")";
        cout<<inds;
        ui->FStextBrowserDatabaseInfo->append("[SFS]max_ind: "  +  QString::fromStdString(inds) + " fisher: " + QString::number(static_cast<double>(FP->FLD)));
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
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open TextFile"), "", tr("Texts Files (*.txt)"));

    if ( !database.load(fileName.toStdString()) )
        QMessageBox::warning(this, "Warning", "File corrupted !!!");
    else
        QMessageBox::information(this, fileName, "File loaded !!!");

    FSupdateButtonState();
    CupdateButtonState();
    updateDatabaseInfo();
}

void MainWindow::on_CpushButtonSaveFile_clicked()
{

}

void MainWindow::on_CpushButtonTrain_clicked()
{
    std::vector<Object> allObjects = database.getObjects();

    int percentNumber = ui->CplainTextEditTrainingPart->toPlainText().toInt();
    int expectedSizeOfTrainingSet = static_cast<int>(allObjects.size() * (percentNumber / 100.0));

    for (int i = 0; i < expectedSizeOfTrainingSet; i++) {
        uint random = rand() % allObjects.size();
        trainingSet.push_back(allObjects.at(random));
        allObjects.erase(allObjects.begin() + random);
    }

    testSet = allObjects;

    cout << "TrainingSet size: " << trainingSet.size() << endl;
    cout << "TestSet size: " << testSet.size() << endl;
}

void MainWindow::on_CpushButtonExecute_clicked()
{

}
