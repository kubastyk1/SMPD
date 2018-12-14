#include "database.h"
#include <vector>

using namespace std;
#ifndef FISHER_H
#define FISHER_H

typedef unsigned int uint;
typedef struct {
    double FLD;
    vector<uint> max_ind;
} fisherPair;

fisherPair* singleFisher(Database db);
fisherPair* bruteForce(uint dimension, Database db);
fisherPair* SFS(uint dimension, Database db);

#endif // FISHER_H
