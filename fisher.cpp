#include <vector>
#include <algorithm>
#include <math.h>
#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>

#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include "database.h"
#include "fisher.h"

using namespace std;
namespace bnu = boost::numeric::ublas;

typedef struct {
    map<string, float> classAverages;
    map<string, float> classStds;
} ASdata;

typedef struct {
    vector<Object> objects;
    uint amount;
} classObjects;

double det(bnu::matrix<double> m)
{
  bnu::permutation_matrix<std::size_t> pivots(m.size1() );
  const int is_singular = bnu::lu_factorize(m, pivots);
  if (is_singular) return 0.0;
  double d = 1.0;
  const std::size_t sz = pivots.size();
  for (std::size_t i=0; i != sz; ++i)
  {
    if (pivots(i) != i)
    {
      d *= -1.0;
    }
    d *= m(i,i);
  }
  return d;
}

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

fisherPair* singleFisher(Database db) {
    fisherPair* FP = new fisherPair();
    vector<ASdata*> asd = calcAS(db);
    double tmp;
    uint indTmp;
    for (unsigned int i = 0; i < db.getNoFeatures(); ++i)
    {
        tmp = abs(asd.at(i)->classAverages[ db.getClassNames()[0] ] - asd.at(i)->classAverages[db.getClassNames()[1]]) / (asd.at(i)->classStds[db.getClassNames()[0]] + asd.at(i)->classStds[db.getClassNames()[1]]);
        if (tmp > FP->FLD)
        {
            FP->FLD = tmp;
            indTmp = i;
        }
      }
    FP->max_ind.push_back(indTmp);
    return FP;
}

fisherPair* computeFisher(uint dimension, Database db, vector<vector<uint>> combinations) {
    fisherPair* FP = new fisherPair();
    vector<ASdata*> asd = calcAS(db);
    classObjects* classAObjects = getObjectsOfClass(0, db);
    classObjects* classBObjects = getObjectsOfClass(1, db);
//    for (vector<uint> &combo : combinations) {
//        cout << "-----------" << endl;
//        cout << "---BEGIN---" << endl;
//        for (uint &digit : combo) {
//            cout << digit << " ";
//        }
//        cout << "----END----" << endl ;
//        cout << "-----------" << endl;
//    }
    for (vector<uint> &combo : combinations) {
        bnu::matrix<double> MA(dimension,classAObjects->amount), MB(dimension,classBObjects->amount);
        bnu::matrix<double> SA_X(dimension,classAObjects->amount), SB_X(dimension,classBObjects->amount);
        for (uint i = 0; i < dimension; i++) {
            for (uint j = 0; j < classAObjects->amount; j++) {
                MA(i,j) = static_cast<double>(asd.at(combo[i]-1)->classAverages[db.getClassNames()[0]]);
                SA_X(i,j) = static_cast<double>(classAObjects->objects.at(j).getFeatures()[combo[i]-1]);
            }
            for (uint j = 0; j < classBObjects->amount; j++) {
                MB(i,j) = static_cast<double>(asd.at(combo[i]-1)->classAverages[db.getClassNames()[1]]);
                SB_X(i,j) = static_cast<double>(classBObjects->objects.at(j).getFeatures()[combo[i]-1]);
            }
        }
        bnu::matrix<double> SA_final(dimension, dimension);
        bnu::matrix<double> SB_final(dimension, dimension);
        SA_final = prod(MA - SA_X, trans(MA - SA_X))/classAObjects->amount;
        SB_final = prod(MB - SB_X, trans(MB - SB_X))/classBObjects->amount;
        double top;
        for (uint i = 0; i < dimension; i++) {
            double tmp = MA(i,1) - MB(i,1);
            top += tmp * tmp;
        }
        bnu::matrix<double> final(dimension, dimension);
        final = SA_final + SB_final;
        double bottom = det(SA_final + SB_final);
        top = sqrt(top);
        double fisher = top/bottom;
        if (fisher > FP->FLD) {
            //cout << fisher << endl;
            FP->FLD = fisher;
            FP->max_ind = combo;
        }
    }
    return FP;
}

fisherPair* bruteForce(uint dimension, Database db) {
    return computeFisher(dimension, db, CreateCombinations(db.getNoFeatures(),dimension));
}

fisherPair* SFS(uint dimension, Database db) {
    fisherPair* FP = singleFisher(db);
    cout << " FISZER SINGEL SFS";
    for ( int i : FP->max_ind) {
        cout << i << " ";
    }
    cout << endl;
    for (uint i = 1; i < dimension; i++) {
        vector<vector<uint>> combo;
        for (uint j = 1; j < db.getNoFeatures(); j++) {
            vector<uint> tmp = FP->max_ind;
            if(!(std::find(tmp.begin(), tmp.end(), j) != tmp.end())) {
                tmp.push_back(j);
                combo.push_back(tmp);
            }
        }
        combo.push_back(FP->max_ind);
        FP = computeFisher(i, db, combo);
    }
    return FP;
}
