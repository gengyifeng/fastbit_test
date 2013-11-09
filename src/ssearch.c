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
#include <vector>
#include<unordered_set>
/*#include<boost/unordered_set.hpp>*/

/*#include "common.h"*/
/*using namespace boost;*/
using namespace std;

typedef struct qnode_t{
        vector<int> *res;
}qnode;

typedef struct rnode_t{
/*    double min;*/
/*    double max;*/
/*    unordered_set<int> vset;*/
    vector<int> vset;
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
    
    double step=1.0/len;
/*    vnode idx[len];*/
/*    vnode idx2[len];*/
    vnode *idx=new vnode[len];
    vnode *idx2=new vnode[len];
    int tmin,tmax;
    for(int i=0;i<len;i++){
/*        if(isrand){*/
            tmin=rand();
            if(isrand)
                tmax=tmin+rand()%(RAND_MAX-tmin);
            else{
                tmax=tmin;
            }
/*            tmax=tmin+rand()%(RAND_MAX-tmin);*/
            idx[i].min=1.0*tmin/RAND_MAX;
            if(isrand)
                idx[i].max=1.0*tmax/RAND_MAX;
            else{
                idx[i].max=idx[i].min+1.0/10000;
                if(idx[i].max>1)
                    idx[i].max=1;
            }
/*        }else{*/
/*            idx[i].min=0;*/
/*            idx[i].max=1;*/
/*        }*/
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
    vector<int> vres;
    for(int i=0;i<len;i++){
        if(!(idx[i].min>max||idx[i].max<min)){
/*           res.insert(i); */
            vres.push_back(i);
        }
    }
    gettimeofday(&tend,NULL);
    indextime=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;
    printf("Sequence scan time is %lf and hits is %d\n",indextime,vres.size());

/*    unordered_set<int> res2;*/
/*    res.clear();*/
/*|+    int rnum=floor(sqrt(len));+|*/
/*    int rnum=10000;*/
/*    rnode ridx[rnum+1];*/
/*    step=1.0/rnum;*/
/*    double buildbin=0,bintime=0;*/
/*    int begin,end;*/
/*    gettimeofday(&tbegin,NULL);*/
/*    for(int i=0;i<len;i++){*/
/*        begin=floor(idx[i].min/step);*/
/*        end=ceil(idx[i].max/step);*/
/*        if(begin<0) begin=0;*/
/*        if(end>rnum) end=rnum;*/
/*        for(int j=begin;j<=end;j++){*/
/*            ridx[j].vset.push_back(i);*/
/*|+            ridx[j].vset.insert(i);+|*/
/*        }*/
/*    }*/
/*    gettimeofday(&tend,NULL);*/
/*    buildbin=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;*/
/*    printf("Time of building bin index is %lf\n",buildbin);*/
/**/
/*    gettimeofday(&tbegin,NULL);*/
/*    begin=floor(min/step);*/
/*    end=ceil(max/step);*/
/*|+    vector<int>::iterator iter;+|*/
/*    unordered_set<int>::iterator siter;*/
/*    for(int i=begin;i<=end;i++){*/
/*|+        for(siter=ridx[i].vset.begin();siter!=ridx[i].vset.end();siter++){+|*/
/*        for(int j=0;j<ridx[i].vset.size();j++){*/
/*            res2.insert(ridx[i].vset[j]); */
/*|+            res2.insert(*siter); +|*/
/*        }*/
/*|+        printf("hello");+|*/
/*    } */
/*|+    for(siter=res.begin();siter!=res.end();siter++){+|*/
/*|+        if(res2.find(*siter)==res2.end()){+|*/
/*|+            printf("wrong result\n");+|*/
/*|+            break;+|*/
/*|+        }+|*/
/*|+    }+|*/
/*    gettimeofday(&tend,NULL);*/
/*    bintime=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;*/
/*    printf("Search time of bin index is %lf and hits is %d\n",bintime,res2.size());*/

    int rowsize=100;
    int allsize=(rowsize+1)*(rowsize+1);
    vres.clear(); 
    double buildtime=0;
    double gmin=0,gmax=1;
    gettimeofday(&tbegin,NULL);
    qnode *nds=(qnode *)calloc(sizeof(qnode),allsize);
    step=(gmax-gmin)/rowsize;
    int minpos,maxpos,pos;
    for(int i=0;i<len;i++){
        minpos=floor((idx[i].min-gmin)/step);
        maxpos=floor((idx[i].max-gmin)/step);  
        pos=maxpos*rowsize+minpos;     
        if(nds[pos].res==NULL){
            nds[pos].res=new vector<int>();
        } 
        nds[pos].res->push_back(i);
/*        if(i==86)*/
/*            printf("%lf %lf\n",idx[i].min,idx[i].max);*/
/*        printf("pos %d i %d\n",pos,i);*/
    }
    gettimeofday(&tend,NULL);
    buildtime=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000; 
    printf("build scatter index time is %lf\n",buildtime);
     
    gettimeofday(&tbegin,NULL);
    double searchtime=0;
/*    double min=0,max=0.5;*/
    minpos=floor((max-gmin)/step); 
    maxpos=floor((min-gmin)/step); 
/*    unordered_set<int> res;*/
    vector<int>::iterator iter;
    vector<int> test;
    int sum=0;
    for(int i=0;i<minpos;i++){
        for(int j=rowsize;j>=maxpos;j--){
/*            if(i>j) break;*/
            vector<int> *tmp=nds[j*rowsize+i].res;
//            printf("spos %d\n",j*rowsize+1);
            if(tmp!=NULL){
/*                sum+=tmp->size();*/
                if(i!=minpos&&j!=maxpos){
                    for(iter=tmp->begin();iter!=tmp->end();iter++){
/*                        res2.insert(idx[*iter].val);             */
                        vres.push_back(idx[*iter].val);             
/*                        test.push_back(idx[*iter].val);*/
/*                        printf("%d\n",idx[*iter].val);*/
                    }
                }else{
                    for(iter=tmp->begin();iter!=tmp->end();iter++){
                        if(idx[*iter].min<=max&&idx[*iter].max>=min){
/*                            res2.insert(idx[*iter].val);             */
                            vres.push_back(idx[*iter].val);             
/*                            test.push_back(idx[*iter].val);*/
/*                            printf("%d\n",idx[*iter].val);*/
                        }
                    }
                }   
            }
        }
    }
/*    printf("%d\n",sum);*/
/*    sort(test.begin(),test.end());*/
/*    for(int i=0;i<test.size();i++){*/
/*        printf("%d ",test[i]);*/
/*    }*/
/*    printf("\n");*/
    gettimeofday(&tend,NULL);
    searchtime=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000; 
    printf("scattter search time is %lf and hits is %d\n",searchtime,vres.size());

/*    unordered_set<int>::iterator iter2;*/
/*    for(iter2=res.begin();iter2!=res.end();iter2++){*/
/*        if(res2.find(*iter2)==res2.end()){*/
/*            printf("not the same result!\n");*/
/*        }*/
/*    }*/
/*    printf("hits is %d(same)\n",res.size());*/


/*    res2.clear();*/
/*    double sorttime=0,bsearchtime=0;*/
/**/
/**/
/*    gettimeofday(&tbegin,NULL);*/
/*    if(isrand){*/
/*        qsort(idx,len,sizeof(vnode),mincompare);*/
/*        qsort(idx2,len,sizeof(vnode),maxcompare);*/
/*    }*/
/*    gettimeofday(&tend,NULL);*/
/*    sorttime=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;*/
/*    printf("Time of building sorted index is %lf\n",sorttime);*/
/**/
/*    unordered_set<int> res3;*/
/*|+    res2.get_allocator().allocate(len);+|*/
/*|+    res3.get_allocator().allocate(len);+|*/
/*    res2.rehash(len);*/
/*    res3.rehash(len);*/
/*    gettimeofday(&tbegin,NULL);*/
/*    vnode omin,omax;*/
/*    omin.min=max;*/
/*    omax.max=min;*/
/*    vnode *low=upper_bound(idx,idx+len,omin,Vmin());*/
/*    vnode *high=upper_bound(idx2,idx2+len,omax,Vmax());*/
/*    gettimeofday(&tend,NULL);*/
/*    bsearchtime=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;*/
/*    printf("Bsearch time is %lf and hits is %d\n",bsearchtime,res3.size());*/
/*    int hbound=(high-idx2);*/
/*    int lbound=(low-idx);*/
/*    if(len-hbound>lbound){*/
/*        for(int i=0;i<lbound;i++){*/
/*            if(idx[i].max>=min)*/
/*                res2.insert(idx[i].val);*/
/*        }*/
/*|+        for(int i=hbound;i<len;i++){+|*/
/*|+            if(res2.find(idx2[i].val)!=res2.end())+|*/
/*|+                res3.insert(idx2[i].val);+|*/
/*|+        }+|*/
/*    }else{*/
/*        for(int i=hbound;i<len;i++){*/
/*            if(idx[i].min<=max)*/
/*                res2.insert(idx2[i].val);*/
/*        }*/
/*|+        for(int i=0;i<lbound;i++){+|*/
/*|+            if(res2.find(idx[i].val)!=res2.end())+|*/
/*|+                res3.insert(idx[i].val);+|*/
/*|+        }+|*/
/*    }*/
/*    gettimeofday(&tend,NULL);*/
/*    bsearchtime=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;*/
/*    printf("Final time is %lf and hits is %d\n",bsearchtime,res2.size());*/

    

    return 0;
}

/*#endif*/


