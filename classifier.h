#include <boost/numeric/ublas/matrix.hpp>

#include "database.h"

namespace bnu = boost::numeric::ublas;

class Classifier
{

    public:
        Classifier( const std::vector<Object> &trainSet, const std::vector<Object> &testSet ) : trainingSet(trainSet), testSet(testSet){}

        virtual void train() = 0;
        virtual float test() = 0;

        double classifyNN(const std::vector<Object> &trainSet, const std::vector<Object> &testSet);
        double classifyNM(const std::vector<Object> &trainSet, const std::vector<Object> &testSet, const std::vector<std::string> &classNames);
        double classifyKNN(const std::vector<Object> &trainSet, const std::vector<Object> &testSet, unsigned int k, const std::vector<std::string> &classNames);
        double classifyKNM(const std::vector<Object> &trainSet, const std::vector<Object> &testSet, unsigned int k, const std::vector<std::string> &classNames);

    private:
        std::vector<Object> trainingSet;
        std::vector<Object> testSet;

        bnu::matrix<double> convertToMatrix(Object object);
        bnu::matrix<double> convertToMatrix(std::vector<double> vector);


};
