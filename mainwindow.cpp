#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include "fisher.h"
#include "Classifier.cpp"


#include <QImage>
#include <QDebug>

std::vector<uint> featureSet;
std::vector<Object> trainingSet, testSet;
std::vector<std::vector<Object>> crossValidationSet;

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
    ui->CcomboBoxClassifiers->clear();
    ui -> CcomboBoxK->clear();
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
    CupdateButtonState();
    updateDatabaseInfo();
}

void MainWindow::on_FSpushButtonCompute_clicked()
{
    fisherPair* FP;
    uint dimension = static_cast<uint>(ui->FScomboBox->currentText().toInt());
    if( ui->FSradioButtonFisher ->isChecked())
    {
        if (dimension == 1 && database.getNoClass() == 2)
        {
            FP = singleFisher(database);
            ui->FStextBrowserDatabaseInfo->append("[S]max_ind: "  +  QString::number(FP->max_ind[0]) + " fisher: " + QString::number(static_cast<double>(FP->FLD)));
        }
        else if (dimension > 1 && database.getNoClass() == 2)
        {
           FP = bruteForce(dimension,database);
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
        FP = SFS(dimension,database);
        string inds = "(";
        for (uint ind : FP->max_ind) {
            inds += to_string(ind) + " ";
        }
        inds = inds.substr(0, inds.size()-1) + ")";
        cout << inds <<endl;
        ui->FStextBrowserDatabaseInfo->append("[SFS]max_ind: "  +  QString::fromStdString(inds) + " fisher: " + QString::number(static_cast<double>(FP->FLD)));
    }
    featureSet = FP->max_ind;
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
    std::vector<Object> objectsWithSelectedFeatures;

    if (featureSet.size() != 0) {
        for (Object o : allObjects){
           vector<float> tmp;
           for (uint featureS : featureSet){
               for(uint i = 0; i < o.getFeaturesNumber(); i++){
                  if (i == featureS-1) {
                    tmp.push_back(o.getFeatures()[i]);
                   }
               }
           }
            o.replaceFeatures(tmp);
            objectsWithSelectedFeatures.push_back(o);
        }
    } else {
        for (Object o : allObjects){
            objectsWithSelectedFeatures.push_back(o);
        }
    }

    cout << "Number of features: " << objectsWithSelectedFeatures[0].getFeaturesNumber() << endl;

    uint crossValidationValue = ui -> CplainTextEditCrossValidation -> toPlainText().toUInt();
    crossValidationSet.clear();
    if (crossValidationValue == 0) {
        trainingSet.clear();
        testSet.clear();

        int percentNumber = ui->CplainTextEditTrainingPart->toPlainText().toInt();
        int expectedSizeOfTrainingSet = static_cast<int>(objectsWithSelectedFeatures.size() * (percentNumber / 100.0));

        for (int i = 0; i < expectedSizeOfTrainingSet; i++) {
            uint random = rand() % objectsWithSelectedFeatures.size();
            trainingSet.push_back(objectsWithSelectedFeatures.at(random));
            objectsWithSelectedFeatures.erase(objectsWithSelectedFeatures.begin() + random);
        }

        testSet = objectsWithSelectedFeatures;

        ui->CtextBrowser->append("Training set size: " +  QString::number(trainingSet.size()));
        ui->CtextBrowser->append("Test set size: " +  QString::number(testSet.size()));
    } else {
        std::vector<Object> tempSet;
        uint expectedSizeOfSet = objectsWithSelectedFeatures.size()/crossValidationValue;

        for (uint i = 0; i < crossValidationValue; i++){
            for (uint j = 0; j < expectedSizeOfSet; j++) {
                uint random = rand() % objectsWithSelectedFeatures.size();
                tempSet.push_back(objectsWithSelectedFeatures.at(random));
                objectsWithSelectedFeatures.erase(objectsWithSelectedFeatures.begin() + random);
            }
            crossValidationSet.push_back(tempSet);
            tempSet.clear();
        }

        ui->CtextBrowser->append("Cross validation sections number: " +  QString::number(crossValidationSet.size()));
        ui->CtextBrowser->append("Cross validation section size: " +  QString::number(crossValidationSet.at(0).size()));
    }
}

void MainWindow::on_CpushButtonExecute_clicked()
{
    Classifier *c;
    double percentOfClassified = 0;
    uint k = static_cast<uint>(ui -> CcomboBoxK -> currentText().toInt());
    std::string methodName = ui -> CcomboBoxClassifiers -> currentText().toStdString();
    uint crossValidationValue = ui -> CplainTextEditCrossValidation -> toPlainText().toUInt();

    if (crossValidationValue == 0) {
        if(methodName == "NN"){
            percentOfClassified = c->classifyNN(trainingSet, testSet);
        } else if (methodName == "NM") {
            percentOfClassified = c->classifyNM(trainingSet, testSet, database.getClassNames());
        } else if (methodName == "k-NN") {
            percentOfClassified = c->classifyKNN(trainingSet, testSet, k, database.getClassNames());
        } else if (methodName == "k-NM") {
            percentOfClassified = c->classifyKNM(trainingSet, testSet, k, database.getClassNames());
        }

        ui->CtextBrowser->append("Percent of properly classified objects: " +  QString::number(percentOfClassified) + "%");
    } else {
        double averagePercentage = 0;

        for (uint i = 0; i < crossValidationValue; i++) {
            testSet.clear();
            testSet = crossValidationSet.at(i);
            trainingSet.clear();

            for (uint j = 0; j < crossValidationSet.size(); j++) {
                if (i != j) {
                    trainingSet.insert(trainingSet.end(), crossValidationSet.at(i).begin(), crossValidationSet.at(i).end());
                }
            }

            if(methodName == "NN"){
                percentOfClassified = c->classifyNN(trainingSet, testSet);
            } else if (methodName == "NM") {
                percentOfClassified = c->classifyNM(trainingSet, testSet, database.getClassNames());
            } else if (methodName == "k-NN") {
                percentOfClassified = c->classifyKNN(trainingSet, testSet, k, database.getClassNames());
            } else if (methodName == "k-NM") {
                percentOfClassified = c->classifyKNM(trainingSet, testSet, k, database.getClassNames());
            }

            averagePercentage += percentOfClassified;
            cout << "Percent of properly classified objects: " << percentOfClassified << endl;
        }

        ui->CtextBrowser->append("Average percentage of properly classified objects: " +  QString::number(averagePercentage / crossValidationSet.size()) + "%");
    }
}
