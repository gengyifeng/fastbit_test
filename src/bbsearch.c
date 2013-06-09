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
size_t validate(size_t *idx,DIMS dims,cond c){
    int i;
    for(i=0;i<c.minsize;i++){
/*       dims.dimvals[idx[c.minmap[i]]];*/
        
    }
}

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
            size_t dshape[dims->dim_size];
            int idx[dims->dim_size];
            get_dshape(dshape,dims->shape,dims->dim_size);
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
                    get_idx(idx,idx_data[data[i].idx+k-res->begin],dshape,dims->dim_size); 
        /*            printf("after idx %d\n",check_index(idx,dims->shape,dims->dim_size));*/
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
            size_t dshape[dims->dim_size];
            int idx[dims->dim_size];
            get_dshape(dshape,dims->shape,dims->dim_size);
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
                get_idx(idx,idx_data[data[i].idx+k-res->begin],dshape,dims->dim_size); 
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

int main(int argc,char ** argv){
    struct timeval tbegin, tend;
    struct timeval read_tbegin,read_tend;
    gettimeofday(&tbegin,NULL);
    int X_LIMIT;
/*    sscanf(argv[1],"%d",&X_LIMIT);*/
    double min=DBL_MIN;
    double max=DBL_MAX;
    bool min_equal=true;
    bool max_equal=true;
    
    char lb,rb;
/*    printf("%s\n",argv[2]);*/
    sscanf(argv[2],"%c%lf,%lf%c",&lb,&min,&max,&rb);
    if(lb=='('){
        min_equal=false;
    }
    if(rb==')'){
        max_equal=false;
    }
    printf("min %lf max %lf\n",min,max);
    

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
    int i;
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

    size_t *newshape = (size_t *)calloc(dims_size,sizeof(size_t));
    size_t *newdshape = (size_t *)calloc(dims_size,sizeof(size_t));
    get_new_shape(newshape,bound,shape,dims_size);
    get_dshape(newdshape,newshape,dims_size);
    size_t block_num=1;
    size_t all_size=1;
    for(i=0;i<dims_size;i++){
        block_num*=newshape[i];
        all_size*=shape[i];
    }
    int block_size=get_max_block_size(bound,shape,dims_size);
/*    printf("block_size %d\n",block_size);*/
    double *buff=(double *)calloc(block_size,sizeof(double)); 
    block_info *binfo=(block_info*)calloc(block_num,sizeof(block_info));
    fread(binfo,sizeof(block_info),block_num,bfp);
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
        printf("block_id %d min %lf max %lf\n",i,binfo[i].min,binfo[i].max);
    }  
/*    int max_level=get_max_level(100*block_num);*/
    int max_level=10;
/*    max_level=3;*/
    int rnodes_size=get_tree_size(max_level);
/*    printf("max_level %d rnodes_size %d\n",max_level,rnodes_size);*/
    rnode rnodes[rnodes_size];
    init_rnodes(rnodes,0,gmin,gmax,0,vns,block_num,max_level);
/*    for(i=0;i<8;i++){*/
/*        vns[i].min=i;*/
/*        vns[i].max=i+1;*/
/*        vns[i].val=i;*/
/*    }*/
/*    init_rnodes(rnodes,0,0,512,0,vns,8,max_level);*/
    std::set<int> vset;
    rquery(vset,min,max,rnodes,0,0,max_level);
    printf("vset size %d\n",vset.size());
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
    DIMS dims;
/*    int shape[3]={21900,94,192};*/
/*    TYPE types[3]={DOUBLE,DOUBLE,DOUBLE};*/
/*    TYPE var_type= DOUBLE;*/
    FILE **fps=(FILE **)calloc(dims_size,sizeof(FILE));
    for(i=0;i<dims_size;i++){
        fps[i]=fopen(dnames[i],"r");
    }
/*    fps[0]=fopen("time","r");*/
/*    fps[1]=fopen("LAT","r");*/
/*    fps[2]=fopen("LON","r");*/
    init_dims(&dims,dims_size,shape,types,var_type,fps);
/*    int cols[3]={0,1,2};*/
/*    int cols_size=3;*/
    FILE *ofp;
    gettimeofday(&read_tbegin,NULL);
    if(argc>=4){
        ofp=fopen(argv[3],"w");
/*        scan(res.begin,res.end,fp,&dims,NULL,0,ofp,TEXT);*/
/*        scan(res.begin,res.end,fp,&dims,cols,cols_size,ofp,TEXT);*/
/*        scan(&cres,&res,fp,ifp,&dims,cols,cols_size,ofp,BINARY);*/
/*        scan(&cres,&res,fp,ifp,&dims,cols,cols_size,ofp,TEXT);*/
    }
    gettimeofday(&read_tend,NULL);
    if(argc>=5)
        fclose(ofp);
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
