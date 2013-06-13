#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
/*#include <time.h>*/
#include <sys/time.h>
#include "common.h"
/*typedef enum { false, true } bool;*/
#define X 21900
//#define X_LIMIT 100
#define Y 94
#define Z 192
typedef struct condtion_t{
    double *mins;
    bool *minequal;
    double *minmap;
    double minsize;
    double *maxs;
    bool *maxequal;
    double *maxmap;
    double maxsize;
}cond;
/*void init_cond(DIMS dims,){*/
/*    int i;*/
/**/
/*}*/
size_t check_index(int *index,int *shape,int size){
    int i;
    size_t tmp=index[0];
    for(i=0;i<size-1;i++){
        tmp=tmp*shape[i+1]+index[i+1];
    }
    return tmp;
}
size_t validate(size_t *idx,DIMS dims,cond c){
    int i;
    for(i=0;i<c.minsize;i++){
/*       dims.dimvals[idx[c.minmap[i]]];*/
        
    }
}
inline void print_to_buf(char *s,TYPE type,void * data){
    switch(type){
        case BYTE:
            sprintf(s,"%c,",*(char *)data);
            break;
        case SHORT:
            sprintf(s,"%d,",*(short *)data);
            break;
        case INT:
            sprintf(s,"%d,",*(int *)data);
            break;
        case LONG:
            sprintf(s,"%ld,",*(long *)data);
            break;
        case FLOAT:
            sprintf(s,"%f,",*(float *)data);
            break;
        case DOUBLE:
            sprintf(s,"%lf,",*(double *)data);
            break;
        defaut:
            printf("unknown type\n");
    }
}

