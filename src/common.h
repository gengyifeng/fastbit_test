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
typedef enum {LINEAR, HCURVE} BLOCK_MODE;

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
int get_block_size(int *bound,size_t *shape,int size){
    int len=1;
    int i;
    for(i=0;i<size;i++){
        len*=shape[i]/bound[i];
    }
    return len;
}
int get_max_block_size(int *bound,size_t *shape,int size){
    int len=1;
    int i;
    for(i=0;i<size;i++){
        len*=shape[i]-(shape[i]/bound[i])*(bound[i]-1);
    }
//    printf("max_block_size %d\n",len);
    return len;
}
void get_new_shape(size_t *newshape,int *bound,size_t *shape,int size){
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
inline void get_start_count(size_t *start,size_t *count,size_t *newidx,size_t *newshape,int *bound,size_t *shape,int size){
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
#endif
