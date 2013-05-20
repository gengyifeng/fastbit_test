#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <netcdf.h>
/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}
/*#define FILE_NAME "/media/Soft/t_10.db.1948-2007.daymean.05APR2010.nc"*/
#define FILE_NAME "t_10.db.1948-2007.daymean.05APR2010.nc"
#define VAR_NAME "T_10_MOD"
#define X_NAME "time"
#define Y_NAME "LAT"
#define Z_NAME "LON"
#define X 21900
//#define X_LIMIT 100
#define Y 94
#define Z 192
#define XU 1

/*#define X 5*/
/*#define Y 10*/
/*#define Z 10*/
#define OUTPUT "T_10_MOD.csv"
int X_LIMIT=1;
typedef struct node_t{
   size_t idx;
   double val;
}node;
typedef struct cnode_t{
   size_t idx;
   double val;
   unsigned char repeat;
}cnode;
int compare(const void *a,const void *b){
   double res=(*(cnode*)a).val-(*(cnode*)b).val;
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
   int xid,yid,zid;
   /* Loop indexes, and error handling. */
   int retval;
   int x,y,z;
   /* Open the file. NC_NOWRITE tells netCDF we want read-only access
    * to the file.*/
   if ((retval = nc_open(FILE_NAME,NC_NOWRITE, &ncid)))
      ERR(retval);

   if ((retval = nc_inq_varid(ncid, VAR_NAME, &varid)))
      ERR(retval);
   if ((retval = nc_inq_varid(ncid, X_NAME, &xid)))
      ERR(retval);
   if ((retval = nc_inq_varid(ncid, Y_NAME, &yid)))
      ERR(retval);
   if ((retval = nc_inq_varid(ncid, Z_NAME, &zid)))
      ERR(retval);
    
   double *x_in= (double *)calloc(sizeof(double),X);
   double *y_in= (double *)calloc(sizeof(double),Y);
   double *z_in= (double *)calloc(sizeof(double),Z);
   memset(z_in,0,sizeof(double)*Z);
   if ((retval = nc_get_var_double(ncid, xid, x_in)))
      ERR(retval);
   if ((retval = nc_get_var_double(ncid, yid, y_in)))
      ERR(retval);
   if ((retval = nc_get_var_double(ncid, zid, z_in)))
      ERR(retval);
    /*   double data_in[XU][Y][Z];*/
   size_t start[]={0,0,0};
   size_t count[]={XU,Y,Z};
   double *data_in= (double *)malloc(sizeof(double)*XU*Y*Z);
   cnode * data=(cnode *)malloc(sizeof(cnode)*X_LIMIT*Y*Z);
/*   memset(data,0,sizeof(cnode)*X_LIMIT*Y*Z);*/
     
   for(x=0;x<X_LIMIT;x+=XU){
       memset(data_in,0,sizeof(double)*XU*Y*Z);
       /* Get the varid of the data variable, based on its name. */

       /* Read the data. */
    /*   if ((retval = nc_get_var_double(ncid, varid, data_in)))*/
    /*      ERR(retval);*/
    /*   if ((retval = nc_get_vara_double(ncid, varid,start,count, &data_in[0][0][0])))*/
    /*      ERR(retval);*/
       if ((retval = nc_get_vara_double(ncid, varid,start,count, data_in)))
          ERR(retval);
        int j;
	size_t idx;
        for(j=0;j<XU;j++)
            for (y = 0; y < Y; y++)
                for (z = 0; z < Z; z++){
        /*            printf("%d,%d,%d,%lf\n",x,y,z,data_in[x][y][z]);*/
                 //printf("%lf,%lf,%lf,%lf\n",x_in[x+j],y_in[y],z_in[z],data_in[j*Y*Z+y*Z+z]);
		  idx=(x+j)*Y*Z+y*Z+z;
		  data[idx].idx=idx;
		  data[idx].val=data_in[j*Y*Z+y*Z+z];
/*                 printf("%d,%d,%d,%lf\n",x+j,y,z,data_in[j*Y*Z+y*Z+z]);*/

                }
       start[0]+=XU;
   }
   FILE * fp=fopen(argv[2],"w");
   char idx_name[128]={0};
   sprintf(idx_name,"%s_idx",argv[2]);
   printf("%s\n",idx_name);
   FILE *fp_idx=fopen(idx_name,"w");
   FILE * xfp=fopen(X_NAME,"w");
   FILE * yfp=fopen(Y_NAME,"w");
   FILE * zfp=fopen(Z_NAME,"w");

   sort_begin=clock();
   qsort(data,X_LIMIT*Y*Z,sizeof(cnode),compare);
   sort_end=clock();
   
/*   fwrite(data,sizeof(cnode),X_LIMIT*Y*Z,fp);*/
   size_t i;
   cnode tmp,last;
   last.idx=0;
   last.val=data[0].val;
   last.repeat=1;
   int linenum=1;
   fwrite(&(data[0].idx),sizeof(size_t),1,fp_idx);
   for(i=1;i<X_LIMIT*Y*Z;i++){
      memcpy(&tmp,&data[i],sizeof(cnode));
      if(tmp.val==last.val&&last.repeat!=UCHAR_MAX){
          last.repeat++;
      }else{
/*          printf("%f ",last.val);*/
          fwrite(&last,sizeof(cnode),1,fp);
          last.idx=i;
          last.val=tmp.val;
          last.repeat=1;
          linenum++;
      }
      fwrite(&(tmp.idx),sizeof(size_t),1,fp_idx);
   }
   printf("line number %d\n",linenum);
   fwrite(&last,sizeof(cnode),1,fp);
   fwrite(x_in,sizeof(double),X,xfp);
   fwrite(y_in,sizeof(double),Y,yfp);
   fwrite(z_in,sizeof(double),Z,zfp);
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
   fclose(xfp);
   fclose(yfp);
   fclose(zfp);
   end=clock();
   printf("all time is %fs and sort time is %fs\n",(double)(end-begin)/CLOCKS_PER_SEC,(double)(sort_end-sort_begin)/CLOCKS_PER_SEC);
/*   printf("*** SUCCESS reading example file %s!\n", FILE_NAME);*/
   return 0;
}