int get_row_size(DIMS *dims,int *cols,int cols_size){
    int i;
    int size=0;
    for(i=0;i<cols_size;i++){
        size+=get_type_size(dims->types[cols[i]]);
    }
    return size;
}
void get_offsets(int *offset,int *sizes,DIMS *dims,int *cols,int cols_size){
    int i;
    offset[0]=0;
    sizes[0]=get_type_size(dims->types[cols[0]]);
    for(i=1;i<cols_size+1;i++){
        sizes[i]=get_type_size(dims->types[cols[i]]);
        offset[i]=offset[i-1]+sizes[i]; 
    }
    sizes[cols_size]=get_type_size(dims->var_type);
    offset[cols_size]=offset[cols_size-1]+get_type_size(dims->var_type);
}
int scan(result *cres,result *res,FILE *vfp,FILE *ifp,DIMS *dims,int *cols,int cols_size,FILE *ofp,MODE m){ 
    struct timeval tbegin,tend;
    struct timeval obegin, oend;
    if(cres->begin>cres->end)
        return -1;
    size_t i;
    int size=sizeof(cnode);
    int idx_size=sizeof(size_t);
    gettimeofday(&tbegin,NULL);
    cnode *data=(cnode*)calloc(cres->end-cres->begin+1,sizeof(cnode));
    size_t *idx_data=(size_t *)calloc(res->end-res->begin+1,sizeof(size_t));
    gettimeofday(&tend,NULL);
/*    printf("time for read buffer is %f\n",tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000);  */
      
    double readtime=0;
    double wtime=0;
     
    gettimeofday(&tbegin,NULL);
    fseek(vfp,size*cres->begin,SEEK_SET);
    fread(data,size,cres->end-cres->begin+1,vfp);
    fseek(ifp,idx_size*res->begin,SEEK_SET);
    fread(idx_data,idx_size,res->end-res->begin+1,ifp);
    gettimeofday(&tend,NULL);
    readtime=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;  
    int k;
    if(m==TEXT){
        if(cols==NULL||cols_size==0){
            for(i=0;i<res->end-res->begin+1;i++){
                for(k=0;k<data[i].repeat;k++){
                    fprintf(ofp,"%f\n",data[i].val);
                }
            }
        }else{
            size_t dshape[dims->dims_size];
            size_t idx[dims->dims_size];
            get_dshape(dshape,dims->shape,dims->dims_size);
            int *offsets=(int *)calloc(cols_size+1,sizeof(int));

            int j;
            int *typesizes=(int *)calloc(cols_size+1,sizeof(int));
            get_offsets(offsets,typesizes,dims,cols,cols_size);
            int row_size=get_row_size(dims,cols,cols_size);
            int buf_size=row_size*10;
            char *buf=(char *)calloc(buf_size,sizeof(char));
            int buf_pos=0;
            for(i=0;i<cres->end-cres->begin+1;i++){
    /*            printf("before idx %d\n",data[i].idx);*/
                for(k=0;k<data[i].repeat;k++){
                    get_idx(idx,idx_data[data[i].idx+k-res->begin],dshape,dims->dims_size); 
        /*            printf("after idx %d\n",check_index(idx,dims->shape,dims->dims_size));*/
                    for(j=0;j<cols_size;j++){
                        print_to_buf(buf+buf_pos,dims->types[cols[j]],(char *)dims->dimvals[cols[j]]+idx[cols[j]]*typesizes[j]); 
                        buf_pos=strlen(buf);
                    }
                    print_to_buf(buf+buf_pos,dims->var_type,&(data[i].val));
                    buf[strlen(buf)-1]='\n';
                    
                    fwrite(buf,1,strlen(buf),ofp);
                    bzero(buf,buf_size);
                    buf_pos=0;
                }
            }
            free(buf);
            free(offsets);
            free(typesizes);
        }
    }else{
        if(cols==NULL||cols_size==0){
            int var_size=get_type_size(dims->var_type);
            for(i=0;i<cres->end-cres->begin+1;i++){
                for(k=0;k<data[i].repeat;k++){
                    fwrite(&(data[i].val),var_size,1,ofp);
                }
            }
        }else{
            size_t dshape[dims->dims_size];
            size_t idx[dims->dims_size];
            get_dshape(dshape,dims->shape,dims->dims_size);
            int *offsets=(int *)calloc(cols_size+1,sizeof(int));
            int j;
            int *typesizes=(int *)calloc(cols_size+1,sizeof(int));
            get_offsets(offsets,typesizes,dims,cols,cols_size);
            int row_size=get_row_size(dims,cols,cols_size);
            char *buf=(char *)calloc(row_size+10,sizeof(char));
            gettimeofday(&obegin,NULL);
            for(i=0;i<cres->end-cres->begin+1;i++){
                for(k=0;k<data[i].repeat;k++){
/*                gettimeofday(&tbegin,NULL);*/
                get_idx(idx,idx_data[data[i].idx+k-res->begin],dshape,dims->dims_size); 
/*                gettimeofday(&tend,NULL);*/
/*                idxtime+=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;*/
                for(j=0;j<cols_size;j++){

                    memcpy((char*)(buf+offsets[j]),(char*)dims->dimvals[cols[j]]+idx[cols[j]]*typesizes[j],typesizes[j]);
    /*                fprintf(ofp,"%lf,",((double*)(dims->dimvals[cols[j]]))[idx[cols[j]]]);*/
                }
    /*            fprintf(ofp,"%lf\n",data[i].val); */
                memcpy((char*)(buf+offsets[j]),&(data[i].val),typesizes[j]);
/*                fwrite(buf,1,row_size,ofp);*/
                }
            }
            gettimeofday(&oend,NULL);
            wtime=oend.tv_sec-obegin.tv_sec+1.0*(oend.tv_usec-obegin.tv_usec)/1000000;
            printf("write time is %f and read time is %f\n",wtime,readtime);
            free(buf);
            free(offsets);
            free(typesizes);
        }
    
    }
    free(data);
    free(idx_data);
    gettimeofday(&tend,NULL);
    printf("scan time is %fs\n",tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000);
    return 0;
}
void print_res(FILE *fp){
    int i=0;
    double data; 
    while(!feof(fp)){
        if(fread(&data,sizeof(double),1,fp)>0){
            i++;
            printf("%lf,",data);
            if(i%3==0){ 
                printf("\n");
            }
        }
    };
    printf("count %d\n",i); 
}
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
int binary_search(const node* data,size_t len,double min,double max,bool min_equal,bool max_equal,result * res){
   if( min>max||(min==max)&&(min_equal!=true||max_equal!=true)){
      return -1; 
   }
   struct timeval tbegin,tend;
   gettimeofday(&tbegin,NULL);
   res->begin=lsearch(data,len,min,min_equal);
   res->end=rsearch(data,len,max,max_equal);
   gettimeofday(&tend,NULL);
   if(res->begin!=-1&&res->end!=-1){
       printf("hit number:%ld\n",res->end-res->begin+1);
       printf("begin %lf %lf end %lf %lf\n",data[res->begin-1].val,data[res->begin].val,data[res->end].val,data[res->end+1].val);
       printf("binary_search time:%f\n",tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000);
       return 0;
   }
   return -1;
}

