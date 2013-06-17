#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include <time.h>*/
#include <sys/time.h>
#include <netcdf.h>
#include "common.h"
#include "rsearch.h"
/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

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
void nbound(int *bound,int n,int size){
    int i;
    for(i=0;i<size;i++){
        bound[i]=n;
    }
}
/*int get_block_size(int *bound,size_t *shape,int size){*/
/*    int len=1;*/
/*    int i;*/
/*    for(i=0;i<size;i++){*/
/*        len*=shape[i]/bound[i];*/
/*    }*/
/*    return len;*/
/*}*/
/*void get_new_shape(size_t *newshape,int *bound,size_t *shape,int size){*/
/*    int i;*/
/*    for(i=0;i<size;i++){*/
/*        if(shape[i]>=bound[i]){*/
/*            newshape[i]=bound[i];*/
/*        }else{*/
/*            newshape[i]=shape[i];*/
/*        }*/
/*    }*/
/*    */
/*}*/
/*inline void get_start_count(size_t *start,size_t *count,int *newidx,size_t *newshape,int *bound,size_t *shape,int size){*/
/*   int i,len;*/
/*   for(i=0;i<size;i++){*/
/*       len=shape[i]/bound[i];*/
/*       start[i]=newidx[i]*len;*/
/*       if(newidx[i]!=newshape[i]-1){*/
/*           count[i]=len;*/
/*       }else{*/
/*           count[i]=shape[i]-(newshape[i]-1)*len;*/
/*       }*/
/*   }*/
/*}*/

