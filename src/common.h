#ifndef _COMMON_H
#define _COMMON_H
#define BLOCKSIZE 33554432 //32*1024*1024
#include <netcdf.h>
typedef struct node_t{
   size_t idx;
   double val;
}node;
typedef struct bnode_t{
   unsigned int idx;
   double val;
}bnode;

typedef struct cnode_t{
   size_t idx;
   double val;
   unsigned char repeat;
}cnode;

typedef struct result_t{
    size_t begin;
    size_t end;
}result;

typedef struct block_info_t{
    double min,max;
    size_t boffset;
}block_info;

typedef enum {BYTE, SHORT, INT, LONG,FLOAT,DOUBLE,UNKNOWN} TYPE;
typedef enum {TEXT, BINARY} MODE;
typedef enum {LINEAR, HCURVE} LAYOUT;

typedef struct dims_t{
   TYPE* types;
   TYPE var_type;
   int dims_size;
   size_t *shape;
   void **dimvals;
}DIMS;

void read_dim(FILE *fp,void **vals,TYPE type,size_t dims_size){
    switch(type){
        case BYTE:
            *vals=calloc(dims_size,sizeof(char));
            fread(*vals,sizeof(char),dims_size,fp); 
            break;
        case SHORT:
            *vals=calloc(dims_size,sizeof(short));
            fread(*vals,sizeof(short),dims_size,fp); 
            break;
        case INT:
            *vals=calloc(dims_size,sizeof(int));
            fread(*vals,sizeof(int),dims_size,fp); 
            break;
        case LONG:
            *vals=calloc(dims_size,sizeof(long));
            fread(*vals,sizeof(long),dims_size,fp); 
            break;
        case FLOAT:
            *vals=calloc(dims_size,sizeof(float));
            fread(*vals,sizeof(float),dims_size,fp); 
            break;
        case DOUBLE:
            *vals=calloc(dims_size,sizeof(double));
            fread(*vals,sizeof(double),dims_size,fp);
            break;
        defaut:
            printf("unknown type\n");
    }
}
void print_dim(void *dimvals,TYPE type,int shape){
    int i;
    for(i=0;i<shape;i++){
        printf("%lf,",((double *)dimvals)[i]);
    }
    printf("\n");
    
}


void init_dims(DIMS* dims,int dims_size,size_t *shape,TYPE * types, TYPE var_type,FILE ** fps){ 
   dims->dims_size=dims_size;
   dims->shape=(size_t *)calloc(dims_size,sizeof(size_t));
   dims->dimvals=(void **)calloc(dims_size,sizeof(void *));
   dims->types=(TYPE *)calloc(dims_size,sizeof(TYPE));
   dims->var_type=var_type;
   int i;
   for(i=0;i<dims_size;i++){
       dims->shape[i]=shape[i];
       dims->types[i]=types[i];
       if(fps!=NULL)
           read_dim(fps[i],&(dims->dimvals[i]),types[i],shape[i]);
/*       print_dim(dims->dimvals[i],types[i],shape[i]);*/
   }
}
void destory_dims(DIMS *dims){
    int i=0;
    free(dims->shape);
    for(i=0;i<dims->dims_size;i++){
        free(dims->dimvals[i]);
    }
    free(dims->types);
    free(dims->dimvals);
}

