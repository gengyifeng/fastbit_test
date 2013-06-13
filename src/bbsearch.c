#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
/*#include <time.h>*/
#include <sys/time.h>
#include "common.h"
#include "rsearch.h"
/*typedef enum { false, true } bool;*/
#define X 21900
//#define X_LIMIT 100
#define Y 94
#define Z 192
typedef struct condition_t{
    double *min;
    bool *minequal;
    double *max;
    bool *maxequal;
    bool *valid; 
    int size;
}cond;

/*void init_cond(DIMS dims,){*/
/*    int i;*/
/**/
/*}*/
TYPE get_type(char *name){
   if(!strcmp(name,"byte"))
       return BYTE;
   else if(!strcmp(name,"short"))
       return SHORT;
   else if(!strcmp(name,"int"))
       return INT;
   else if(!strcmp(name,"float"))
       return FLOAT;
   else if(!strcmp(name,"double"))
       return DOUBLE;
   return UNKNOWN;
}
size_t check_index(int *index,int *shape,int size){
    int i;
    size_t tmp=index[0];
    for(i=0;i<size-1;i++){
        tmp=tmp*shape[i+1]+index[i+1];
    }
    return tmp;
}
inline bool check_dim_condition(size_t *idx,size_t *begin,size_t *end,int dims_size){
    int i;
/*    bool res=true;*/
    for(i=0;i<dims_size;i++){
        if(idx[i]>end[i]||idx[i]<begin[i]){
            return false;
        }
    }
    return true;
}
/*inline void get_idx_in_block(size_t *idx,unsigned int offset,size_t * begin,size_t *countdshape,size_t* int dims_size){*/
/*    get_idx_in_block(idx,offset,countdshape,begin,dims_size);*/
/*    size_t count[dims_size];*/
/*    size_t idx[dims_size];*/
/*    int i;*/
/*    get_idx(idx,blockid,newdshape,dims_size);*/
/*    for(i=0;i<dims_size;i++){*/
/*        if(idx[i]!=bound[i]-1)*/
/*            count[i]=shape[i]/bound[i];*/
/*        else*/
/*            count[i]=shape[i]-(shape[i]/bound[i])*(bound[i]-1);*/
/*    }*/
/*    get_dshape(countdshape,count,dims_size);*/
/*    get_index(countdshape);*/

    
/*}*/
inline void get_begin_count_countdshape(size_t *begin,size_t *count,size_t *countdshape,size_t id,size_t *shape,size_t *newdshape,int *bound,int dims_size){
/*    size_t count[dims_size];*/
    size_t idx[dims_size];
    int i;
    get_idx(idx,id,newdshape,dims_size);
    size_t len;
    for(i=0;i<dims_size;i++){
        len=shape[i]/bound[i];
        begin[i]=idx[i]*len;
        if(idx[i]!=bound[i]-1){
            count[i]=len;
        }else{
            count[i]=shape[i]-len*(bound[i]-1);
        }
    }
    get_dshape(countdshape,count,dims_size);
}
/*size_t validate(size_t *idx,DIMS dims,cond c){*/
/*    int i;*/
/*    for(i=0;i<c.minsize;i++){*/
/*       dims.dimvals[i</c>dx[c.minmap[i]]];*/
        
/*    }*/
/*}*/