inline void read_from_file(FILE * fp,size_t size,size_t pos,cnode *data){
        fseek(fp,size*pos,SEEK_SET);
        fread(data,size,1,fp);
/*        printf("%f %d\n",data->val,data->repeat);*/
}
size_t frsearch(FILE * fp,size_t size,size_t len,double val,bool equal){
/*    printf("rsearch\n");*/
    int lp=0;
    int rp=len-1;
    int mid;
    cnode data;
    while(rp>lp){
        mid=lp+((rp-lp)>>1);
        read_from_file(fp,size,mid,&data);
        if(data.val>val)
            rp=mid;
        else
            lp=mid+1;
    }
    if(rp==len-1)
        return len-1;
    int res=-1;
    
    read_from_file(fp,size,rp,&data);
    double tmp=data.val;
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
        read_from_file(fp,size,i,&data);
        if(data.val<tmp){
            if(data.val==val){
               if(equal){
                    return i;
               }else{
                    tmp=data.val;
                    while(i>=0){
                        read_from_file(fp,size,i,&data);
                        if(data.val<tmp){
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
size_t flsearch(FILE * fp,size_t size,size_t len,double val,bool equal){
/*    printf("lsearch\n");*/
    int lp=0;
    int rp=len-1;
    int mid;
    cnode data;
    while(rp>lp){
        mid=lp+((rp-lp)>>1);
        read_from_file(fp,size,mid,&data);
        if(data.val>val)
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
    read_from_file(fp,size,rp,&data);
    double tmp=data.val;
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
        read_from_file(fp,size,i,&data);
        if(data.val<tmp){
/*            printf("move %lf %lf\n",data[i].val,val);*/
            if(equal&&data.val==val){
                tmp=data.val;
                while(i>=0){
                    read_from_file(fp,size,i,&data);
                    if(data.val<tmp){
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

int fbsearch(FILE * fp,size_t size,size_t len,double min,double max,bool min_equal,bool max_equal,result * cres,result *res){
   
   if( min>max||(min==max)&&(min_equal!=true||max_equal!=true)){
      return -1; 
   }
   struct timeval tbegin,tend;
   gettimeofday(&tbegin,NULL);
   cres->begin=flsearch(fp,size,len,min,min_equal);
   cres->end=frsearch(fp,size,len,max,max_equal);
   gettimeofday(&tend,NULL);
   if(res->begin!=-1&&res->end!=-1){
       cnode data; 
       read_from_file(fp,sizeof(cnode),cres->begin,&data);
       res->begin=data.idx;
       read_from_file(fp,sizeof(cnode),cres->end,&data);
       res->end=data.idx+data.repeat-1;
       printf("hit number:%ld\n",res->end-res->begin+1);
/*       printf("begin %lf %lf end %lf %lf\n",data[res->begin-1].val,data[res->begin].val,data[res->end].val,data[res->end+1].val);*/
       printf("bsearch time:%f\n",tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000);
       return 0;
   }
   return -1;
}

int main(int argc,char ** argv){
    struct timeval tbegin, tend;
    struct timeval read_tbegin,read_tend;
    gettimeofday(&tbegin,NULL);
    int X_LIMIT;
    sscanf(argv[1],"%d",&X_LIMIT);
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
    
/*    node* data=(node*)calloc(sizeof(node),X_LIMIT*Y*Z);*/
    FILE *fp=fopen(argv[2],"r");
    char ifilename[128]={0};
    sprintf(ifilename,"%s_idx",argv[2]);
    FILE *ifp=fopen(ifilename,"r");

/*    memset(data,0,sizeof(node)*X_LIMIT*Y*Z);*/
/*    sscanf(argv[2],"%lf,%lf",&min,&max);*/
/*    printf("%f %f\n",min,max);*/
/*    fread(data,sizeof(node),X_LIMIT*Y*Z,fp);*/
/*    fclose(fp);*/
    result res,cres;
/*    bsearch(data,X_LIMIT*Y*Z,min,max,min_equal,max_equal,&res);*/
    fseek(fp,0,SEEK_END); 
    size_t fsize = ftell(fp);
    fbsearch(fp,sizeof(cnode),fsize/sizeof(cnode),min,max,min_equal,max_equal,&cres,&res);
/*  free(data);*/
    DIMS dims;
    size_t shape[3]={21900,94,192};
    TYPE types[3]={DOUBLE,DOUBLE,DOUBLE};
    TYPE var_type= DOUBLE;
    FILE **fps=(FILE **)calloc(3,sizeof(FILE));
    fps[0]=fopen("time","r");
    fps[1]=fopen("LAT","r");
    fps[2]=fopen("LON","r");
    init_dims(&dims,3,shape,types,var_type,fps);
    int cols[3]={0,1,2};
    int cols_size=3;
    FILE *ofp;
    gettimeofday(&read_tbegin,NULL);
    if(argc>=5){
        ofp=fopen(argv[4],"w");
/*        scan(res.begin,res.end,fp,&dims,NULL,0,ofp,TEXT);*/
/*        scan(res.begin,res.end,fp,&dims,cols,cols_size,ofp,TEXT);*/
        scan(&cres,&res,fp,ifp,&dims,cols,cols_size,ofp,BINARY);
/*        scan(&cres,&res,fp,ifp,&dims,cols,cols_size,ofp,TEXT);*/
    }
    gettimeofday(&read_tend,NULL);
    if(argc>=5)
        fclose(ofp);
    destory_dims(&dims);
    fclose(fp);
    fclose(ifp);
/*    ofp=fopen(argv[4],"r");*/
/*    print_res(ofp);*/
/*    fclose(ofp);*/
    gettimeofday(&tend,NULL);
    printf("all time is %fs and scan time is %fs\n",tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000,read_tend.tv_sec-read_tbegin.tv_sec+1.0*(read_tend.tv_usec-read_tbegin.tv_usec)/1000000);
    return 0;
}
