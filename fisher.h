#include "database.h"
#include <vector>

using namespace std;
#ifndef FISHER_H
#define FISHER_H

struct fisherPair {
    float FLD;
    int max_ind;
};

vector<vector<int>> CreateCombinations(int range, int size);
fisherPair* computeFisher(Database db);
void computeFisher(int dimension, Database db);

#endif // FISHER_H
