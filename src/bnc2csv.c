#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netcdf.h>
#include "common.h"
#include "rsearch.h"
/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

inline void print_var(void *var,nc_type type){
    switch(type){
        case NC_BYTE:
            printf("%d,",*(char *)var);
            break;
        case NC_SHORT:
            printf("%d,",*(short *)var);
            break;
        case NC_INT:
            printf("%d,",*(int *)var);
            break;
/*        case NC_INT64:*/
        case NC_FLOAT:
            printf("%f,",*(float *)var);
            break;
        case NC_DOUBLE:
            printf("%lf,",*(double *)var);
            break;
        default:
            printf("print_var() unknown type!\n");
    }
    return;
}
inline void print_last_var(void *var,nc_type type){
    switch(type){
        case NC_BYTE:
            printf("%d",*(char *)var);
            break;
        case NC_SHORT:
            printf("%d",*(short *)var);
            break;
        case NC_INT:
            printf("%d",*(int *)var);
            break;
/*        case NC_INT64:*/
        case NC_FLOAT:
            printf("%f",*(float *)var);
            break;
        case NC_DOUBLE:
            printf("%lf",*(double *)var);
            break;
        default:
            printf("print_last_var() unknown type!\n");
    }
    return;
}
inline void print_row(void **dims_in,int dims_size, int *idx,nc_type*dtypes,int * sizes,void *buff,size_t j,nc_type vtype,int vsize){
    int i;
    for(i=0;i<dims_size;i++){
        print_var(((char *)dims_in[i])+sizes[i]*idx[i],dtypes[i]);
    }
    print_last_var(((char *)buff)+vsize*j,vtype);
    printf("\n");

}

char *get_type_name(nc_type type){
    switch(type){
        case NC_BYTE:
            return "byte";
        case NC_SHORT:
            return "short";
        case NC_INT:
            return "int";
/*        case NC_INT64:*/
        case NC_FLOAT:
            return "float";
        case NC_DOUBLE:
            return "double";
        default:
            printf("get_type_name() unknown type!\n");
    }
    return "unknown";
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
int main(int argc, char ** argv){
   clock_t begin, end;
   clock_t sort_begin,sort_end;
   begin=clock();
/*   sscanf(argv[1],"%d",&X_LIMIT);  */
   /* This will be the netCDF ID for the file and data variable. */
   int ncid, varid;
   int retval;
   /* Open the file. NC_NOWRITE tells netCDF we want read-only access
    * to the file.*/
   if ((retval = nc_open(argv[1],NC_NOWRITE, &ncid)))
      ERR(retval);
    
   if ((retval = nc_inq_varid(ncid, argv[2], &varid)))
      ERR(retval);
   nc_type vtype;
   int dims_size,nattsp;
   int *dimids=(int *)calloc(NC_MAX_DIMS,sizeof(int));
   if ((retval = nc_inq_var(ncid,varid,0,&vtype,&dims_size,dimids, &nattsp)))
      ERR(retval);
   int i,j;
   nc_type* dtypes=(nc_type*)calloc(dims_size,sizeof(nc_type));
   char **dnames=(char **)calloc(dims_size,sizeof(char *));
   size_t * dsizes=(size_t *)calloc(dims_size,sizeof(size_t));
   size_t all_size=1;
   for(i=0;i<dims_size;i++){
       dnames[i]=(char *)calloc(128,sizeof(char));
       if ((retval = nc_inq_dimname(ncid,dimids[i],dnames[i])))
          ERR(retval);
       if ((retval = nc_inq_dimlen(ncid,dimids[i],&dsizes[i])))
          ERR(retval);
       if ((retval = nc_inq_varid(ncid,dnames[i],&dimids[i])))
          ERR(retval);
       if ((retval = nc_inq_var(ncid,dimids[i],0,&dtypes[i],0,0, &nattsp)))
          ERR(retval);
       all_size*=dsizes[i];
   }
   void ** dims_in=(void **)calloc(dims_size,sizeof(void *));
   size_t *start=(size_t *)calloc(dims_size,sizeof(size_t));
   size_t *count=(size_t *)calloc(dims_size,sizeof(size_t));
   size_t *dshape=(size_t *)calloc(dims_size,sizeof(size_t));
   int *sizes=(int *)calloc(dims_size,sizeof(int));
   int *idx=(int *)calloc(dims_size,sizeof(int));
   int var_size=get_nctype_size(vtype);
   void * buff=(void *)calloc(all_size/dsizes[0],var_size);
   for(i=0;i<dims_size;i++){
       dims_in[i]=(void *)calloc(dsizes[i],get_nctype_size(dtypes[i]));
       nc_get_var(ncid, dimids[i], dims_in[i],dtypes[i]);
       start[i]=0;
       count[i]=dsizes[i];
       sizes[i]=get_nctype_size(dtypes[i]);
   }
   count[0]=1;
   get_dshape(dshape,dsizes,dims_size);
   for(i=0;i<dsizes[0];i++){
       start[i]=i;
       nc_get_vara(ncid,varid,start,count,buff,vtype);
       for(j=0;j<all_size/dsizes[0];j++){
            get_idx(idx,j,dshape,dims_size);
            idx[0]=idx[0]+i;
/*            print_row(dims_in,idx,dtypes,sizes,buff,j,vtype,var_size);*/
            print_row(dims_in,dims_size,idx,dtypes,sizes,buff, j,vtype,var_size);
       }
   }
   if ((retval = nc_close(ncid)))
      ERR(retval);

/*   sort_begin=clock();*/
/*   sort_end=clock();*/
/*   end=clock();*/
/*   printf("all time is %fs and sort time is %fs\n",(double)(end-begin)/CLOCKS_PER_SEC,(double)(sort_end-sort_begin)/CLOCKS_PER_SEC);*/
/*   printf("*** SUCCESS reading example file %s!\n", FILE_NAME);*/
   return 0;
}
