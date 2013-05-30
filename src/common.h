#ifndef _COMMON_H
#define _COMMON_H
#define BLOCKSIZE 33554432 //32*1024*1024
typedef struct node_t{
   size_t idx;
   double val;
}node;

typedef struct cnode_t{
   size_t idx;
   double val;
   unsigned char repeat;
}cnode;

typedef struct result_t{
    size_t begin;
    size_t end;
}result;

typedef enum {BYTE, SHORT, INT, LONG,FLOAT,DOUBLE} TYPE;
typedef enum {TEXT, BINARY} MODE;

typedef struct dims_t{
   TYPE* types;
   TYPE var_type;
   int dim_size;
   int *shape;
   void **dimvals;
}DIMS;

void read_dim(FILE *fp,void **vals,TYPE type,size_t dim_size){
    switch(type){
        case BYTE:
            *vals=calloc(dim_size,sizeof(char));
            fread(*vals,sizeof(char),dim_size,fp); 
            break;
        case SHORT:
            *vals=calloc(dim_size,sizeof(short));
            fread(*vals,sizeof(short),dim_size,fp); 
            break;
        case INT:
            *vals=calloc(dim_size,sizeof(int));
            fread(*vals,sizeof(int),dim_size,fp); 
            break;
        case LONG:
            *vals=calloc(dim_size,sizeof(long));
            fread(*vals,sizeof(long),dim_size,fp); 
            break;
        case FLOAT:
            *vals=calloc(dim_size,sizeof(float));
            fread(*vals,sizeof(float),dim_size,fp); 
            break;
        case DOUBLE:
            *vals=calloc(dim_size,sizeof(double));
            fread(*vals,sizeof(double),dim_size,fp);
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


void init_dims(DIMS* dims,int dims_size,int *shape,TYPE * types, TYPE var_type,FILE ** fps){ 
   dims->dim_size=dims_size;
   dims->shape=(int *)calloc(dims_size,sizeof(int));
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
    for(i=0;i<dims->dim_size;i++){
        free(dims->dimvals[i]);
    }
    free(dims->types);
    free(dims->dimvals);
}

void get_dshape(int *dshape,int *shape,int size){
    int i;
    int tmp=1;
    dshape[0]=1;
    for(i=1;i<size;i++){
      dshape[i]=tmp=tmp*shape[size-i];
    } 
}
inline void get_idx(int *idx,size_t pos,int *dshape,int size){
   int i;
   size_t tmp=pos;
   for(i=0;i<size;i++){
       idx[i]=tmp/dshape[size-1-i];
       tmp=tmp-idx[i]*dshape[size-1-i];
/*     printf("%d:%d ",dshape[size-1-i],idx[i]);*/
   }
/*   printf("\n");*/
}
inline size_t get_index(int *idx,int *dshape,int size){
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
#endif
