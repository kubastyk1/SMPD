#include <vector>
#include <algorithm>
#include <math.h>
#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

#include "Classifier.h"

using namespace std;
namespace bnu = boost::numeric::ublas;


double Classifier::classifyNN( const std::vector<Object> &trainSet, const std::vector<Object> &testSet) {

    double result[testSet.size()][trainSet.size()];
    int numberOfProperPredictions = 0;

    for (unsigned int testIndex = 0; testIndex < testSet.size(); testIndex++) {
        bnu::matrix<double> testSetMatrix = convertToMatrix(testSet[testIndex]);
        for (unsigned int trainIndex = 0; trainIndex < trainSet.size(); trainIndex++) {
            bnu::matrix<double> trainSetMatrix = convertToMatrix(trainSet[trainIndex]);
            bnu::matrix<double> substractedMatrix = trainSetMatrix - testSetMatrix;

            double sumOfMatrixElements = 0;
            for (unsigned int i = 0; i < substractedMatrix.size1(); i++) {
                sumOfMatrixElements += pow(substractedMatrix(i, 0), 2);
            }

            result[testIndex][trainIndex] = sqrt(sumOfMatrixElements);
        }
    }

    double lowestTrainSetValue;
    unsigned int indexOfLowestTrainSetValue = 0;

    for (unsigned int i = 0; i < testSet.size(); i++) {
        lowestTrainSetValue = result[i][0];
        for (unsigned int j = 0; j < trainSet.size(); j++) {
            if (result[i][j] < lowestTrainSetValue) {
                lowestTrainSetValue = result[i][j];
                indexOfLowestTrainSetValue = j;
            }
        }

        if (testSet[i].getClassName() == trainSet[indexOfLowestTrainSetValue].getClassName()) {
            numberOfProperPredictions += 1;
        }

        cout << "Lowest value: " << lowestTrainSetValue << endl;
        cout << "Test set classname: " << testSet[i].getClassName() << endl;
        cout << "Train set classname: " << trainSet[indexOfLowestTrainSetValue].getClassName() << endl;
    }

    cout << "Number of proper predictions: " << numberOfProperPredictions << endl;
    return numberOfProperPredictions * (100.0 / testSet.size());
}

double Classifier::classifyNM(const std::vector<Object> &trainSet, const std::vector<Object> &testSet) {

    return 0;
}

double Classifier::classifyKNN(const std::vector<Object> &trainSet, const std::vector<Object> &testSet, unsigned int kValue, const std::vector<std::string> &classNames) {

    double result[testSet.size()][trainSet.size()];
    int numberOfProperPredictions = 0;

    for (unsigned int testIndex = 0; testIndex < testSet.size(); testIndex++) {
        bnu::matrix<double> testSetMatrix = convertToMatrix(testSet[testIndex]);
        for (unsigned int trainIndex = 0; trainIndex < trainSet.size(); trainIndex++) {
            bnu::matrix<double> trainSetMatrix = convertToMatrix(trainSet[trainIndex]);
            bnu::matrix<double> substractedMatrix = trainSetMatrix - testSetMatrix;

            double sumOfMatrixElements = 0;
            for (unsigned int i = 0; i < substractedMatrix.size1(); i++) {
                sumOfMatrixElements += pow(substractedMatrix(i, 0), 2);
            }

            result[testIndex][trainIndex] = sqrt(sumOfMatrixElements);
        }
    }

    double bestResults[kValue];
    unsigned int indexesOfBestResults[kValue];

    for (unsigned int i = 0; i < testSet.size(); i++) {

        for(unsigned int j = 0; j < kValue; j++) {
            bestResults[j] = result[i][j];
            indexesOfBestResults[j] = j;
        }

        for (unsigned int j = kValue; j < trainSet.size(); j++) {
            double currentObject = result[i][j];
            double largestValueInArray = bestResults[0];
            unsigned int indexOfLargestValueInArray = 0;

            for(unsigned int q = 0; q < kValue; q++) {
                if (bestResults[q] > largestValueInArray) {
                    largestValueInArray = bestResults[q];
                    indexOfLargestValueInArray = q;
                }
            }

            if (largestValueInArray > currentObject) {
                bestResults[indexOfLargestValueInArray] = currentObject;
                indexesOfBestResults[indexOfLargestValueInArray] = j;
            }
        }

        std::string resultString[kValue];
        for (unsigned int j = 0; j < kValue; j++) {
            resultString[j] = trainSet[indexesOfBestResults[j]].getClassName();
        }

        int largestNumberOfElements = 0;
        std::string classNameOfClassWithLargestNumberOfElements;

        for (std::string className : classNames) {
            int numberOfElements = std::count (resultString, resultString + resultString->size(), className);
            if (numberOfElements > largestNumberOfElements) {
                classNameOfClassWithLargestNumberOfElements = className;
                largestNumberOfElements = numberOfElements;
            }
        }

        if (testSet[i].getClassName() == classNameOfClassWithLargestNumberOfElements) {
            numberOfProperPredictions += 1;
        }
    }

    cout << "Number of proper predictions: " << numberOfProperPredictions << endl;
    return numberOfProperPredictions * (100.0 / testSet.size());
}

double Classifier::classifyKNM(const std::vector<Object> &trainSet, const std::vector<Object> &testSet, unsigned int k, const std::vector<std::string> &classNames) {

    return 0;
}

bnu::matrix<double> Classifier::convertToMatrix(Object object) {
    bnu::matrix<double> matrix(object.getFeaturesNumber(), 1);
    for (unsigned int i = 0; i < object.getFeaturesNumber(); i++) {
        matrix(i,0) = object.getFeatures()[i];
    }
    return matrix;
}
