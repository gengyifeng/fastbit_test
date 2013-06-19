#include <stdio.h>
#include <stdlib.h>
#include <netcdf.h>
#include <math.h>
#include <string.h>
#include "common.h"
#define BAIL(e) do{ \
    printf("ERROR:: file: %s, line: %d, func: %s, code: %s.\n",__FILE__,__LINE__,__FUNCTION__, nc_strerror(e)); \
    return e;\
} while(0)

typedef enum {RANDOM, LOCALITY} GMODE;
GMODE gm;
inline void random(void *res,double min,double max,TYPE type){
   double f=1.0*rand()/RAND_MAX;
   switch(type){
        case(BYTE):
           *(unsigned char*)res=(unsigned char)(min+f*(max-min));
           break;
        case(SHORT):
           *(short*)res=(short)(min+f*(max-min));
           break;
        case(INT):
           *(int*)res=(int)(min+f*(max-min));
           break;
        case(LONG):
           *(long*)res=(long)(min+f*(max-min));
           break;
        case(FLOAT):
           *(float*)res=(float)(min+f*(max-min));
           break;
        case(DOUBLE):
           *(double*)res=(double)(min+f*(max-min));
           break;
        default:
           printf("unknown types in random()!\n");
   }
}
inline void getval(void *res,size_t val,TYPE type){
   switch(type){
        case(BYTE):
           *(unsigned char*)res=(unsigned char)(val);
           break;
        case(SHORT):
           *(short*)res=(short)(val);
           break;
        case(INT):
           *(int*)res=(int)(val);
           break;
        case(LONG):
           *(long*)res=(long)(val);
           break;
        case(FLOAT):
           *(float*)res=(float)(1.0f*val);
           break;
        case(DOUBLE):
           *(double*)res=(double)(1.0*val);
           break;
        default:
           printf("unknown types in getval()!\n");
   }
}
nc_type get_nctype(TYPE type){
   double f=1.0*rand()/RAND_MAX;
   switch(type){
        case(BYTE):
            return NC_BYTE;
        case(SHORT):
            return NC_SHORT;
        case(INT):
            return NC_INT;
        case(LONG):
            return NC_LONG;
        case(FLOAT):
            return NC_FLOAT;
        case(DOUBLE):
            return NC_DOUBLE;
        default:
           printf("unknown types in get_nctype()!\n");
           return -1;
/*           return NC_UNSPECIFIED;*/
   }
}
void nc_put_var(TYPE type,int ncid,int varid,const void *dp){
    switch(type){
        case(BYTE):
            nc_put_var_uchar(ncid,varid,(const unsigned char *)dp);
            break;
        case(SHORT):
            nc_put_var_short(ncid,varid,(const short *)dp);
            break;
        case(INT):
            nc_put_var_int(ncid,varid,(const int *)dp);
            break;
        case(LONG):
            nc_put_var_long(ncid,varid,(const long *)dp);
            break;
        case(FLOAT):
            nc_put_var_float(ncid,varid,(const float *)dp);
            break;
        case(DOUBLE):
            nc_put_var_double(ncid,varid,(const double *)dp);
            break;
        default:
            printf("unkown types nc_put_var()!\n");
    }
}
void nc_put_vara(TYPE type,int ncid,int varid,const size_t start[],const size_t count[],const void *dp){
    switch(type){
        case(BYTE):
            nc_put_vara_uchar(ncid,varid,start,count,(const unsigned char  *)dp);
            break;
        case(SHORT):
            nc_put_vara_short(ncid,varid,start,count,(const short *)dp);
            break;
        case(INT):
            nc_put_vara_int(ncid,varid,start,count,(const int *)dp);
            break;
        case(LONG):
            nc_put_vara_long(ncid,varid,start,count,(const long *)dp);
            break;
        case(FLOAT):
            nc_put_vara_float(ncid,varid,start,count,(const float *)dp);
            break;
        case(DOUBLE):
            nc_put_vara_double(ncid,varid,start,count,(const double *)dp);
            break;
        default:
            printf("unkown types in nc_put_vara()!\n");
    }
}
void init_dimvar(void *data,size_t len,TYPE type){
   size_t i;
   int size=get_type_size(type);
   for(i=0;i<len;i++){
/*       random(((char *)data+i*size),0,len*2,type);*/
       getval(((char *)data+i*size),i,type);
   }
}