inline void get_dshape(size_t *dshape,size_t *shape,int size){
    int i;
    int tmp=1;
    dshape[0]=1;
    for(i=1;i<size;i++){
      dshape[i]=tmp=tmp*shape[size-i];
    } 
}
inline void get_idx_in_block(size_t *idx,size_t pos,size_t *dshape,size_t *begin,int size){
   int i;
   size_t tmp=pos;
   for(i=0;i<size;i++){
       idx[i]=tmp/dshape[size-1-i];
       tmp=tmp-idx[i]*dshape[size-1-i];
       idx[i]+=begin[i];
/*     printf("%d:%d ",dshape[size-1-i],idx[i]);*/
   }
/*   printf("\n");*/
}
inline void get_idx(size_t *idx,size_t pos,size_t *dshape,int size){
   int i;
   size_t tmp=pos;
   for(i=0;i<size;i++){
       idx[i]=tmp/dshape[size-1-i];
       tmp=tmp-idx[i]*dshape[size-1-i];
/*     printf("%d:%d ",dshape[size-1-i],idx[i]);*/
   }
/*   printf("\n");*/
}
inline size_t get_index(size_t *idx,size_t *dshape,int size){
   int i;
//   size_t tmp=pos;
   size_t tmp=idx[size-1];
   for(i=1;i<size;i++){
        tmp+=idx[size-1-i]*dshape[i];
//       idx[i]=tmp/dshape[size-1-i];
//       tmp=tmp-idx[i]*dshape[size-1-i];
/*       printf("%d:%d ",dshape[size-1-i],idx[i]);*/
   }
   return tmp;
/*   printf("\n");*/
}
int get_type_size(TYPE type){
    switch(type){
        case BYTE:
            return sizeof(char);
        case SHORT:
            return sizeof(short);
        case INT:
            return sizeof(int);
        case LONG:
            return sizeof(long);
            break;
        case FLOAT:
            return sizeof(float);
        case DOUBLE:
            return sizeof(double);
        defaut:
            printf("unknown type\n");
    }
    return -1;
}
int get_nctype_size(nc_type  type){
    switch(type){
        case NC_BYTE:
            return sizeof(char);
        case NC_SHORT:
            return sizeof(short);
        case NC_INT:
            return sizeof(int);
/*        case NC_INT64:*/
/*            return sizeof(long);*/
        case NC_FLOAT:
            return sizeof(float);
        case NC_DOUBLE:
            return sizeof(double);
        default:
            printf("get_nctype_size() unknown type!\n");
    }
    return -1;
}
size_t get_block_size(size_t *bound,size_t *shape,int size){
    size_t len=1;
    int i;
    for(i=0;i<size;i++){
        len*=shape[i]/bound[i];
    }
    return len;
}
size_t get_max_block_size(size_t *bound,size_t *shape,int size){
    size_t len=1;
    int i;
    for(i=0;i<size;i++){
        len*=shape[i]-(shape[i]/bound[i])*(bound[i]-1);
    }
//    printf("max_block_size %d\n",len);
    return len;
}
void get_new_shape(size_t *newshape,size_t *bound,size_t *shape,int size){
    int i;
    for(i=0;i<size;i++){
        if(shape[i]>=bound[i]){
            newshape[i]=bound[i];
        }else{
            bound[i]=shape[i];
            newshape[i]=shape[i];
        }
    }
    
}
inline void get_start_count(size_t *start,size_t *count,size_t *newidx,size_t *newshape,size_t *bound,size_t *shape,int size){
   int i,len;
   for(i=0;i<size;i++){
       len=shape[i]/bound[i];
       start[i]=newidx[i]*len;
       if(newidx[i]!=newshape[i]-1){
           count[i]=len;
       }else{
           count[i]=shape[i]-newidx[i]*len;
//           printf("shape[i] %d newshape[i] %d newidx[i] %d len %d\n",shape[i],newshape[i],newidx[i],len);
       }
//       printf("len %d count[i] %d\n",len,count[i]);
   }
}

int nc_get_vara(int ncid, int varid, size_t *start, size_t *count, void* buff,nc_type type){
    int retval=-1; 
    switch(type){
        case NC_BYTE:
            retval=nc_get_vara_uchar(ncid,varid,start,count,(unsigned char *)buff);
            break;
        case NC_SHORT:
            retval=nc_get_vara_short(ncid,varid,start,count,(short  *)buff);
            break;
        case NC_INT:
            retval=nc_get_vara_int(ncid,varid,start,count,(int  *)buff);
            break;
/*        case NC_INT64:*/
/*            retval=nc_get_vara_long(ncid,varid,start,count,(long  *)buff);*/
/*            break;*/
        case NC_FLOAT:
            retval=nc_get_vara_float(ncid,varid,start,count,(float  *)buff);
            break;
        case NC_DOUBLE:
            retval=nc_get_vara_double(ncid,varid,start,count,(double  *)buff);
            break;
        default:
            printf("nc_get_vara() unknown type!\n");
    }
    return retval;

    }
int nc_get_var(int ncid,int varid,void *buff,nc_type type){
    int retval=-1; 
    switch(type){
        case NC_BYTE:
            retval=nc_get_var_uchar(ncid,varid,(unsigned char *)buff);
            break;
        case NC_SHORT:
            retval=nc_get_var_short(ncid,varid,(short  *)buff);
            break;
        case NC_INT:
            retval=nc_get_var_int(ncid,varid,(int  *)buff);
            break;
/*        case NC_INT64:*/
/*            retval=nc_get_var_long(ncid,varid,(long  *)buff);*/
/*            break;*/
        case NC_FLOAT:
            retval=nc_get_var_float(ncid,varid,(float  *)buff);
            break;
        case NC_DOUBLE:
            retval=nc_get_var_double(ncid,varid,(double  *)buff);
            break;
        default:
            printf("nc_get_var() unknown type!\n");
    }
    return retval;
}

inline void to_batch_buff(void * buff,size_t *offset,size_t maxsize,void *src, size_t len,FILE *ofp){
    if(len>maxsize){
        printf("batch_buff size is %d, but the data size is %d!\n",maxsize,len);
        return;
    }
    if(*offset+len>maxsize){
        fwrite(buff,*offset,1,ofp);
        memcpy((char *)buff,src,len);
        *offset=len;
    }else{
        memcpy((char *)buff+*offset,src,len);
        *offset+=len;
    }
}
void flush_batch_buff(char *buff, size_t *offset,size_t maxsize,FILE *ofp){
    if(*offset>0){
        fwrite(buff,*offset,1,ofp);
    }
}

