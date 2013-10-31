/*#ifndef RSEARCH_H*/
/*#define RESARCH_H*/
#include <stdio.h>
#include <stdlib.h>
/*#include <unordered_set>*/
#include <set>
#include <algorithm>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include<boost/unordered_set.hpp>

/*#include "common.h"*/
using namespace boost;
using namespace std;
typedef struct rnode_t{
/*    double min;*/
/*    double max;*/
    unordered_set<int> vset;
}rnode;

typedef struct vnode_t{
    double min;
    double max;
    int val;
}vnode;

int mincompare(const void *a,const void *b){
    double res=(*(vnode*)a).min-(*(vnode*)b).min;
    if(res>0) return 1;
    if(res<0) return -1;
    return 0;
}

int maxcompare(const void *a,const void *b){
    double res=(*(vnode*)a).max-(*(vnode*)b).max;
    if(res>0) return 1;
    if(res<0) return -1;
    return 0;
}

class Vmin{
    public:
    bool operator()(const vnode &a,const vnode &b){
        return a.min<b.min;
    }
};
class Vmax{
    public:
    bool operator()(const vnode &a,const vnode &b){
        return a.max<b.max;
    }
};
/*bool vnodemaxcomapre(const vnode &a,const vnode &b){*/
/*    return a.max<b.max;*/
/*}*/
/*void init_rnodes(rnode *nodes,int pos, double min,double max,int level,vnode * vns,int vsize,int max_level);*/
/*void rquery(set<int> &vset,double min,double max,rnode* nodes,int pos,int level,int max_level);*/
/*int get_max_level(int partitions);*/
/*size_t get_tree_size(int max_level);*/
int main(int argc,char **argv){
    if(argc<2){
        printf("Usage:%s index_length [min,max] -r\nThe value range for the blocks is [0,1], -r means to use random range for each block, otherwise [0,1] is used by default\n",argv[0]);
        exit(1);
    }
    int len;
    double min,max;
    bool isrand=false;
    sscanf(argv[1],"%d",&len);
    sscanf(argv[2],"[%lf,%lf]",&min,&max);
    if(min>max){
        printf("Error, min is greater than max\n");
        exit(1);
    }
    if(argc>=4){
        if(strcmp(argv[3],"-r")==0){
            isrand=true;
        }
    }
/*    if(isrand)*/
/*        printf("min %lf max %lf\n",min,max);*/
    
    double begin=0,end=1;
    double step=1.0/len;
/*    vnode idx[len];*/
/*    vnode idx2[len];*/
    vnode *idx=new vnode[len];
    vnode *idx2=new vnode[len];
    int tmin,tmax;
    for(int i=0;i<len;i++){
        if(isrand){
            tmin=rand();
            tmax=tmin+rand()%(RAND_MAX-tmin);
            idx[i].min=1.0*tmin/RAND_MAX;
            idx[i].max=1.0*tmax/RAND_MAX;
        }else{
            idx[i].min=0;
            idx[i].max=1;
        }
        idx[i].val=i;
        idx2[i].min=idx[i].min;
        idx2[i].max=idx[i].max;
        idx2[i].val=idx[i].val;
/*        printf("imin %lf imax %lf\n",idx[i].min,idx[i].max);*/
    }
    struct timeval tbegin,tend;
    double indextime=0;
    gettimeofday(&tbegin,NULL);
    unordered_set<int> res;
    for(int i=0;i<len;i++){
        if(!(idx[i].min>max||idx[i].max<min)){
           res.insert(i); 
        }
    }
    gettimeofday(&tend,NULL);
    indextime=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;
    printf("Sequence scan time is %lf and hits is %d\n",indextime,res.size());

    unordered_set<int> res2;
/*    res.clear();*/
/*    int rnum=floor(sqrt(len));*/
/*    rnode ridx[rnum];*/
/*    step=1.0/rnum;*/
/*    double buildbin=0,bintime=0;*/

/*    gettimeofday(&tbegin,NULL);*/
/*    for(int i=0;i<len;i++){*/
/*        begin=floor(idx[i].min/step);*/
/*        end=ceil(idx[i].max/step);*/
/*        for(int j=begin;j<end;j++){*/
/*            ridx[j].vset.insert(i);*/
/*        }*/
/*    }*/
/*    gettimeofday(&tend,NULL);*/
/*    buildbin=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;*/
/*    printf("Time of building bin index is %lf\n",buildbin);*/

/*    gettimeofday(&tbegin,NULL);*/
/*    begin=floor(min/step);*/
/*    end=ceil(max/step);*/
/*    unordered_set<int>::iterator iter;*/
/*    for(int i=begin;i<end;i++){*/
/*        for(iter=ridx[i].vset.begin();iter!=ridx[i].vset.end();iter++){*/
/*            res2.insert(*iter); */
/*        }*/
/*    } */
/*    for(iter=res.begin();iter!=res.end();iter++){*/
/*        if(res2.find(*iter)==res2.end()){*/
/*            printf("wrong result\n");*/
/*            break;*/
/*        }*/
/*    }*/
/*    gettimeofday(&tend,NULL);*/
/*    bintime=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;*/
/*    printf("Search time of bin index is %lf and hits is %d\n",bintime,res2.size());*/

    res2.clear();
    double sorttime=0,bsearchtime=0;


    gettimeofday(&tbegin,NULL);
    if(isrand){
        qsort(idx,len,sizeof(vnode),mincompare);
        qsort(idx2,len,sizeof(vnode),maxcompare);
    }
    gettimeofday(&tend,NULL);
    sorttime=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;
    printf("Time of building sorted index is %lf\n",sorttime);

    unordered_set<int> res3;
/*    res2.get_allocator().allocate(len);*/
/*    res3.get_allocator().allocate(len);*/
    res2.rehash(len);
    res3.rehash(len);
    gettimeofday(&tbegin,NULL);
    vnode omin,omax;
    omin.min=max;
    omax.max=min;
    vnode *low=upper_bound(idx,idx+len,omin,Vmin());
    vnode *high=upper_bound(idx2,idx2+len,omax,Vmax());
    gettimeofday(&tend,NULL);
    bsearchtime=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;
    printf("Bsearch time is %lf and hits is %d\n",bsearchtime,res3.size());
    int hbound=(high-idx2);
    int lbound=(low-idx);
    if(len-hbound>lbound){
        for(int i=0;i<lbound;i++){
            if(idx[i].max>=min)
                res2.insert(idx[i].val);
        }
/*        for(int i=hbound;i<len;i++){*/
/*            if(res2.find(idx2[i].val)!=res2.end())*/
/*                res3.insert(idx2[i].val);*/
/*        }*/
    }else{
        for(int i=hbound;i<len;i++){
            if(idx[i].min<=max)
                res2.insert(idx2[i].val);
        }
/*        for(int i=0;i<lbound;i++){*/
/*            if(res2.find(idx[i].val)!=res2.end())*/
/*                res3.insert(idx[i].val);*/
/*        }*/
    }
    gettimeofday(&tend,NULL);
    bsearchtime=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;
    printf("Final time is %lf and hits is %d\n",bsearchtime,res2.size());
    return 0;
}

/*#endif*/


