#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netcdf.h>
//#include <limits.h>
#include <float.h>
#include <sys/time.h>
#include "common.h"
using namespace std;
/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}
typedef struct condition_t{
    double *min;
    bool *minequal;
    double *max;
    bool *maxequal;
    bool *valid; 
    int size;
}cond;

int init_conditions(cond *conds,int dims_size){
    int i;
    conds->size=dims_size;
    conds->min=(double *)calloc(dims_size,sizeof(double));
    conds->minequal=(bool *)calloc(dims_size,sizeof(bool));
    conds->max=(double *)calloc(dims_size,sizeof(double));
    conds->maxequal=(bool *)calloc(dims_size,sizeof(bool));
    conds->valid=(bool *)calloc(dims_size,sizeof(bool));
    for(i=0;i<dims_size;i++){
        conds->minequal[i]=true;
        conds->maxequal[i]=true;
        conds->valid[i]=false;
    }
}

int parse_condition(char *s,double *min,bool *min_equal,double *max,bool *max_equal){
    char lb,rb;
    sscanf(s,"%c%lf,%lf%c",&lb,min,max,&rb);
    if(lb=='('){
        *min_equal=false;
    }
    if(rb==')'){
        *max_equal=false;
    }
/*    printf("%s min %lf max %lf\n",s, *min,*max);*/
    return 0;
}
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

