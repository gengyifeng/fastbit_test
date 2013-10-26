#include "rsearch.h"


void init_rnodes(rnode *nodes,int pos, double min,double max,int level,vnode * vns,int vsize,int max_level){
    if(level>max_level)
        return;
    int left=pos*2+1;
    int right=pos*2+2;
    nodes[pos].min=min;
    nodes[pos].max=max;
    nodes[pos].level=level;
    init_rnodes(nodes,left,min,min+(max-min)/2,level+1,vns,vsize,max_level);
    init_rnodes(nodes,right,min+(max-min)/2,max,level+1,vns,vsize,max_level);
    if(level==max_level){ 
        int i=0;
        for(i=0;i<vsize;i++){
            if(vns[i].max<min||vns[i].min>max){
                continue;
            }else{
                nodes[pos].vset.insert(vns[i].val);
            }
/*            printf("vmin %lf vmax %lf \n",vns[i].min,vns[i].max);*/
        }
/*        nodes[pos].size=nodes[pos].vset.size();*/
/*        printf("pos %d min %lf max %lf size %d\n",pos,min,max,nodes[pos].size);*/
/*        nodes[pos].vals=(int *)calloc(nodes[pos].size,sizeof(int));*/
/*        i=0;*/
/*        for(std::set<int>::iterator iter=nodes[pos].vset.begin();iter!=nodes[pos].vset.end();iter++){*/
/*            nodes[pos].vals[i++]=*iter;*/
/*        }*/
    }else{
        int i=0;
        for(std::tr1::unordered_set<int>::iterator iter=nodes[left].vset.begin();iter!=nodes[left].vset.end();iter++){
            nodes[pos].vset.insert(*iter);
/*            nodes[pos].vals[i++]=*iter;*/
        }
        i=0;
        for(std::tr1::unordered_set<int>::iterator iter=nodes[right].vset.begin();iter!=nodes[right].vset.end();iter++){
            nodes[pos].vset.insert(*iter);
/*            nodes[pos].vals[i++]=*iter;*/
        }
/*        nodes[pos].size=nodes[pos].vset.size();*/
/*        nodes[pos].vals=(int *)calloc(nodes[pos].size,sizeof(int));*/
/*        i=0;*/
/*        for(std::set<int>::iterator iter=nodes[pos].vset.begin();iter!=nodes[pos].vset.end();iter++){*/
/*            nodes[pos].vals[i++]=*iter;*/
/*        }*/
    }
}
void rquery(std::set<int> &vset,double min,double max,rnode* nodes,int pos,int level,int max_level){
    if(level>max_level)
        return;
    int left=pos*2+1;
    int right=pos*2+2;
    int parent=(pos-1)/2;
    if(nodes[pos].min>max||nodes[pos].max<min)
        return; 
/*    printf("pos %d level %d pmin %lf pmax %lf min %lf max %lf size %d\n",pos,level,min,max,nodes[pos].min,nodes[pos].max,nodes[pos].size);*/
    if(level==max_level){
        for(std::tr1::unordered_set<int>::iterator iter=nodes[pos].vset.begin();iter!=nodes[pos].vset.end();iter++){
            vset.insert(*iter);
        }
        return;
    }
    if(nodes[pos].min>=min&&nodes[pos].max<=max){
        for(std::tr1::unordered_set<int>::iterator iter=nodes[pos].vset.begin();iter!=nodes[pos].vset.end();iter++){
            vset.insert(*iter);
        }
        if(min!=nodes[pos].min&&level!=0)
            rquery(vset,min,nodes[pos].min,nodes,parent,level-1,max_level);
        if(max!=nodes[pos].max&&level!=0)
            rquery(vset,nodes[pos].max,max,nodes,parent,level-1,max_level);
        return;
    }
    if(min<=nodes[left].max&&max>nodes[left].max){
        rquery(vset,min,max,nodes,left,level+1,max_level);
        rquery(vset,min,max,nodes,right,level+1,max_level);
        return;
    }
    if(max<=nodes[left].max){
        rquery(vset,min,max,nodes,left,level+1,max_level);
        return;
    }
    if(min>=nodes[left].max){
        rquery(vset,min,max,nodes,right,level+1,max_level);
        return;
    }
}
int get_max_level(int partitions){
    int i=0,n=1;
    while(n<partitions){
        n=n<<1;
        i++;
    }
    return i;
}
size_t get_tree_size(int max_level){
    int i=0;
    size_t n=1;
    for(i=0;i<=max_level;i++){
        n=n<<1;
    }
    return n-1;
}
void init_vnodes(vnode *vns,int size){
    int i;
    for(i=0;i<size;i++){
        vns[i].min=i;
        vns[i].max=i+1;
        vns[i].val=i;
    }
}
/*int main(){*/
/*    int partitions=8;*/
/*    int max_level=get_max_level(partitions);*/
/*    printf("level %d\n",max_level);*/
/*|+    rnode *nodes=(rnode *)calloc(get_tree_size(max_level),sizeof(rnode));+|*/
/*    rnode nodes[get_tree_size(max_level)];*/
/*    printf("tree size %d\n",get_tree_size(max_level));*/
/*    vnode vns[partitions];*/
/*    init_vnodes(vns,partitions);*/
/*    init_rnodes(nodes,0,0,512,0,vns,partitions,max_level);*/
/*    int i;*/
/*    for(i=0;i<256;i++){*/
/*|+        printf("pos %d min %lf max %lf size %d level %d\n",i,nodes[i].min,nodes[i].max,nodes[i].size,nodes[i].level);+|*/
/*    }*/
/*    std::set<int> vset;*/
/*    rquery(vset,10,11,nodes,0,0,max_level);*/
/*    for(std::set<int>::iterator iter=vset.begin();iter!=vset.end();iter++){*/
/*        printf("%d\n",*iter);*/
/*    }*/
/**/
/*    return 0;*/
/*}*/