/*
 * binary search for left bound using in-memory array
*/
inline size_t lsearch(const double* data,size_t len,double val,bool equal){
/*    printf("lsearch\n");*/
    int lp=0;
    int rp=len-1;
    int mid;
    while(rp>lp){
        mid=rp-((rp-lp)>>1);
/*        printf("mid %d\n",mid);*/
        if(data[mid]>=val)
            rp=mid-1;
        else
            lp=mid;
/*        printf("lsearch %d %d %d\n",lp,mid ,rp);*/
    }
/*    printf("rp %d val %lf\n",rp,data[rp].val);*/
    double tmp=data[lp];
    if(lp==0){
        if(equal&&tmp==val){
            return 0;
        }
        if(val<tmp)
            return 0;
    }
    int i;
/*    printf("%f\n",data[lp]);*/
    for(i=lp+1;i<len;i++){
        if(data[i]>tmp){
            if(data[i]==val){
               if(equal){
                    return i;
               }else{
                    tmp=data[i];
                    while(i<len){
                        if(data[i]>tmp){
                            return i;
                        }
                        i++;
                    }
                    return -1;
               }
            }else{
                return i;
            }
        }
    }
    return -1;
}

/*
 * binary search for right bound using in-memory array
*/
inline size_t rsearch(const double* data,size_t len,double val,bool equal){
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
    double tmp=data[rp];
    if(rp==len-1){
        if(equal&&tmp==val){
            return rp;
        }
        if(val>tmp){
            return rp;
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
                    return -1;
               }
            }else{
                return i;
            }
        }
    }
    return -1;
}
/*
 * reverse binary search for left bound using in-memory array
*/
inline size_t reverse_lsearch(const double* data,size_t len,double val,bool equal){
/*    printf("lsearch\n");*/
    int lp=0;
    int rp=len-1;
    int mid;
    while(rp>lp){
        mid=rp-((rp-lp)>>1);
/*        printf("mid %d\n",mid);*/
        if(data[mid]<=val)
            rp=mid-1;
        else
            lp=mid;
/*        printf("lsearch %d %d %d\n",lp,mid ,rp);*/
    }
/*    printf("lp %d val %lf\n",rp,data[lp]);*/
    double tmp=data[lp];
    if(lp==0){
        if(equal&&tmp==val){
            return 0;
        }
        if(val>tmp)
            return 0;
    }
    int i;
/*    printf("%f\n",data[lp]);*/
    for(i=lp+1;i<len;i++){
        if(data[i]<tmp){
            if(data[i]==val){
               if(equal){
                    return i;
               }else{
                    tmp=data[i];
                    while(i<len){
                        if(data[i]<tmp){
                            return i;
                        }
                        i++;
                    }
                    return -1;
               }
            }else{
                return i;
            }
        }
    }
    return -1;
}
/*
 * reverse binary search for right bound using in-memory array
*/
inline size_t reverse_rsearch(const double* data,size_t len,double val,bool equal){
/*    printf("rsearch\n");*/
    int lp=0;
    int rp=len-1;
    int mid;
    while(rp>lp){
        mid=lp+((rp-lp)>>1);
        if(data[mid]<val)
            rp=mid;
        else
            lp=mid+1;
    }
    double tmp=data[rp];
    if(rp==len-1){
        if(equal&&tmp==val){
            return rp;
        }
        if(val<tmp){
            return rp;
        }
    }
    int i;
    for(i=rp-1;i>=0;i--){
        if(data[i]>tmp){
            if(data[i]==val){
               if(equal){
                    return i;
               }else{
                    tmp=data[i];
                    while(i>=0){
                        if(data[i]>tmp){
                            return i;
                        }
                        i--;
                    }
                    return -1;
               }
            }else{
                return i;
            }
        }
    }
    return -1;
}
/*
 * binary search using in-memory array
*/
inline int binary_search(const double* data,size_t len,double min,double max,bool min_equal,bool max_equal,result * res){
   if( min>max||(min==max)&&(min_equal!=true||max_equal!=true)){
      return -1; 
   }
   struct timeval tbegin,tend;
/*   printf("min %lf max %lf len %d\n",min,max,len);*/
   if(data[len-1]>=data[0]){
       res->begin=lsearch(data,len,min,min_equal);
       res->end=rsearch(data,len,max,max_equal);
   }else{
       res->begin=reverse_lsearch(data,len,max,max_equal);
       res->end=reverse_rsearch(data,len,min,min_equal);
   }
/*   printf("res %d %d\n",res->begin,res->end);*/
   if(res->begin!=-1&&res->end!=-1&&res->end>=res->begin){

/*       printf("hit number:%ld\n",res->end-res->begin+1);*/
/*       printf("begin %lf %lf end %lf %lf\n",data[res->begin-1],data[res->begin],data[res->end],data[res->end+1]);*/
       return 0;
   }
   return -1;
}
#endif
