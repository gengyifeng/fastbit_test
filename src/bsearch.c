#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <time.h>
#define X_NAME "time"
#define Y_NAME "LAT"
#define Z_NAME "LON"
#define X 21900
//#define X_LIMIT 21900
#define Y 94
#define Z 192

typedef struct node_t{
   size_t idx;
   double val;
}node;

typedef struct result_t{
    size_t begin;
    size_t end;
}result;

inline int compare(const void *a,const void *b){
   double res=(*(node*)a).val-(*(node*)b).val;
   if(res>0) return 1;
   if(res<0) return -1;
   if(res==0) return 0;
}

size_t lsearch(const node* data,size_t len,double val,bool equal){
/*    printf("lsearch\n");*/
    int lp=0;
    int rp=len-1;
    int mid;
    while(rp>lp){
        mid=lp+((rp-lp)>>1);
        if(data[mid].val>val)
            rp=mid;
        else
            lp=mid+1;
/*        printf("lsearch %d %d %d\n",lp,mid ,rp);*/
    }
    if(rp==0){
       return 0; 
    }
/*    printf("rp %d val %lf\n",rp,data[rp].val);*/
    int res=-1;
    double tmp=data[rp].val;
    if(rp==len-1){
        if(equal&&tmp==val){
            return rp;
        }
        if(val>=tmp){
            return -1;
        }
    }
    int i;
    for(i=rp-1;i>=0;i--){
        if(data[i].val<tmp){
/*            printf("move %lf %lf\n",data[i].val,val);*/
            if(equal&&data[i].val==val){
                tmp=data[i].val;
                while(i>=0){
                    if(data[i].val<tmp){
                        return i+1;
                    }
                    i--;
                }
            }else{
                return i+1;
            }
            break;
        }
    }
/*    printf("res %d val %lf\n",res,data[res].val);*/
    return res;
}
size_t rsearch(const node* data,size_t len,double val,bool equal){
/*    printf("rsearch\n");*/
    int lp=0;
    int rp=len-1;
    int mid;
    while(rp>lp){
        mid=lp+((rp-lp)>>1);
        if(data[mid].val>val)
            rp=mid;
        else
            lp=mid+1;
    }
    if(rp==len-1)
        return len-1;
    int res=-1;
    double tmp=data[rp].val;
    if(rp==0){
        if(equal&&tmp==val){
            return rp;
        }
        if(val<=tmp){
            return -1;
        }
    }
    int i;
    for(i=rp-1;i>=0;i--){
        if(data[i].val<tmp){
            if(data[i].val==val){
               if(equal){
                    return i;
               }else{
                    tmp=data[i].val;
                    while(i>=0){
                        if(data[i].val<tmp){
                            return i;
                        }
                        i--;
                    }
               }
            }else{
                return i;
            }
        }
    }
    return res;
}
int bsearch(const node* data,size_t len,double min,double max,bool min_equal,bool max_equal,result * res){
   if( min>max||(min==max)&&(min_equal!=true||max_equal!=true)){
      return -1; 
   }
   res->begin=lsearch(data,len,min,min_equal);
   res->end=rsearch(data,len,max,max_equal);
   if(res->begin!=-1&&res->end!=-1){
       printf("hit number:%ld\n",res->end-res->begin+1);
       printf("begin %lf %lf end %lf %lf\n",data[res->begin-1].val,data[res->begin].val,data[res->end].val,data[res->end+1].val);
       return 0;
   }
   int i;
/*   for(i=res->begin;i<=res->end;i++){*/
/*       printf();V*/
/*   }*/
   return -1;
}
int main(int argc,char ** argv){
    clock_t begin, end;
    clock_t read_begin,read_end;
    begin=clock();
    int X_LIMIT;
    sscanf(argv[1],"%d",&X_LIMIT);
    FILE *fp=fopen(argv[2],"r");
    double min=DBL_MIN;
    double max=DBL_MAX;
    bool min_equal=true;
    bool max_equal=true;
    
    char lb,rb;
/*    printf("%s\n",argv[2]);*/
    sscanf(argv[3],"%c%lf,%lf%c",&lb,&min,&max,&rb);
    if(lb=='('){
        min_equal=false;
    }
    if(rb==')'){
        max_equal=false;
    }

    read_begin=clock();
    node* data=(node*)malloc(sizeof(node)*X_LIMIT*Y*Z);
    memset(data,0,sizeof(node)*X_LIMIT*Y*Z);
    read_end=clock();
/*    sscanf(argv[2],"%lf,%lf",&min,&max);*/
/*    printf("%f %f\n",min,max);*/
/*    read_begin=clock();*/
    fread(data,sizeof(node),X_LIMIT*Y*Z,fp);
    fclose(fp);
/*    read_end=clock();*/
    result res;
/*    read_begin=clock();*/
    bsearch(data,X_LIMIT*Y*Z,min,max,min_equal,max_equal,&res);
/*    read_end=clock();*/
    end=clock(); 
    printf("all time is %fs and read time is %fs\n",(double)(end-begin)/CLOCKS_PER_SEC,(double)(read_end-read_begin)/CLOCKS_PER_SEC);
    return 0;
}
