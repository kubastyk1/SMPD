#include <vector>
#include <algorithm>
#include <iostream>
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



fisherPair* computeFisher(Database db) {
    fisherPair* FP = new fisherPair;
    float tmp;
    ASdata asd;
    for (unsigned int i = 0; i < db.getNoFeatures(); ++i)
    {
        map<string, float> classAverages;
        map<string, float> classStds;
        for (auto const &ob : db.getObjects())
        {
            classAverages[ob.getClassName()] += ob.getFeatures()[i];
            classStds[ob.getClassName()] += ob.getFeatures()[i] * ob.getFeatures()[i];
        }
        for_each(db.getClassCounters().begin(), db.getClassCounters().end(), [&](const pair<string, int> &it)
        {
            classAverages[it.first] /= it.second;
            classStds[it.first] = sqrt(classStds[it.first] / it.second - classAverages[it.first] * classAverages[it.first]);
        }
        );
        tmp = abs(classAverages[ db.getClassNames()[0] ] - classAverages[db.getClassNames()[1]]) / (classStds[db.getClassNames()[0]] + classStds[db.getClassNames()[1]]);
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
    for (vector<uint> &combination : combinations) {
        for (uint &value : combination) {
            cout << (value) << " ";
        }
        cout << "\n";
    }
}
