#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include <time.h>*/
#include <sys/time.h>
#include <netcdf.h>
#include <limits.h>
#include "common.h"
#include "rsearch.h"
#include "mapping.h"
/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}
#define WRITE_BUFF_SIZE 16777216 //16M

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
void nbound(size_t *bound,int n,int size){
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
   if(argc<5){
       printf("Usage:%s netcdf_file var_name partition indexing_file layout\n",argv[0]);
       printf("\tEach dimension is divided into the number of partition.\n");
       printf("\tLayout can be l(linear) or h(hilbert curve), if not specified, layout is chosen by the program.\n");
       printf("\tYou can check the layout information in the meta file.\n");
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
   LAYOUT ly=HCURVE;
   sscanf(argv[3],"%d",&n);
   int nt=n;
   while(nt>1){
       if(nt%2!=0){
           ly=LINEAR;
           break;
       }
       nt=nt>>1;
   }
   if(argc>5){
       if(strcmp(argv[5],"l")==0){
           ly=LINEAR;
       }else if(strcmp(argv[5],"h")==0){
           ly=HCURVE;
       }
   }
   size_t *start=(size_t*)calloc(dims_size,sizeof(size_t));
   size_t *count=(size_t*)calloc(dims_size,sizeof(size_t));
   size_t  *countdshape=(size_t *)calloc(dims_size,sizeof(size_t));
   size_t *newshape=(size_t *)calloc(dims_size,sizeof(size_t));
   size_t *newdshape=(size_t *)calloc(dims_size,sizeof(size_t));
   size_t *newidx=(size_t *)calloc(dims_size,sizeof(size_t));
   size_t *bound=(size_t *)calloc(dims_size,sizeof(size_t));
   nbound(bound,n,dims_size);
   get_new_shape(newshape,bound,dsizes,dims_size);
   for(i=0;i<dims_size;i++){
       if(newshape[i]!=n){
           ly=LINEAR;
           break;
       }
   }
   get_dshape(newdshape,newshape,dims_size);
   size_t block_size=get_max_block_size(bound,dsizes,dims_size);
   if(block_size>ULONG_MAX){
       printf("index number for a block is beyond the max value of unsigned int\n");
       exit(1);
   }
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
   char tname[256]={0};
   for(i=0;i<dims_size;i++){
       fprintf(fp_meta,"%s\t%s\t%d\t%d\n",dnames[i],get_type_name(dtypes[i]),dsizes[i],bound[i]);
       bzero(tname,sizeof(tname));
       sprintf(tname,"%s.%s",argv[4],dnames[i]);
/*       dfp[i]=fopen(dnames[i],"w");*/
       dfp[i]=fopen(tname,"w");
   }
   fprintf(fp_meta,"Variable\tType\n");
   fprintf(fp_meta,"%s\t%s\n",vname,get_type_name(vtype));
   if(ly==LINEAR)
       fprintf(fp_meta,"Block Arrangement=linear\n");
   if(ly==HCURVE)
       fprintf(fp_meta,"Block Arrangement=hcurve\n");
   if(block_size>65535){
       fprintf(fp_meta,"InnerIndexSize=%d\n",sizeof(int));
   }else{
       fprintf(fp_meta,"InnerIndexSize=%d\n",sizeof(short));
   }
   size_t boffset=0;
   int isize=sizeof(int);
   if(block_size<=65535){
       isize=sizeof(short);
   }
   block_info *binfo=(block_info *)calloc(block_num,sizeof(block_info));
/*   int max_level=get_max_level(block_num);*/
/*   int vnodes_size=get_tree_size(block_num);*/
   vnode *vnodes=(vnode *)calloc(block_num,sizeof(vnode));
   double *vals=(double *)calloc(block_size,sizeof(double));
/*   unsigned int *idxs =(unsigned int *)calloc(block_size,sizeof(unsigned int));*/
   char *idxs =(char *)calloc(block_size,isize);
   char *vals_batch_buff=(char *)calloc(WRITE_BUFF_SIZE,sizeof(char));
   char *idxs_batch_buff=(char *)calloc(WRITE_BUFF_SIZE,sizeof(char));
   bool use_batch=true;
   bool iuse_batch=true;
   if(block_size*sizeof(double)>=WRITE_BUFF_SIZE){
       use_batch=false;
   }
   if(block_size*sizeof(unsigned int)>=WRITE_BUFF_SIZE){
       iuse_batch=false;
   }
   size_t off=0;
   size_t ioff=0;
   double secs=0;
   double writetime=0;
   double readtime=0;
   Hcode h;
   h.hcode=(U_int *)calloc(dims_size,sizeof(U_int));
   Point pt;
   pt.hcode=(U_int *)calloc(dims_size,sizeof(U_int));
   U_int *g_mask=(U_int *)calloc(dims_size,sizeof(U_int));
   for(i=0;i<dims_size;i++){
       g_mask[i]=1<< dims_size-1-i;      
   }
   printf("BLOCK NUM %d\n",block_num);
   for(i=0;i<block_num;i++){
       if(ly==HCURVE){
           bzero(h.hcode,sizeof(U_int)*dims_size);
           bzero(pt.hcode,sizeof(U_int)*dims_size);
           h.hcode[0]=i;
           H_decode(pt,h,dims_size,g_mask);
           for(j=0;j<dims_size;j++){
               newidx[j]=pt.hcode[j];
    /*           printf("%d ",newidx[j]);*/
           }
    /*       printf("\n");*/
       }else{
           get_idx(newidx,i,newdshape,dims_size);
       }
       get_start_count(start,count,newidx,newshape,bound,dsizes,dims_size);
       int count_size=1;
       for(j=0;j<dims_size;j++){
           count_size*=count[j];
/*           printf("newidx[%d]=%d\n",j,newidx[j]);*/
/*           printf("start[%d]=%d\n",j,start[j]);*/
/*           printf("count[%d]=%d\n",j,count[j]);*/
       }
       gettimeofday(&sort_begin,NULL);
       if ((retval = nc_get_vara(ncid, varid,start,count,buff,vtype)))
          ERR(retval);
       gettimeofday(&sort_end,NULL);
       readtime+=sort_end.tv_sec-sort_begin.tv_sec+1.0*(sort_end.tv_usec-sort_begin.tv_usec)/1000000;
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
       if(isize==sizeof(int)){
           for(j=0;j<count_size;j++){
                ((unsigned int *)idxs)[j]=data[j].idx;
                vals[j]=data[j].val; 
           }
       }else{
           for(j=0;j<count_size;j++){
                ((unsigned short *)idxs)[j]=(unsigned short)data[j].idx;
                vals[j]=data[j].val; 
           }
       }
       binfo[i].boffset=boffset;
       binfo[i].min=data[0].val;
       binfo[i].max=data[count_size-1].val;
/*       printf("id %d boffset %d count_size %d block min %lf, max %lf\n",i,boffset, count_size,binfo[i].min,binfo[i].max);*/
       vnodes[i].min=data[0].val;
       vnodes[i].max=data[count_size-1].val;
       vnodes[i].val=i;
/*       fwrite(&boffset,sizeof(size_t),1,fp_boffset);*/
       gettimeofday(&sort_begin,NULL);
       if(iuse_batch){
            to_batch_buff(idxs_batch_buff,&ioff,WRITE_BUFF_SIZE,idxs,isize*count_size,fp_idx);
       }else{
           fwrite(idxs,isize,count_size,fp_idx);
       }
/*       if(i==0)*/
/*           printf("test %d %lf\n",idxs[0],vals[0]);*/
       if(use_batch){
            to_batch_buff(vals_batch_buff,&off,WRITE_BUFF_SIZE,vals,sizeof(double)*count_size,fp);
       }else{
           fwrite(vals,sizeof(double),count_size,fp);
       }
       gettimeofday(&sort_end,NULL);
       writetime+=sort_end.tv_sec-sort_begin.tv_sec+1.0*(sort_end.tv_usec-sort_begin.tv_usec)/1000000;
       boffset+=count_size;
/*       if(boffset>all_size)*/
/*           printf("%d %d\n",i,boffset);*/
   }
   gettimeofday(&sort_begin,NULL);
   if(iuse_batch)
       flush_batch_buff(idxs_batch_buff,&ioff,WRITE_BUFF_SIZE,fp_idx);
   if(use_batch)
       flush_batch_buff(vals_batch_buff,&off,WRITE_BUFF_SIZE,fp);
/*   fwrite(&max_level,sizeof(int),1,fp_tidx);*/
   fwrite(vnodes,sizeof(vnode),block_num,fp_tidx);
   fwrite(binfo,sizeof(block_info),block_num,fp_binfo);
   gettimeofday(&sort_end,NULL);
   writetime+=sort_end.tv_sec-sort_begin.tv_sec+1.0*(sort_end.tv_usec-sort_begin.tv_usec)/1000000;
   
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
   printf("all time is %lfs and sort time is %lfs and write_time is %lf and read time is %lf\n",(end.tv_sec-begin.tv_sec+1.0*(end.tv_usec-begin.tv_usec)/1000000),secs,writetime,readtime);
/*   printf("*** SUCCESS reading example file %s!\n", FILE_NAME);*/
   return 0;
}
