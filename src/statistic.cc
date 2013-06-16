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
inline void print_row(void **dims_in,int dims_size, size_t *idx,nc_type*dtypes,size_t * sizes,void *buff,size_t j,nc_type vtype,int vsize){
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
   size_t i,j;
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
   size_t *sizes=(size_t *)calloc(dims_size,sizeof(size_t));
   size_t *idx=(size_t *)calloc(dims_size,sizeof(size_t));
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
   size_t hits=0;
   for(i=0;i<dsizes[0];i++){
       start[0]=i;
       nc_get_vara(ncid,varid,start,count,buff,vtype);
       for(j=0;j<all_size/dsizes[0];j++){
            get_idx(idx,j,dshape,dims_size);
            idx[0]+=i;
            if(((double *)buff)[j]>=0.1&&((double *)buff)[j]<=0.4){
                hits++;
            }
/*            printf("i %d j %d idx[0] %d idx[1] %d idx[2] %d\n",i,j,idx[0],idx[1],idx[2]);*/
//            print_row(dims_in,dims_size,idx,dtypes,sizes,buff,j,vtype,var_size);
       }
   }
   printf("hits %d\n",hits);
   if ((retval = nc_close(ncid)))
      ERR(retval);

/*   sort_begin=clock();*/
/*   sort_end=clock();*/
/*   end=clock();*/
/*   printf("all time is %fs and sort time is %fs\n",(double)(end-begin)/CLOCKS_PER_SEC,(double)(sort_end-sort_begin)/CLOCKS_PER_SEC);*/
/*   printf("*** SUCCESS reading example file %s!\n", FILE_NAME);*/
   return 0;
}
