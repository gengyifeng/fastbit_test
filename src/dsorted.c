#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netcdf.h>
#include "common.h"
/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}
/*#define FILE_NAME "/media/Soft/t_10.db.1948-2007.daymean.05APR2010.nc"*/
#define FILE_NAME "t_10.db.1948-2007.daymean.05APR2010.nc"
#define VAR_NAME "T_10_MOD"
#define X 21900
//#define X_LIMIT 100
#define XU 1

/*#define X 5*/
/*#define Y 10*/
/*#define Z 10*/
int X_LIMIT=1;
int get_size(nc_type  type){
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
            printf("get_size() unknown type!\n");
    }
    return -1;
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
int compare(const void *a,const void *b){
   double res=(*(node*)a).val-(*(node*)b).val;
   if(res>0) return 1;
   if(res<0) return -1;
   if(res==0) return 0;
}
int main(int argc, char ** argv){
   clock_t begin, end;
   clock_t sort_begin,sort_end;
   begin=clock();
   sscanf(argv[1],"%d",&X_LIMIT);  
   /* This will be the netCDF ID for the file and data variable. */
   int ncid, varid;
   int retval;
   /* Open the file. NC_NOWRITE tells netCDF we want read-only access
    * to the file.*/
   if ((retval = nc_open(FILE_NAME,NC_NOWRITE, &ncid)))
      ERR(retval);
    
   if ((retval = nc_inq_varid(ncid, VAR_NAME, &varid)))
      ERR(retval);
   nc_type vtype;
   int dims_size,nattsp;
   int *dimids=(int *)calloc(NC_MAX_DIMS,sizeof(int));
   if ((retval = nc_inq_var(ncid,varid,0,&vtype,&dims_size,dimids, &nattsp)))
      ERR(retval);
   int i;
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

/*       printf("%s %d %d\n",dnames[i],dsizes[i],dimids[i]);*/
/*       if ((retval = nc_inq_var(ncid,dimids[i],dnames[i],&dtypes[i],&dsizes[i],NULL,&nattsp)))*/
/*          ERR(retval);*/
       all_size*=dsizes[i];
   }
/*    */
/*   if ((retval = nc_inq_varid(ncid, X_NAME, &xid)))*/
/*      ERR(retval);*/
/*   if ((retval = nc_inq_varid(ncid, Y_NAME, &yid)))*/
/*      ERR(retval);*/
/*   if ((retval = nc_inq_varid(ncid, Z_NAME, &zid)))*/
/*      ERR(retval);*/
   void ** dims_in=(void **)calloc(dims_size,sizeof(void *));
   for(i=0;i<dims_size;i++){
       dims_in[i]=(void *)calloc(dsizes[i],get_size(dtypes[i]));
       nc_get_var(ncid, dimids[i], dims_in[i],dtypes[i]);

   }

/*   double *x_in= (double *)calloc(sizeof(double),X);*/
/*   double *y_in= (double *)calloc(sizeof(double),Y);*/
/*   double *z_in= (double *)calloc(sizeof(double),Z);*/
/*   if ((retval = nc_get_var_double(ncid, xid, x_in)))*/
/*      ERR(retval);*/
/*   if ((retval = nc_get_var_double(ncid, yid, y_in)))*/
/*      ERR(retval);*/
/*   if ((retval = nc_get_var_double(ncid, zid, z_in)))*/
/*      ERR(retval);*/
    /*   double data_in[XU][Y][Z];*/
/*   size_t start[]={0,0,0};*/
   size_t *start=(size_t*)calloc(dims_size,sizeof(size_t));
   size_t *count=(size_t*)calloc(dims_size,sizeof(size_t));
/*   size_t buff_size=XU;*/
   count[0]=XU;
   size_t buff_size=XU;
   for(i=1;i<dims_size;i++){
       count[i]=dsizes[i];
       buff_size*=dsizes[i];
   }
/*   size_t count[]={XU,Y,Z};*/
/*   size_t count[]={XU,Y,Z};*/
/*   double *data_in= (double *)calloc(buff_size,get_size(vtype));*/
   int vsize=get_size(vtype);
   void *data_in= (void *)calloc(buff_size,vsize);