/*int nc_get_vara(int ncid, int varid, size_t *start, size_t *count, void* buff,nc_type type){*/
/*    int retval=-1; */
/*    switch(type){*/
/*        case NC_BYTE:*/
/*            retval=nc_get_vara_uchar(ncid,varid,start,count,(unsigned char *)buff);*/
/*            break;*/
/*        case NC_SHORT:*/
/*            retval=nc_get_vara_short(ncid,varid,start,count,(short  *)buff);*/
/*            break;*/
/*        case NC_INT:*/
/*            retval=nc_get_vara_int(ncid,varid,start,count,(int  *)buff);*/
/*            break;*/
/*|+        case NC_INT64:+|*/
/*|+            retval=nc_get_vara_long(ncid,varid,start,count,(long  *)buff);+|*/
/*|+            break;+|*/
/*        case NC_FLOAT:*/
/*            retval=nc_get_vara_float(ncid,varid,start,count,(float  *)buff);*/
/*            break;*/
/*        case NC_DOUBLE:*/
/*            retval=nc_get_vara_double(ncid,varid,start,count,(double  *)buff);*/
/*            break;*/
/*        default:*/
/*            printf("nc_get_vara() unknown type!\n");*/
/*    }*/
/*    return retval;*/
/**/
/*    }*/
/*int nc_get_var(int ncid,int varid,void *buff,nc_type type){*/
/*    int retval=-1; */
/*    switch(type){*/
/*        case NC_BYTE:*/
/*            retval=nc_get_var_uchar(ncid,varid,(unsigned char *)buff);*/
/*            break;*/
/*        case NC_SHORT:*/
/*            retval=nc_get_var_short(ncid,varid,(short  *)buff);*/
/*            break;*/
/*        case NC_INT:*/
/*            retval=nc_get_var_int(ncid,varid,(int  *)buff);*/
/*            break;*/
/*|+        case NC_INT64:+|*/
/*|+            retval=nc_get_var_long(ncid,varid,(long  *)buff);+|*/
/*|+            break;+|*/
/*        case NC_FLOAT:*/
/*            retval=nc_get_var_float(ncid,varid,(float  *)buff);*/
/*            break;*/
/*        case NC_DOUBLE:*/
/*            retval=nc_get_var_double(ncid,varid,(double  *)buff);*/
/*            break;*/
/*        default:*/
/*            printf("nc_get_var() unknown type!\n");*/
/*    }*/
/*    return retval;*/
/*}*/
int bcompare(const void *a,const void *b){
   double res=(*(bnode*)a).val-(*(bnode*)b).val;
   if(res>0) return 1;
   if(res<0) return -1;
   if(res==0) return 0;
}
int main(int argc, char ** argv){
/*   clock_t begin, end;*/
/*   clock_t sort_begin,sort_end;*/
   struct timeval begin,end;
   struct timeval sort_begin,sort_end;
   gettimeofday(&begin,NULL);
   if(argc!=5){
       printf("Usage:%s netcdf_file var_name bound indexing_file\n",argv[0]);
       exit(1);
   }
   /* This will be the netCDF ID for the file and data variable. */
   int ncid, varid;
   int retval;
   /* Open the file. NC_NOWRITE tells netCDF we want read-only access
    * to the file.*/
/*   if ((retval = nc_open(FILE_NAME,NC_NOWRITE, &ncid)))*/
/*      ERR(retval);*/
   if ((retval = nc_open(argv[1],NC_NOWRITE, &ncid)))
      ERR(retval);
    
   if ((retval = nc_inq_varid(ncid, argv[2], &varid)))
      ERR(retval);
   nc_type vtype;
   int dims_size,nattsp;
   char vname[128]={0};
   int *dimids=(int *)calloc(NC_MAX_DIMS,sizeof(int));
   if ((retval = nc_inq_var(ncid,varid,vname,&vtype,&dims_size,dimids, &nattsp)))
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
   for(i=0;i<dims_size;i++){
       dims_in[i]=(void *)calloc(dsizes[i],get_nctype_size(dtypes[i]));
       nc_get_var(ncid, dimids[i], dims_in[i],dtypes[i]);
   }

   int n;
   sscanf(argv[3],"%d",&n);
   size_t *start=(size_t*)calloc(dims_size,sizeof(size_t));
   size_t *count=(size_t*)calloc(dims_size,sizeof(size_t));
   size_t  *countdshape=(size_t *)calloc(dims_size,sizeof(size_t));
   size_t *newshape=(size_t *)calloc(dims_size,sizeof(size_t));
   size_t *newdshape=(size_t *)calloc(dims_size,sizeof(size_t));
   size_t *newidx=(size_t *)calloc(dims_size,sizeof(size_t));
   int *bound=(int *)calloc(dims_size,sizeof(int));
   nbound(bound,n,dims_size);
   get_new_shape(newshape,bound,dsizes,dims_size);
   get_dshape(newdshape,newshape,dims_size);
   int block_size=get_max_block_size(bound,dsizes,dims_size);
   int vsize=get_nctype_size(vtype);
   double * buff=(double *)calloc(block_size,vsize);
   bnode *data=(bnode *)calloc(block_size,sizeof(bnode));
   int block_num=1;
   for(i=0;i<dims_size;i++){
       block_num*=newshape[i];
   }

   FILE * fp=fopen(argv[4],"w");
   char idx_name[128]={0};
   sprintf(idx_name,"%s_bidx",argv[4]);
   printf("%s\n",idx_name);
   FILE *fp_idx=fopen(idx_name,"w");
   sprintf(idx_name,"%s_tidx",argv[4]);
   printf("%s\n",idx_name);
   FILE *fp_tidx=fopen(idx_name,"w");
   sprintf(idx_name,"%s_meta",argv[4]);
   printf("%s\n",idx_name);
   FILE *fp_meta=fopen(idx_name,"w");
   sprintf(idx_name,"%s_binfo",argv[4]);
   printf("%s\n",idx_name);

   FILE *fp_binfo=fopen(idx_name,"w");
   FILE **dfp=(FILE **)calloc(dims_size,sizeof(FILE *));
   fprintf(fp_meta,"Dimension size=%d\n",dims_size);
   fprintf(fp_meta,"Dimension\tType\tLength\tPartition\n");
   char tname[128]={0};
   for(i=0;i<dims_size;i++){
       fprintf(fp_meta,"%s\t%s\t%d\t%d\n",dnames[i],get_type_name(dtypes[i]),dsizes[i],bound[i]);
       dfp[i]=fopen(dnames[i],"w");
   }
   fprintf(fp_meta,"Variable\tType\n");
   fprintf(fp_meta,"%s\t%s\n",vname,get_type_name(vtype));
   fprintf(fp_meta,"Block Arrangement=linear\n");
   size_t boffset=0;
   block_info *binfo=(block_info *)calloc(block_num,sizeof(block_info));
/*   int max_level=get_max_level(block_num);*/
/*   int vnodes_size=get_tree_size(block_num);*/
   vnode *vnodes=(vnode *)calloc(block_num,sizeof(vnode));
   double *vals=(double *)calloc(block_size,sizeof(double));
   unsigned int *idxs =(unsigned int *)calloc(block_size,sizeof(unsigned int));
   double secs=0;
   for(i=0;i<block_num;i++){
/*       printf("Block num %d\n",i);*/
       get_idx(newidx,i,newdshape,dims_size);
       get_start_count(start,count,newidx,newshape,bound,dsizes,dims_size);
       int count_size=1;
       for(j=0;j<dims_size;j++){
           count_size*=count[j];
/*           printf("newidx[%d]=%d\n",j,newidx[j]);*/
/*           printf("start[%d]=%d\n",j,start[j]);*/
/*           printf("count[%d]=%d\n",j,count[j]);*/
       }
/*       printf("count_size %d\n",count_size);*/
       if ((retval = nc_get_vara(ncid, varid,start,count,buff,vtype)))
          ERR(retval);
/*       get_dshape(countdshape,count,dims_size);*/
       for(j=0;j<count_size;j++){
/*           get_idx(newidx,j,countdshape,dims_size);*/
            data[j].idx=j;
            data[j].val=buff[j];
       }
/*       sort_begin=clock();*/
       gettimeofday(&sort_begin,NULL);
       qsort(data,count_size,sizeof(bnode),bcompare);
       gettimeofday(&sort_end,NULL);
       secs+=sort_end.tv_sec-sort_begin.tv_sec+1.0*(sort_end.tv_usec-sort_begin.tv_usec)/1000000;

       for(j=0;j<count_size;j++){
            idxs[j]=data[j].idx;
            vals[j]=data[j].val; 
       }
       binfo[i].boffset=boffset;
       binfo[i].min=data[0].val;
       binfo[i].max=data[count_size-1].val;
/*       printf("id %d boffset %d count_size %d block min %lf, max %lf\n",i,boffset, count_size,binfo[i].min,binfo[i].max);*/
       vnodes[i].min=data[0].val;
       vnodes[i].max=data[count_size-1].val;
       vnodes[i].val=i;
/*       fwrite(&boffset,sizeof(size_t),1,fp_boffset);*/
       fwrite(idxs,sizeof(unsigned int),count_size,fp_idx);
/*       if(i==0)*/
/*           printf("test %d %lf\n",idxs[0],vals[0]);*/
       fwrite(vals,sizeof(double),count_size,fp);
       boffset+=count_size;
   }
/*   fwrite(&max_level,sizeof(int),1,fp_tidx);*/
   fwrite(vnodes,sizeof(vnode),block_num,fp_tidx);
   fwrite(binfo,sizeof(block_info),block_num,fp_binfo);
   
   for(i=0;i<dims_size;i++){
       fwrite(dims_in[i],get_nctype_size(dtypes[i]),dsizes[i],dfp[i]);
   }
   if ((retval = nc_close(ncid)))
      ERR(retval);
   fclose(fp);
   fclose(fp_binfo);
   fclose(fp_idx);
   fclose(fp_tidx);
   fclose(fp_meta);
   for(i=0;i<dims_size;i++){
       fclose(dfp[i]);
   }


/*   count[0]=XU;*/
/*   size_t buff_size=XU;*/
/*   for(i=1;i<dims_size;i++){*/
/*       count[i]=dsizes[i];*/
/*       buff_size*=dsizes[i];*/
/*   }*/
/*   int vsize=get_size(vtype);*/
/*   void *data_in= (void *)calloc(buff_size,vsize);*/
/*   node * data=(node *)calloc(X_LIMIT*buff_size,sizeof(node));*/
/*   double rawdata=(double *)calloc(X_LIMIT*buff_size,sizeof(double));*/
/*   int x; */
/*   size_t slen=buff_size/XU;*/
/*   */
/*   for(x=0;x<X_LIMIT;x+=XU){*/
/*       memset(data_in,0,vsize*buff_size);*/
/*       if ((retval = nc_get_vara(ncid, varid,start,count, data_in,vtype)))*/
/*          ERR(retval);*/
/*        int j;*/
/*        for(j=0;j<buff_size;j++){*/
/*            data[j+x*slen].idx=j+x*slen;*/
/*            data[j+x*slen].val=((double *)data_in)[j];*/
/*        }*/
/*        memcpy(rawdata[x*slen],(double *)data_in,sizeof(double)*buff_size);*/
/*        start[0]+=XU;*/
/*   }*/

/*   qsort(data,X_LIMIT*slen,sizeof(node),compare);*/
/*   fwrite(data,sizeof(node),X_LIMIT*slen,fp);*/
   gettimeofday(&end,NULL);
   printf("all time is %fs and sort time is %fs\n",(end.tv_sec-begin.tv_sec+1.0*(end.tv_usec-begin.tv_usec)/1000000),secs);
/*   printf("*** SUCCESS reading example file %s!\n", FILE_NAME);*/
   return 0;
}
