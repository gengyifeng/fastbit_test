#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
//#define X_LIMIT 21900
#define Y 94
#define Z 192
#define XU 1

/*#define X 5*/
/*#define Y 10*/
/*#define Z 10*/
int X_LIMIT=1;
int main(int argc, char ** argv){
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
        for(j=0;j<XU;j++)
            for (y = 0; y < Y; y++)
                for (z = 0; z < Z; z++){
        /*            printf("%d,%d,%d,%lf\n",x,y,z,data_in[x][y][z]);*/
                 printf("%lf,%lf,%lf,%lf\n",x_in[x+j],y_in[y],z_in[z],data_in[j*Y*Z+y*Z+z]);
/*                 printf("%d,%d,%d,%lf\n",x+j,y,z,data_in[j*Y*Z+y*Z+z]);*/

                }
       start[0]+=XU;
   }
   /* Close the file, freeing all resources. */
   if ((retval = nc_close(ncid)))
      ERR(retval);

/*   printf("*** SUCCESS reading example file %s!\n", FILE_NAME);*/
   return 0;
}