/*   node * data=(node *)calloc(sizeof(node),X_LIMIT*Y*Z);*/
   node * data=(node *)calloc(X_LIMIT*buff_size,sizeof(node));
/*   node * data=(node *)calloc(all_size,sizeof(node));*/
   int x; 
/*   int *dshape=(int *)calloc(dims_size,sizeof(int)); */
/*   get_dshape(dshape,dsizes,dims_size);*/
/*   size_t idx;*/
   size_t slen=buff_size/XU;
   for(x=0;x<X_LIMIT;x+=XU){
       memset(data_in,0,vsize*buff_size);
       /* Get the varid of the data variable, based on its name. */

       /* Read the data. */
    /*   if ((retval = nc_get_var_double(ncid, varid, data_in)))*/
    /*      ERR(retval);*/
    /*   if ((retval = nc_get_vara_double(ncid, varid,start,count, &data_in[0][0][0])))*/
    /*      ERR(retval);*/
       if ((retval = nc_get_vara(ncid, varid,start,count, data_in,vtype)))
          ERR(retval);
        int j;
        for(j=0;j<buff_size;j++){
            data[j+x*slen].idx=j+x*slen;
            data[j+x*slen].val=((double *)data_in)[j];
        }
/*        for(j=0;j<XU;j++)*/
/*            for (y = 0; y < Y; y++)*/
/*                for (z = 0; z < Z; z++){*/
        /*            printf("%d,%d,%d,%lf\n",x,y,z,data_in[x][y][z]);*/
                 //printf("%lf,%lf,%lf,%lf\n",x_in[x+j],y_in[y],z_in[z],data_in[j*Y*Z+y*Z+z]);
/*          idx=(x+j)*Y*Z+y*Z+z;*/
/*          data[idx].idx=idx;*/
/*          data[idx].val=data_in[j*Y*Z+y*Z+z];*/
/*                 printf("%d,%d,%d,%lf\n",x+j,y,z,data_in[j*Y*Z+y*Z+z]);*/

/*                }*/
       start[0]+=XU;
   }
   FILE * fp=fopen(argv[2],"w");
   FILE **dfp=(FILE **)calloc(dims_size,sizeof(FILE *));
   for(i=0;i<dims_size;i++){
    dfp[i]=fopen(dnames[i],"w");
   }
/*   FILE * xfp=fopen(dnames[0],"w");*/
/*   FILE * yfp=fopen(dnames[1],"w");*/
/*   FILE * zfp=fopen(dnames[2],"w");*/

   sort_begin=clock();
   qsort(data,X_LIMIT*slen,sizeof(node),compare);
   sort_end=clock();
   fwrite(data,sizeof(node),X_LIMIT*slen,fp);
   for(i=0;i<dims_size;i++){
       fwrite(dims_in[i],get_size(dtypes[i]),dsizes[i],dfp[i]);
   }
/*   fwrite(x_in,sizeof(double),X,xfp);*/
/*   fwrite(y_in,sizeof(double),Y,yfp);*/
/*   fwrite(z_in,sizeof(double),Z,zfp);*/
   //for(x=0;x<X_LIMIT;x++)
     // for(y=0;y<Y;y++)
       //  for(z=0;z<Z;z++){
   	    //fprintf(fp,"%ld%lf",data[x*Y*Z+y*Z+z].idx,data[x*Y*Z+y*Z+z].val);
         //    fwrite(data[x*Y*Z+y*Z+z],sizeof()) 
	//}
   /* Close the file, freeing all resources. */
   if ((retval = nc_close(ncid)))
      ERR(retval);
   fclose(fp);
   for(i=0;i<dims_size;i++){
       fclose(dfp[i]);
   }
   end=clock();
   printf("all time is %fs and sort time is %fs\n",(double)(end-begin)/CLOCKS_PER_SEC,(double)(sort_end-sort_begin)/CLOCKS_PER_SEC);
/*   printf("*** SUCCESS reading example file %s!\n", FILE_NAME);*/
   return 0;
}
