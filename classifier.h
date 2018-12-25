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
        double classifyNM(const std::vector<Object> &trainSet, const std::vector<Object> &testSet);
        double classifyKNN(const std::vector<Object> &trainSet, const std::vector<Object> &testSet);
        double classifyKNM(const std::vector<Object> &trainSet, const std::vector<Object> &testSet);

    private:
        std::vector<Object> trainingSet;
        std::vector<Object> testSet;

        bnu::matrix<double> convertToMatrix(Object object);

};
