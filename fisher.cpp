#include <vector>
#include <algorithm>
#include <iostream>
#include "database.h"
#include "fisher.h"

using namespace std;

vector<vector<int>> CreateCombinations(int range, int size) {
       std::vector<std::vector<int>> combinations;
       std::vector<bool> v(range);
       std::fill(v.begin(), v.begin() + size, true);
       do {
           std::vector<int> singleCombination;
           for (int i = 0; i < range; ++i) {
               if (v[i]) {
                   singleCombination.push_back(i+1);
               }
           }
           combinations.push_back(singleCombination);
       } while (prev_permutation(v.begin(), v.end()));
       return combinations;
}

fisherPair* computeFisher(Database db) {
    fisherPair* FP;
    float tmp;
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

void computeFisher(int dimension, Database db) {
    vector<vector<int>> combinations = CreateCombinations(db.getNoFeatures(),dimension);
    for (vector<int> &combination : combinations) {
        for (int &value : combination) {
            cout << (value) << " ";
        }
        cout << "\n";
    }
}
