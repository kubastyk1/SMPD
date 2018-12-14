#include <vector>
#include <algorithm>
#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include "database.h"
#include "fisher.h"

using namespace std;

typedef struct {
    map<string, float> classAverages;
    map<string, float> classStds;
} ASdata;

typedef struct {
    vector<Object> objects;
    uint amount;
} classObjects;

classObjects* getObjectsOfClass(uint objClass, Database db) {
    classObjects* co = new classObjects();
    co->amount = 0;
    for(Object o : db.getObjects()) {
        if(o.getClassName() == db.getClassNames()[objClass]) {
            co->objects.push_back(o);
            co->amount++;
        }
    }
    return co;
}

vector<vector<uint>> CreateCombinations(uint range, uint size) {
       vector<vector<uint>> combinations;
       vector<bool> v(range);
       fill(v.begin(), v.begin() + static_cast<int>(size), true);
       do {
           vector<uint> singleCombination;
           for (uint i = 0; i < range; ++i) {
               if (v[i]) {
                   singleCombination.push_back(i+1);
               }
           }
           combinations.push_back(singleCombination);
       } while (prev_permutation(v.begin(), v.end()));
       return combinations;
}

vector<ASdata*> calcAS(Database db) {
    vector<ASdata*> asd;
    for (unsigned int i = 0; i < db.getNoFeatures(); ++i)
    {
        ASdata* tmp = new ASdata();
        for (auto const &ob : db.getObjects())
        {
            tmp->classAverages[ob.getClassName()] += ob.getFeatures()[i];
            tmp->classStds[ob.getClassName()] += ob.getFeatures()[i] * ob.getFeatures()[i];
        }
        for_each(db.getClassCounters().begin(), db.getClassCounters().end(), [&](const pair<string, int> &it)
        {
            tmp->classAverages[it.first] /= it.second;
            tmp->classStds[it.first] = sqrt(tmp->classStds[it.first] / it.second - tmp->classAverages[it.first] * tmp->classAverages[it.first]);
        }
        );
        asd.push_back(tmp);
      }
    return asd;
}

fisherPair* computeFisher(Database db) {
    fisherPair* FP = new fisherPair;
    vector<ASdata*> asd = calcAS(db);
    float tmp;
    for (unsigned int i = 0; i < db.getNoFeatures(); ++i)
    {
        tmp = abs(asd.at(i)->classAverages[ db.getClassNames()[0] ] - asd.at(i)->classAverages[db.getClassNames()[1]]) / (asd.at(i)->classStds[db.getClassNames()[0]] + asd.at(i)->classStds[db.getClassNames()[1]]);
        if (tmp > FP->FLD)
        {
            FP->FLD = tmp;
            FP->max_ind = i;
        }
      }
    return FP;
}

void computeFisher(uint dimension, Database db) {
    vector<fisherPair> fpMulti;
    vector<vector<uint>> combinations = CreateCombinations(db.getNoFeatures(),dimension);
    vector<ASdata*> asd = calcAS(db);
    classObjects* classAObjects = getObjectsOfClass(0, db);
    classObjects* classBObjects = getObjectsOfClass(1, db);
    for (vector<uint> &combo : combinations) {
        boost::numeric::ublas::matrix<double> MA(dimension,classAObjects->amount), MB(dimension,classBObjects->amount);
        boost::numeric::ublas::matrix<double> SA_X(dimension,classAObjects->amount), SB_X(dimension,classBObjects->amount);
        for (uint i = 0; i < dimension; i++) { // POPULATING MA, MB, SA_X, SB_X
            for (uint j = 0; j < classAObjects->amount; j++) {
                MA(i,j) = static_cast<double>(asd.at(combo[i]-1)->classAverages[db.getClassNames()[0]]);
                SA_X(i,j) = static_cast<double>(classAObjects->objects.at(j).getFeatures()[combo[i]-1]);
            }
            for (uint j = 0; j < classBObjects->amount; j++) {
                MB(i,j) = static_cast<double>(asd.at(combo[i]-1)->classAverages[db.getClassNames()[1]]);
                SB_X(i,j) = static_cast<double>(classBObjects->objects.at(j).getFeatures()[combo[i]-1]);
            }
        }
        boost::numeric::ublas::matrix<double> SA_final(dimension, dimension);
        boost::numeric::ublas::matrix<double> SB_final(dimension, dimension);
        SA_final = prod(MA - SA_X, trans(MA - SA_X))/classAObjects->amount;
        SB_final = prod(MB - SB_X, trans(MB - SB_X))/classBObjects->amount;
//        for (uint i = 0; i < dimension; i++) {
//            for (uint j = 0; j < dimension; j++) {
//                cout << SA_final(i,j) << " ";
//            }
//            cout << endl;
//        }
//       cout << "------------" <<endl;
    }
}
