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
    for (vector<uint> &combo : combinations) {
        boost::numeric::ublas::matrix<double> MA(dimension,4), MB(dimension,4);
        for (uint i = 0; i < dimension; i++) {
            for (uint j = 0; j < 4; j++) {
                MA(i,j) = static_cast<double>(asd.at(combo[i]-1)->classAverages[db.getClassNames()[0]]);
                MB(i,j) = static_cast<double>(asd.at(combo[i]-1)->classAverages[db.getClassNames()[1]]);
                //cout << MA(i,j) << " ";
            }
            //cout << endl;
        }
        //cout << "------------" <<endl;
    }
}
