#include "database.h"
#include <vector>

using namespace std;
#ifndef FISHER_H
#define FISHER_H

typedef unsigned int uint;
typedef struct {
    float FLD;
    uint max_ind;
} fisherPair;

vector<vector<int>> CreateCombinations(uint range, int size);
fisherPair* computeFisher(Database db);
void computeFisher(uint dimension, Database db);

#endif // FISHER_H