//int set_begin_end(size_t  *begin, size_t *end,DIMS &dims,cond &conds){
//    int i;
//    result res;
//    for(i=0;i<dims.dims_size;i++){
//        if(conds.valid[i]==true){
//            if(0<=binary_search((double*)(dims.dimvals[i]),dims.shape[i],conds.min[i],conds.max[i],conds.minequal[i],conds.maxequal[i],&res)){
//               begin[i]=res.begin;
//               end[i]=res.end;
///*               printf("set_begin_end begin %d end %d\n",begin[i],end[i]);*/
//            }else{
//                return -1;
//            }
//        }else{
//   aa         begin[i]=0;
//  a          end[i]=dims.shape[i]-1;
//        }
//    }
//    return 0;
//}a
int main(int argc, char ** argv){
    if(argc<3){
        printf("Usage: %s file -v varname \"[vmin,vmax]\" -d dname \"(dmin,dmax]\" ...\n",argv[0]);
        exit(1);
    }
   struct timeval abegin,aend;
   gettimeofday(&abegin,NULL);
   double alltime=0;
   double readdimtime=0;
   double processtime=0;
   /* This will be the netCDF ID for the file and data variable. */
   int ncid, varid;
   int retval;
   /* Open the file. NC_NOWRITE tells netCDF we want read-only access
    * to the file.*/
   if ((retval = nc_open(argv[1],NC_NOWRITE, &ncid)))
      ERR(retval);
   int vpos=-1;
   for(int i=2;i<argc;i++){
       if(0==strcmp(argv[i],"-v")){
            if(i+1<argc){
                vpos=i+1;
            }
       }
   }
   if(vpos<0){
      printf("Usage: %s file -v varname \"[vmin,vmax]\" -d dname \"(dmin,dmax]\" ...\n -a means to scan all data",argv[0]);
      exit(1);
   }
   if ((retval = nc_inq_varid(ncid, argv[vpos], &varid)))
      ERR(retval);
    
   double min,max;
   bool min_equal,max_equal;
   struct timeval tbegin,tend,pbegin,pend;
   double readtime=0;
//   sscanf(argv[3],"%lf",&min);
//   sscanf(argv[4],"%lf",&max);
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

    /*parse the input arguments start!*/
    cond conds;
    bool has_dim_condition=false;
    bool has_var_condition=false;
    bool no_tree_index=false;
    bool scan_all=false;
    init_conditions(&conds,dims_size);
    for(i=2;i<argc;i=i+1){
        if(strcmp(argv[i],"-d")==0){
            for(j=0;j<dims_size;j++){
                if(0==strcmp(dnames[j],argv[i+1])){
                    parse_condition(argv[i+2],&(conds.min[j]),&(conds.minequal[j]),&(conds.max[j]),&(conds.maxequal[j]));
                    conds.valid[j]=true;
                    has_dim_condition=true;
                    break;
                }
            }
            i+=2;
        }
        if(strcmp(argv[i],"-v")==0){
            if(strcmp(argv[i+1],argv[vpos])!=0){
                printf("variable %s does not exists in file %s\n",argv[i+1],argv[1]);
            }else{
                if(i+2<argc&&argv[i+2][0]=='['||argv[i+2][0]=='('){
                    parse_condition(argv[i+2],&min,&min_equal,&max,&max_equal);
                    has_var_condition=true;
                    i+=2;
                }else{
                    i++;
                }
            }
        }
        if(strcmp(argv[i],"-a")==0){
            scan_all=true;
        }
    }

   void ** dims_in=(void **)calloc(dims_size,sizeof(void *));
   size_t *start=(size_t *)calloc(dims_size,sizeof(size_t));
   size_t *count=(size_t *)calloc(dims_size,sizeof(size_t));
   size_t *dshape=(size_t *)calloc(dims_size,sizeof(size_t));
   size_t *sizes=(size_t *)calloc(dims_size,sizeof(size_t));
   size_t *idx=(size_t *)calloc(dims_size,sizeof(size_t));
   int var_size=get_nctype_size(vtype);
   gettimeofday(&tbegin,NULL);
   for(i=0;i<dims_size;i++){
       dims_in[i]=(void *)calloc(dsizes[i],get_nctype_size(dtypes[i]));
       nc_get_var(ncid, dimids[i], dims_in[i],dtypes[i]);
       start[i]=0;
       count[i]=dsizes[i];
       sizes[i]=get_nctype_size(dtypes[i]);
   }
   gettimeofday(&tend,NULL);
   readdimtime=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;  

   get_dshape(dshape,dsizes,dims_size);
   size_t hits=0;
   bool check=true;
   if(!scan_all){
      result res;
      size_t asize=1;
      double tmpval;
      for(i=0;i<dims_size;i++){
            if(conds.valid[i]==true){
                if(0<=binary_search((double*)(dims_in[i]),dsizes[i],conds.min[i],conds.max[i],conds.minequal[i],conds.maxequal[i],&res)){
                   start[i]=res.begin;
                   count[i]=res.end-res.begin+1;
                }else{
                    return -1;
                }
            }else{
                start[i]=0;
                count[i]=dsizes[i];
            }
//            printf("%d\n",count[i]);
            asize*=count[i];
      }
      size_t *countdshape=(size_t *)calloc(dims_size,sizeof(size_t));
//      get_dshape(countdshape,count,dims_size);
    
      gettimeofday(&tbegin,NULL);
      void * buff=(void *)calloc(asize,var_size);
      nc_get_vara(ncid,varid,start,count,buff,vtype);
      gettimeofday(&tend,NULL);
      readtime=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;  
//      printf("read time is %lf\n",readtime);
      gettimeofday(&tbegin,NULL);
      if(has_var_condition){

          for(i=0;i<asize;i++){
    //            get_idx(idx,i,countdshape,dims_size);
                check=true;
                    tmpval=((double *)buff)[i];
    //                printf("%lf,%lf,%lf,%lf\n",((double *)dims_in[0])[idx[0]+start[0]],((double *)dims_in[1])[idx[1]+start[1]],((double *)dims_in[2])[idx[2]+start[2]],tmpval);
                    if(min_equal){
                        if(max_equal){
                            if(check&&tmpval<min||tmpval>max){
                                check=false;
                            }
                        }else{
                            if(check&&tmpval<min||tmpval>=max){
                                check=false;
                            }
                        }
                    }else{
                        if(max_equal){
                            if(check&&tmpval<=min||tmpval>max){
                                check=false;
                            }
                        }else{
                            if(check&&tmpval<=min||tmpval>=max){
                                check=false;
                            }
                        }
                    }
                
                if(check){
                    hits++;
                }
            
          }
      }else{
        hits+=asize;
      }
      gettimeofday(&tend,NULL);
      processtime+=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;  
   }else{
       count[0]=1;
       void * buff=(void *)calloc(all_size/dsizes[0],var_size);
       for(i=0;i<dsizes[0];i++){
           start[0]=i;
           gettimeofday(&tbegin,NULL);
           nc_get_vara(ncid,varid,start,count,buff,vtype);
           gettimeofday(&tend,NULL);
           readtime+=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;  
           double tmpval,tmpdim;
           gettimeofday(&tbegin,NULL);
           for(j=0;j<all_size/dsizes[0];j++){
//                get_idx(idx,j,dshape,dims_size);
//                idx[0]+=i;
                check=true;
                if(has_var_condition){
                    tmpval=((double *)buff)[j];
                    if(min_equal){
                        if(max_equal){
                            if(check&&tmpval<min||tmpval>max){
                                check=false;
                            }
                        }else{
                            if(check&&tmpval<min||tmpval>=max){
                                check=false;
                            }
                        }
                    }else{
                        if(max_equal){
                            if(check&&tmpval<=min||tmpval>max){
                                check=false;
                            }
                        }else{
                            if(check&&tmpval<=min||tmpval>=max){
                                check=false;
                            }
                        }
                    }
                }
                if(check&&has_dim_condition){
                    for(int s=0;check&&(s<dims_size);s++){
                        if(conds.valid[s]==false){
                            continue;
                        }
                        tmpdim=((double *)(dims_in[s]))[idx[s]];
                        if(conds.minequal[s]){
                            if(conds.maxequal[s]){
                                if(tmpdim<conds.min[s]||tmpdim>conds.max[s]){
                                    check=false;
                                }
                            }else{
                                if(tmpdim<conds.min[s]||tmpdim>=conds.max[s]){
                                    check=false;
                                }
                            }
                        }else{
                            if(conds.maxequal[s]){
                                if(tmpdim<=conds.min[s]||tmpdim>conds.max[s]){
                                    check=false;
                                }
                            }else{
                                if(tmpdim<=conds.min[s]||tmpdim>=conds.max[s]){
                                    check=false;
                                }
                            }
                        }
                    }
                }
                if(check){
    //            print_row(dims_in,dims_size,idx,dtypes,sizes,buff,j,vtype,var_size);
                    hits++;
                }

    /*            printf("i %d j %d idx[0] %d idx[1] %d idx[2] %d\n",i,j,idx[0],idx[1],idx[2]);*/
    //            print_row(dims_in,dims_size,idx,dtypes,sizes,buff,j,vtype,var_size);
           }
           gettimeofday(&tend,NULL);
           processtime+=tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000;  
       }

   }
//   processtime=pend.tv_sec-pbegin.tv_sec+1.0*(pend.tv_usec-pbegin.tv_usec)/1000000;  
   printf("hits %d\n",hits);
   printf("read time is %lf\n",readtime);
   printf("read dim time is %lf\n",readdimtime);
   printf("process time is %lf\n",processtime);

   if ((retval = nc_close(ncid)))
      ERR(retval);

   gettimeofday(&aend,NULL);
   alltime=aend.tv_sec-abegin.tv_sec+1.0*(aend.tv_usec-abegin.tv_usec)/1000000;  
   printf("all time is %lf\n",alltime);
   return 0;
}