inline void init_var_locality(void *data,size_t len,size_t offset,size_t *dshapes,int dims_size,double shift,size_t max_pos,TYPE type){
   size_t i,j;
   int size=get_type_size(type);
   size_t idx[dims_size];
   double min,max;
   size_t pos;
   for(i=0;i<len;i++){
       get_idx(idx,i,dshapes,dims_size);
       idx[0]+=offset;
       pos=0;
       for(j=0;j<dims_size;j++){
           pos+=idx[j]+1;
       }
       min=1.0*(pos-max_pos*shift)/max_pos;
       max=1.0*(pos+max_pos*shift)/max_pos;
/*       printf("%lf %lf %d\n",min,max,max_pos);*/
       if(min<0){
           min=0;
       }
       if(max>1){
           max=1;
       }
       random(((char *)data+i*size),min,max,type);
/*       getval(((char *)data+i*size),i+offset,type);*/
/*       *(double *)data=i;*/
   }
}
inline void init_var(void *data,size_t len,double min, double max,TYPE type){
   size_t i;
   int size=get_type_size(type);
   for(i=0;i<len;i++){
       random(((char *)data+i*size),min,max,type);
/*       getval(((char *)data+i*size),i,type);*/
/*       *(double *)data=i;*/
   }
}
/*void generator(const char* fname,int * shapes,int *types,int dim_size,TYPE var_type){*/
int generator(const char* fname,char *dimnames[],char *varname,size_t * shapes,int dim_size,TYPE *types,TYPE var_type){
    int res;    
    int ncid,vlid;
    int *dimids=(int *)calloc(dim_size,sizeof(int));
    int *dimvarids=(int *)calloc(dim_size,sizeof(int));
/*        dimids[3],dimvarids[3];*/
/*    int d1_id,d2_id,d3_id;*/
    double coverage=1.0;
/*    char *dim_names[]={"d1","d2","d3"};*/
    size_t total_size=1;
    size_t max_pos=0;
    int i; 
    for(i=0;i<dim_size;i++){
        total_size*=shapes[i];
        max_pos+=shapes[i];
    }
/*    double min=0;*/
/*    double max=coverage;*/
/*    TYPE types[3]={DOUBLE,DOUBLE,DOUBLE};*/
    if((res=nc_create(fname,NC_CLOBBER|NC_64BIT_OFFSET,&ncid)))
        BAIL(res);
/*    if((res=nc_def_dim(ncid,"d1",dims.shape[0],&dimids[0])))*/
/*        BAIL(res);*/
    for(i=0;i<dim_size;i++){
        if(i==0){
            if((res=nc_def_dim(ncid,dimnames[i],NC_UNLIMITED,&dimids[i])))
                BAIL(res);
        }else{
             if((res=nc_def_dim(ncid,dimnames[i],shapes[i],&dimids[i])))
                BAIL(res);
        }
        if((res=nc_def_var(ncid,dimnames[i],get_nctype(types[i]),1,&dimids[i],&dimvarids[i])))
            BAIL(res);
    }
/*    if((res=nc_def_var(ncid,"d1",get_nctype(types[0]),1,&dimids[0],&dimvarids[0])))*/
/*        BAIL(res);*/
/*    if((res=nc_def_var(ncid,"d2",get_nctype(types[1]),1,&dimids[1],&dimvarids[1])))*/
/*        BAIL(res);*/
/*    if((res=nc_def_var(ncid,"d3",get_nctype(types[2]),1,&dimids[2],&dimvarids[2])))*/
/*        BAIL(res);*/
    if((res=nc_def_var(ncid,varname,get_nctype(var_type),dim_size,dimids,&vlid)))
        BAIL(res);
    if((res=nc_enddef(ncid)))
        BAIL(res);
    
    void **dimvar=(void **)calloc(dim_size,sizeof(void *));
    void *buffer=calloc(total_size/shapes[0],get_type_size(var_type));
    void *udata=calloc(1,get_type_size(types[0]));
    for(i=1;i<dim_size;i++){
        dimvar[i]=calloc(shapes[i],get_type_size(types[i]));
        init_dimvar(dimvar[i],shapes[i],types[i]);
        nc_put_var(types[i],ncid,dimvarids[i],dimvar[i]);
        free(dimvar[i]); 
    }
    size_t *start=(size_t *)calloc(dim_size,sizeof(size_t));
    size_t *count=(size_t *)calloc(dim_size,sizeof(size_t));
    size_t *dshape=(size_t *)calloc(dim_size,sizeof(size_t));
    size_t ustart[1]={0};
    size_t ucount[1]={1};
    for(i=1;i<dim_size;i++){ 
        count[i]=shapes[i];
    }
    count[0]=1;
    get_dshape(dshape,shapes,dim_size);
    double shift=0.01;
    for(i=0;i<shapes[0];i++){
        start[0]=i;
        ustart[0]=i;
        getval(((char *)udata),i,types[0]);
        nc_put_vara(types[0],ncid,dimids[0],ustart,ucount,udata);
        if(gm==RANDOM)
            init_var(buffer,total_size/shapes[0],0,1,var_type);
        if(gm==LOCALITY){
/*            printf("locality");*/
/*            init_var_locality(buffer,total_size/shapes[0],i*(total_size/shapes[0]),var_type);*/
            init_var_locality(buffer,total_size/shapes[0],i,dshape,dim_size,shift,max_pos,var_type);
        }
        nc_put_vara(var_type,ncid,vlid,start,count,buffer);
    }
    if((res=nc_close(ncid)))
        BAIL(res);
    return 0;
}
int main(int argc,char *argv[]){
/*    size_t shapes[3]={60,120,240};*/
/*    size_t shapes[3]={4096,1024,256};*/
/*    size_t shapes[3]={2048,1024,512};*/
/*    size_t shapes[3]={8,8,8};*/
    size_t shapes[3]={1024,512,256};
    TYPE var_type=DOUBLE;
    TYPE types[3]={DOUBLE,DOUBLE,DOUBLE};
    int dim_size=3;
    char *dimnames[3]={"d1","d2","d3"};
    char *varname="v1";
    gm=RANDOM;
    if(argc==1){
        printf("Usage: %s netcdf_file value_mode.\n",argv[0]);
        printf("\tvalue_mode can be r(random) or l(locality), r is used when value_mode is not specified.\n");
        exit(1);
    }
    if(argc>2){
        if(strcmp(argv[2],"r")==0){
            gm=RANDOM;
        }
        if(strcmp(argv[2],"l")==0){
            gm=LOCALITY;
        }
    }
    generator(argv[1],dimnames,varname,shapes,dim_size,types,var_type);   
    return 0;
}
