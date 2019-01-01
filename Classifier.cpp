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
    }

    cout << "Number of proper predictions: " << numberOfProperPredictions << endl;
    return numberOfProperPredictions * (100.0 / testSet.size());
}

double Classifier::classifyNM(const std::vector<Object> &trainSet, const std::vector<Object> &testSet, const std::vector<std::string> &classNames) {

    std::map <std::string, vector<Object> > classObjectsMap;
    std::map <std::string, vector<double> > classMiddleValuesMap;
    int numberOfProperPredictions = 0;
    vector<double> tempMiddleValues;
    double sum = 0;

    for (Object o : trainSet) {
        vector<Object> tempObjects;
        if (classObjectsMap.find(o.getClassName()) != classObjectsMap.end()) {
            tempObjects = classObjectsMap[o.getClassName()];
        }
        tempObjects.push_back(o);
        classObjectsMap[o.getClassName()] = tempObjects;
    }

    for (std::string className : classNames) {
        vector<Object> classObjects = classObjectsMap.find(className) -> second;
        tempMiddleValues.clear();
        for (unsigned int i = 0; i < classObjects[0].getFeaturesNumber(); i++) {
            sum = 0;
            for (Object o : classObjects) {
                sum += static_cast<double>(o.getFeatures()[i]);
            }
            tempMiddleValues.push_back(sum/classObjects.size());
        }
        classMiddleValuesMap[className] = tempMiddleValues;
    }

    for (unsigned int i = 0; i < testSet.size(); i++) {
        bnu::matrix<double> testSetMatrix = convertToMatrix(testSet[i]);
        double lowestValue = 0;
        std::string lowestValueClass;

        for (std::string className : classNames) {
            vector<double> classMiddleValues = classMiddleValuesMap.find(className) -> second;
            bnu::matrix<double> trainSetMatrix = convertToMatrix(classMiddleValues);

            bnu::matrix<double> resultMatrix = trainSetMatrix - testSetMatrix;
            sum = 0;

            for (unsigned int j = 0; j < resultMatrix.size1(); j++) {
                sum += pow(resultMatrix(j,0), 2);
            }

            double result = sqrt(sum);

            if (lowestValue == 0) {
                lowestValue = result;
                lowestValueClass = className;
            } else if (result < lowestValue) {
                lowestValue = result;
                lowestValueClass = className;
            }
        }

        if (testSet[i].getClassName() == lowestValueClass) {
            numberOfProperPredictions += 1;
        }
    }

    return numberOfProperPredictions * (100.0 / testSet.size());
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

bnu::matrix<double> Classifier::convertToMatrix(vector<double> vector) {
    bnu::matrix<double> matrix(vector.size(), 1);
    for (unsigned int i = 0; i < vector.size(); i++) {
        matrix(i,0) = vector[i];
    }
    return matrix;
}
