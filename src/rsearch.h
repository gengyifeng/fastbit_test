#ifndef RSEARCH_H
#define RESARCH_H
#include <stdio.h>
#include <stdlib.h>
//#include <unordered_set>
#include <set>
#include <algorithm>
#ifdef __GNUC__
#include <tr1/unordered_set>
#else
#include <unordered_set>
#endif
using namespace std;
typedef struct rnode_t{
    double min;
    double max;
    int level;
    std::tr1::unordered_set<int> vset;
//    int size;
//    int *vals;
}rnode;

typedef struct vnode_t{
    double min;
    double max;
    int val;
}vnode;

void init_rnodes(rnode *nodes,int pos, double min,double max,int level,vnode * vns,int vsize,int max_level);
void rquery(std::set<int> &vset,double min,double max,rnode* nodes,int pos,int level,int max_level);
int get_max_level(int partitions);
size_t get_tree_size(int max_level);

#endif