/*
 * print the data to a string;
 */
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
/*
 * get the size for the output row.
 */
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
int block_query(std::set<int> &dblocks,size_t*begins, size_t*ends,size_t *shape,int *bound,int dims_size){

    int head[dims_size];  
    size_t newshape[dims_size];
    size_t newdshape[dims_size];
    size_t count[dims_size];
    size_t countdshape[dims_size];
    size_t idx[dims_size];
    bool headfull[dims_size];
    bool tailfull[dims_size];
    int len[dims_size];
    int i,j;
    get_new_shape(newshape,bound,shape,dims_size);
    get_dshape(newdshape,newshape,dims_size);

    for(i=0;i<dims_size;i++){
        len[i]=shape[i]/bound[i];
        head[i]=begins[i]/len[i];
        count[i]=ends[i]/len[i]-head[i]+1;
/*        if(begins[i]%len[i]==0){*/
/*            headfull[i]=true;*/
/*        }else{*/
/*            headfull[i]=false;*/
/*        }*/
/*        if(ends[i]%bound[i]==len[i]-1||ends[i]==shape[i]-1){*/
/*            tailfull[i]=true;*/
/*        }else{*/
/*            tailfull[i]=false;*/
/*        }*/
    }
    int all_size=1;
    for(i=0;i<dims_size;i++){
        all_size*=count[i];
    }
/*    printf("dquery size %d\n",all_size);*/
    get_dshape(countdshape,count,dims_size);
    int pos;
    for(i=0;i<all_size;i++){
       get_idx(idx,i,countdshape,dims_size);
       for(j=0;j<dims_size;j++){
           idx[j]+=head[j];
       }
       pos=get_index(idx,newdshape,dims_size);
       dblocks.insert(pos);
/*       printf("dquery pos %d\n",pos);*/
    }

    return 0;
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

/*
 * binary search for left bound using in-memory array
*/
size_t lsearch(const double* data,size_t len,double val,bool equal){
/*    printf("lsearch\n");*/
    int lp=0;
    int rp=len-1;
    int mid;
    while(rp>lp){
        mid=lp+((rp-lp)>>1);
/*        printf("mid %d\n",mid);*/
        if(data[mid]>val)
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
    double tmp=data[rp];
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
        if(data[i]<tmp){
/*            printf("move %lf %lf\n",data[i].val,val);*/
            if(equal&&data[i]==val){
                tmp=data[i];
                while(i>=0){
                    if(data[i]<tmp){
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

/*
 * binary search for right bound using in-memory array
*/
size_t rsearch(const double* data,size_t len,double val,bool equal){
/*    printf("rsearch\n");*/
    int lp=0;
    int rp=len-1;
    int mid;
    while(rp>lp){
        mid=lp+((rp-lp)>>1);
        if(data[mid]>val)
            rp=mid;
        else
            lp=mid+1;
    }
    if(rp==len-1)
        return len-1;
    int res=-1;
    double tmp=data[rp];
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
        if(data[i]<tmp){
            if(data[i]==val){
               if(equal){
                    return i;
               }else{
                    tmp=data[i];
                    while(i>=0){
                        if(data[i]<tmp){
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

/*
 * binary search using in-memory array
*/
int binary_search(const double* data,size_t len,double min,double max,bool min_equal,bool max_equal,result * res){
   if( min>max||(min==max)&&(min_equal!=true||max_equal!=true)){
      return -1; 
   }
   struct timeval tbegin,tend;
   gettimeofday(&tbegin,NULL);
/*   printf("min %lf max %lf len %d\n",min,max,len);*/
   res->begin=lsearch(data,len,min,min_equal);
   res->end=rsearch(data,len,max,max_equal);
   gettimeofday(&tend,NULL);
   if(res->begin!=-1&&res->end!=-1){

/*       printf("hit number:%ld\n",res->end-res->begin+1);*/
/*       printf("begin %lf %lf end %lf %lf\n",data[res->begin-1],data[res->begin],data[res->end],data[res->end+1]);*/
/*       printf("binary_search time:%f\n",tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000);*/
       return 0;
   }
   return -1;
}

/*
 * read a value from a file using fseek
*/
inline void read_from_file_with_offset(FILE * fp,size_t offset,size_t size,size_t pos,double *data){
        fseek(fp,offset+size*pos,SEEK_SET);
        fread(data,size,1,fp);
/*        printf("%f %d\n",data->val,data->repeat);*/
}

/*
 * binary search for right bound using fseek
*/
size_t frsearch_with_offset(FILE * fp,size_t offset,size_t size,size_t len,double val,bool equal){
/*    printf("rsearch\n");*/
    int lp=0;
    int rp=len-1;
    int mid;
    double data;
    while(rp>lp){
        mid=lp+((rp-lp)>>1);
        read_from_file_with_offset(fp,offset,size,mid,&data);
        if(data>val)
            rp=mid;
        else
            lp=mid+1;
    }
    if(rp==len-1)
        return len-1;
    int res=-1;
    
    read_from_file_with_offset(fp,offset,size,rp,&data);
    double tmp=data;
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
        read_from_file_with_offset(fp,offset,size,i,&data);
        if(data<tmp){
            if(data==val){
               if(equal){
                    return i;
               }else{
                    tmp=data;
                    while(i>=0){
                        read_from_file_with_offset(fp,offset,size,i,&data);
                        if(data<tmp){
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

/*
 * binary search for left bound using fseek
*/
size_t flsearch_with_offset(FILE * fp,size_t offset,size_t size,size_t len,double val,bool equal){
/*    printf("lsearch\n");*/
    int lp=0;
    int rp=len-1;
    int mid;
    double data;
    while(rp>lp){
        mid=lp+((rp-lp)>>1);
        read_from_file_with_offset(fp,offset,size,mid,&data);
        if(data>val)
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
    read_from_file_with_offset(fp,offset,size,rp,&data);
    double tmp=data;
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
        read_from_file_with_offset(fp,offset,size,i,&data);
        if(data<tmp){
/*            printf("move %lf %lf\n",data[i].val,val);*/
            if(equal&&data==val){
                tmp=data;
                while(i>=0){
                    read_from_file_with_offset(fp,offset,size,i,&data);
                    if(data<tmp){
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


/*
 * binary search using fseek
 */
int fbsearch(FILE * fp,size_t offset,size_t size,size_t len,double min,double max,bool min_equal,bool max_equal,result *res){
   printf("fbsearch\n");
   if( min>max||(min==max)&&(min_equal!=true||max_equal!=true)){
      return -1; 
   }
   struct timeval tbegin,tend;
   gettimeofday(&tbegin,NULL);
   res->begin=flsearch_with_offset(fp,offset,size,len,min,min_equal);
   res->end=frsearch_with_offset(fp,offset,size,len,max,max_equal);
   gettimeofday(&tend,NULL);
   if(res->begin!=-1&&res->end!=-1){
/*       double data; */
/*       read_from_file_with_offset(fp,offset,sizeof(double),cres->begin,&data);*/
/*       res->begin=data.idx;*/
/*       read_from_file_with_offset(fp,offset,sizeof(double),cres->end,&data);*/
/*       res->end=data.idx+data.repeat-1;*/
/*       printf("hit number:%ld\n",res->end-res->begin+1);*/
/*       printf("begin %lf %lf end %lf %lf\n",data[res->begin-1].val,data[res->begin].val,data[res->end].val,data[res->end+1].val);*/
/*       printf("bsearch time:%f\n",tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000);*/
       return 0;
   }
   return -1;
}
int init_conditions(cond *conds,int dims_size){
    int i;
    conds->size=dims_size;
    conds->min=(double *)calloc(dims_size,sizeof(double));
    conds->minequal=(bool *)calloc(dims_size,sizeof(bool));
    conds->max=(double *)calloc(dims_size,sizeof(double));
    conds->maxequal=(bool *)calloc(dims_size,sizeof(bool));
    conds->valid=(bool *)calloc(dims_size,sizeof(bool));
    for(i=0;i<dims_size;i++){
        conds->valid[i]=false;
    }
}
int parse_condition(char *s,double *min,bool *min_equal,double *max,bool *max_equal){
    char lb,rb;
    sscanf(s,"%c%lf,%lf%c",&lb,min,max,&rb);
    if(lb=='('){
        *min_equal=false;
    }
    if(rb==')'){
        *max_equal=false;
    }
    printf("%s min %lf max %lf\n",s, *min,*max);
    return 0;
}
int set_begin_end(size_t  *begin, size_t *end,DIMS &dims,cond &conds){
    int i;
    result res;
    for(i=0;i<dims.dims_size;i++){
        if(conds.valid[i]==true){
            if(0<=binary_search((double*)(dims.dimvals[i]),dims.shape[i],conds.min[i],conds.max[i],conds.minequal[i],conds.maxequal[i],&res)){
               begin[i]=res.begin;
               end[i]=res.end;
               printf("set_begin_end begin %d end %d\n",begin[i],end[i]);
            }else{
                return -1;
            }
        }else{
            begin[i]=0;
            end[i]=dims.shape[i]-1;
        }
    }
    return 0;
}

int main(int argc,char ** argv){
    struct timeval tbegin, tend;
    struct timeval read_tbegin,read_tend;
    gettimeofday(&tbegin,NULL);
/*    int X_LIMIT;*/
/*    sscanf(argv[1],"%d",&X_LIMIT);*/
    bool need_scan=false;
    double min=DBL_MIN;
    double max=DBL_MAX;
    bool min_equal=true;
    bool max_equal=true;
/*    char lb,rb;*/
/*    printf("%s\n",argv[2]);*/
/*    sscanf(argv[2],"%c%lf,%lf%c",&lb,&min,&max,&rb);*/
/*    if(lb=='('){*/
/*        min_equal=false;*/
/*    }*/
/*    if(rb==')'){*/
/*        max_equal=false;*/
/*    }*/

/*    node* data=(node*)calloc(sizeof(node),X_LIMIT*Y*Z);*/

    FILE *fp=fopen(argv[1],"r");
    char ifilename[128]={0};
    sprintf(ifilename,"%s_bidx",argv[1]);
    FILE *ifp=fopen(ifilename,"r");
    sprintf(ifilename,"%s_meta",argv[1]);
    FILE *mfp=fopen(ifilename,"r");
    sprintf(ifilename,"%s_binfo",argv[1]);
    FILE *bfp=fopen(ifilename,"r");
   
    /* parse the bmeta file start!*/ 
    size_t i,j;
    char line[512]={0};
    int dims_size;
    fgets(line,sizeof(line),mfp);
    sscanf(line,"Dimension size=%d",&dims_size);
    char varname[128]={0};
    char vtypename[128]={0};
    char tempname[128]={0};
    char **dnames=(char **)calloc(dims_size,sizeof(char *));
    size_t *shape=(size_t *)calloc(dims_size,sizeof(size_t *));
    int *bound=(int *)calloc(dims_size,sizeof(int *));
    TYPE *types=(TYPE *)calloc(dims_size,sizeof(TYPE));
    TYPE var_type;
    fgets(line,sizeof(line),mfp);
    for(i=0;i<dims_size;i++){
        memset(line,0,sizeof(line));
        fgets(line,sizeof(line),mfp);
        dnames[i]=(char *)calloc(128,sizeof(char *));
        sscanf(line,"%s\t%s\t%d\t%d",dnames[i],tempname,&shape[i],&bound[i]);
        types[i]=get_type(tempname);
        memset(tempname,0,128);
    }
    fgets(line,sizeof(line),mfp);
    memset(line,0,sizeof(line));
    fgets(line,sizeof(line),mfp);
    sscanf(line,"%s\t%s",varname,vtypename);
    var_type=get_type(vtypename);
    memset(line,0,sizeof(line));
    fgets(line,sizeof(line),mfp);
    sscanf(line,"Block arrangement=%s",tempname);
    BLOCK_MODE bmode; 
    if(!strcmp(tempname,"linear")){
        bmode=LINEAR;
    }else if(!strcmp(tempname,"hcurve")){
        bmode=HCURVE;
    }
    /* parse the bmeta file end!*/ 

    /*parse the input arguments start!*/
    cond conds;
    bool has_dim_condition=false;
    bool has_var_condition=false;
    init_conditions(&conds,dims_size);
    for(i=2;i<argc;i=i+3){
        if(strcmp(argv[i],"-d")==0){
            for(j=0;j<dims_size;j++){
                if(0==strcmp(dnames[j],argv[i+1])){
                    parse_condition(argv[i+2],&(conds.min[j]),&(conds.minequal[j]),&(conds.max[j]),&(conds.maxequal[j]));
                    conds.valid[j]=true;
                    has_dim_condition=true;
                    break;
                }
            }
        }
        if(strcmp(argv[i],"-v")==0){
            if(strcmp(argv[i+1],varname)!=0){
                printf("variable %s doesnot exists in file %s\n",argv[i+1],argv[1]);
            }else{
                parse_condition(argv[i+2],&min,&min_equal,&max,&max_equal);
                has_var_condition=true;
            }
        }
    }
    /*parse the input arguments start!*/

/*   parse_condition(argv[2],&min,&min_equal,&max,&max_equal); */
    size_t *newshape = (size_t *)calloc(dims_size,sizeof(size_t));
    size_t *newdshape = (size_t *)calloc(dims_size,sizeof(size_t));
    size_t *dshape = (size_t *)calloc(dims_size,sizeof(size_t));
    get_new_shape(newshape,bound,shape,dims_size);
    get_dshape(newdshape,newshape,dims_size);
    get_dshape(dshape,shape,dims_size);
    size_t block_num=1;
    size_t all_size=1;
    for(i=0;i<dims_size;i++){
        block_num*=newshape[i];
        all_size*=shape[i];
    }
    block_info *binfo=(block_info*)calloc(block_num,sizeof(block_info));
    fread(binfo,sizeof(block_info),block_num,bfp);
/*    for(i=0;i<8;i++){*/
/*        vns[i].min=i;*/
/*        vns[i].max=i+1;*/
/*        vns[i].val=i;*/
/*    }*/
/*    init_rnodes(rnodes,0,0,512,0,vns,8,max_level);*/
    std::set<int> *vset=NULL;
    std::set<int> *dset=NULL;
    std::set<int> *fset=NULL;
    if(has_var_condition){

        vnode vns[block_num];
    /*    vnode *vns=(vnode*)calloc(block_num,sizeof(vnode));*/
        double gmin=DBL_MAX,gmax=DBL_MIN;
        for(i=0;i<block_num;i++){
            if(binfo[i].min<gmin)
                gmin=binfo[i].min;
            if(binfo[i].max>gmax)
                gmax=binfo[i].max;
            vns[i].min=binfo[i].min;
            vns[i].max=binfo[i].max;
            vns[i].val=i;
    /*        printf("block_id %d min %lf max %lf\n",i,binfo[i].min,binfo[i].max);*/
        }  
        int max_level=10;
        int rnodes_size=get_tree_size(max_level);
    /*    printf("max_level %d rnodes_size %d\n",max_level,rnodes_size);*/
        rnode rnodes[rnodes_size];
        init_rnodes(rnodes,0,gmin,gmax,0,vns,block_num,max_level);

        vset=new std::set<int>();

        rquery(*vset,min,max,rnodes,0,0,max_level);
        printf("vset size %d\n",(*vset).size());
    }
    size_t *dbegins=(size_t *)calloc(dims_size,sizeof(size_t));
    size_t *dends=(size_t *)calloc(dims_size,sizeof(size_t));
    DIMS dims;
    FILE **fps;
    if(has_dim_condition||need_scan){
        fps=(FILE **)calloc(dims_size,sizeof(FILE*));
        for(i=0;i<dims_size;i++){
            fps[i]=fopen(dnames[i],"r");
        }
        init_dims(&dims,dims_size,shape,types,var_type,fps);
        for(i=0;i<dims_size;i++){
            fclose(fps[i]);
        }
    }
    if(has_dim_condition){
        set_begin_end(dbegins,dends,dims,conds);
        printf("dbegins %d %d %d\n",dbegins[0],dbegins[1],dbegins[2]);
        printf("dends %d %d %d\n",dends[0],dends[1],dends[2]);
        for(i=0;i<dims_size;i++){
            printf("%d %d\n",dbegins[i],dends[i]);
        }
        dset=new std::set<int>();
        block_query(*dset,dbegins,dends,shape,bound,dims_size);
        printf("dset size %d\n",(*dset).size());
    }
    if(vset==NULL&&dset==NULL){
        if(strcmp(argv[2],"*")==0){
            /*to do scan all data*/
/*            set_begin_end(dbegins,dends,dims,conds);*/
        }else{
            printf("Please specify a condition or use %s %s * to scan all data\n",argv[0],argv[1]);
            return 0;
        }
    }else if(vset==NULL){
        fset=dset;
    }else if(dset==NULL){
        fset=vset;
    }else{
        fset=new std::set<int>();
        set_intersection(vset->begin(), vset->end(), dset->begin(), dset->end(), inserter(*fset, fset->begin()));
        printf("fset size %d\n",(*fset).size());
    }
    int block_size=get_max_block_size(bound,shape,dims_size);
    double *buff=(double *)calloc(block_size,sizeof(double)); 
    unsigned int *ibuff=(unsigned int *)calloc(block_size,sizeof(unsigned int)); 
    size_t idx[dims_size];
/*    size_t iidx[dims_size];*/
    size_t count[dims_size];
    size_t countdshape[dims_size];
    size_t offs[dims_size];
    result res;
    size_t len;
    size_t hits=0;
    int vsize=sizeof(double);// for value
    int isize=sizeof(unsigned int); // for index value
    int label=0;
    int pre=0;
    printf("fsize %d\n",fset->size());
    for(std::set<int>::iterator iter=fset->begin();iter!=fset->end();iter++){
        i=*iter;
        if(i!=block_num-1){
            len=binfo[i+1].boffset-binfo[i].boffset;
        }else{
            len=all_size-binfo[i].boffset;
        }
        if(vset!=NULL){
            if(dset==NULL){
                if(fbsearch(fp,binfo[i].boffset*sizeof(double),sizeof(double),len,min,max,min_equal,max_equal,&res)>=0){
/*                fread(buff,sizeof(double),res.end-res.begin+1,fp);*/
                    hits+=res.end-res.begin+1;
                }
            }else{
                if(fbsearch(fp,binfo[i].boffset*sizeof(double),sizeof(double),len,min,max,min_equal,max_equal,&res)>=0){
                    fseek(fp,(binfo[i].boffset+res.begin)*vsize,SEEK_SET);
                    fread(buff,vsize,res.end-res.begin+1,fp);
                    fseek(ifp,(binfo[i].boffset+res.begin)*isize,SEEK_SET);
                    fread(ibuff,isize,res.end-res.begin+1,fp);
                    size_t m;
                    get_begin_count_countdshape(offs,count,countdshape,i,shape,newdshape,bound,dims_size);
                    for(m=0;m<res.end-res.begin+1;m++){
                        get_idx_in_block(idx,ibuff[m+res.begin],countdshape,offs,dims_size);
                        if(check_dim_condition(idx,dbegins,dends,dims_size)){
                            hits++;
                        }
                    }

                }
            
            }
        }else{ //only with dimensional conditions
            size_t len;
            if(i!=block_num-1){
                len=binfo[i+1].boffset-binfo[i].boffset;
            }else{
                len=all_size-binfo[i].boffset;
            }
            if(label!=0){
                if(i!=pre+1){
                    fseek(fp,binfo[i].boffset*vsize,SEEK_SET);
                    fread(buff,vsize,len,fp);
                    fseek(ifp,binfo[i].boffset*isize,SEEK_SET);
                    fread(ibuff,isize,len,ifp);
                }else{
                    fread(buff,vsize,len,fp);
                    fread(ibuff,isize,len,ifp);
                }
                pre=i;
            }else{
                fseek(fp,binfo[i].boffset*vsize,SEEK_SET);
                fread(buff,vsize,len,fp);
                fseek(ifp,binfo[i].boffset*isize,SEEK_SET);
                fread(ibuff,isize,len,ifp);
/*                printf("binfo offset %d\n",binfo[i].boffset);*/
/*                printf("test %d %lf\n",ibuff[0],buff[0]);*/
                pre=i; 
            }
            get_begin_count_countdshape(offs,count,countdshape,i,shape,newdshape,bound,dims_size);
        
            bool contained=true;
            printf("len %d \n",len);
            for(j=0;j<dims_size;j++){
                if(!(dbegins[j]<=offs[j]&&dends[j]>=(offs[j]+count[j]))){
                    contained=false;
                    break;
                }
            }
            if(contained){
                hits+=count[0]*countdshape[dims_size-1];
                printf("contained %d\n",count[0]*countdshape[dims_size-1]);
            }else{
                for(j=0;j<len;j++){
                    get_idx_in_block(idx,ibuff[j],countdshape,offs,dims_size);
                    if(check_dim_condition(idx,dbegins,dends,dims_size)){
                        hits++;
                    }
                }
            }
        }
        label++;

    }
        int phits=1;
        for(i=0;i<dims_size;i++){
            phits=phits*(dends[i]-dbegins[i]+1);
        }
        printf("hits %d should be %d\n",hits,phits);
    

/*    result res;*/
/*    size_t hits=0;*/
/*    for(i=0;i<block_num;i++){*/
/*        if(min>binfo[i].max||max<binfo[i].min)*/
/*            continue;*/
/*        if((binfo[i].max==min&&!max_equal)||(binfo[i].min==max&&!min_equal))*/
/*            continue;*/
/*        size_t len;*/
/*        if(i!=block_num-1){*/
/*            len=binfo[i+1].boffset-binfo[i].boffset;*/
/*        }else{*/
/*            len=all_size-binfo[i].boffset;*/
/*        }*/
/*|+        fseek(bfp,binfo[i].boffset*sizeof(block_info),SEEK_SET);+|*/
/*|+        fread(buff,sizeof(double),len,fp);+|*/
/*|+        printf("len=%d\n",len);+|*/
/*|+        if(binary_search(buff,len, min,max,min_equal,max_equal,&res)>=0){+|*/
/*|+            hits+=res.end-res.begin+1;+|*/
/*|+        }+|*/
/*        if(fbsearch(fp,binfo[i].boffset*sizeof(double),sizeof(double),len,min,max,min_equal,max_equal,&res)>=0){*/
/*            hits+=res.end-res.begin+1;*/
/*        }*/
/*    }*/
/*    printf("hits %d\n",hits);*/
    
    
/*    fweek(bfp,0,SEEK_SET);*/

/*    fseek(bfp,0,SEEK_END); */
/*    int bfsize= ftell(bfp);*/
/*    block_info *binfo=(block_info*)calloc(bfsize/sizeof(block_info),sizeof(block_info));*/
     

/*    memset(data,0,sizeof(node)*X_LIMIT*Y*Z);*/
/*    sscanf(argv[2],"%lf,%lf",&min,&max);*/
/*    printf("%f %f\n",min,max);*/
/*    fread(data,sizeof(node),X_LIMIT*Y*Z,fp);*/
/*    fclose(fp);*/
/*    result res,cres;*/
/*|+    bsearch(data,X_LIMIT*Y*Z,min,max,min_equal,max_equal,&res);+|*/
/*    fseek(fp,0,SEEK_END); */
/*    size_t fsize = ftell(fp);*/
/*    fbsearch(fp,sizeof(cnode),fsize/sizeof(cnode),min,max,min_equal,max_equal,&cres,&res);*/
/*|+  free(data);+|*/
/*    DIMS dims;*/
/*    int shape[3]={21900,94,192};*/
/*    TYPE types[3]={DOUBLE,DOUBLE,DOUBLE};*/
/*    TYPE var_type= DOUBLE;*/
/*    FILE **fps=(FILE **)calloc(dims_size,sizeof(FILE));*/
/*    for(i=0;i<dims_size;i++){*/
/*        fps[i]=fopen(dnames[i],"r");*/
/*    }*/
/*    fps[0]=fopen("time","r");*/
/*    fps[1]=fopen("LAT","r");*/
/*    fps[2]=fopen("LON","r");*/
/*    init_dims(&dims,dims_size,shape,types,var_type,fps);*/
/*    int cols[3]={0,1,2};*/
/*    int cols_size=3;*/
/*    FILE *ofp;*/
    gettimeofday(&read_tbegin,NULL);
/*    if(argc>=4){*/
/*        ofp=fopen(argv[3],"w");*/
/*        scan(res.begin,res.end,fp,&dims,NULL,0,ofp,TEXT);*/
/*        scan(res.begin,res.end,fp,&dims,cols,cols_size,ofp,TEXT);*/
/*        scan(&cres,&res,fp,ifp,&dims,cols,cols_size,ofp,BINARY);*/
/*        scan(&cres,&res,fp,ifp,&dims,cols,cols_size,ofp,TEXT);*/
/*    }*/
    gettimeofday(&read_tend,NULL);
/*    if(argc>=5)*/
/*        fclose(ofp);*/
    destory_dims(&dims);
    fclose(fp);
    fclose(ifp);
    fclose(mfp);
/*    ofp=fopen(argv[4],"r");*/
/*    print_res(ofp);*/
/*    fclose(ofp);*/
    gettimeofday(&tend,NULL);
    printf("all time is %fs and scan time is %fs\n",tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000,read_tend.tv_sec-read_tbegin.tv_sec+1.0*(read_tend.tv_usec-read_tbegin.tv_usec)/1000000);
    return 0;
}
