#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <sys/time.h>
#include "common.h"
#include "rsearch.h"
#include "mapping.h"
using namespace std;
/*typedef enum { false, true } bool;*/
/*#define BLOCK_THRESHOLD 268435456 //256M*/
#define BLOCK_THRESHOLD 22268435456 
/*#define BLOCK_THRESHOLD 1 */
#define BATCH_BUFF_SIZE 16777216 //16M
#define READ_BUFF_SIZE 16777216 //16M
/*#define READ_BUFF_SIZE 8388608 //8M*/
/*#define READ_BUFF_SIZE 262144 //256K*/
/*#define READ_BUFF_SIZE 1*/
/*#define BLOCK_THRESHOLD 1*/
typedef struct condition_t{
    double *min;
    bool *minequal;
    double *max;
    bool *maxequal;
    bool *valid; 
    int size;
}cond;
typedef struct output_cols_t{
    int size;
    bool *cols;
/*    char **cols;*/
    char *varname;
    bool emptycol;
}output_cols;
void init_output_cols(output_cols *ocols,int size){
    ocols->size=size;
    ocols->cols=(bool *)calloc(size,sizeof(bool));
    for(int i=0;i<size;i++){
        ocols->cols[i]=false;
    }
    ocols->varname=NULL;
    ocols->emptycol=true;
};
LAYOUT ly;
/*struct timeval rbegin,rend;*/
/*void init_cond(DIMS dims,){*/
/*    int i;*/
/**/
/*}*/
TYPE get_type(char *name){
   if(!strcmp(name,"byte"))
       return BYTE;
   else if(!strcmp(name,"short"))
       return SHORT;
   else if(!strcmp(name,"int"))
       return INT;
   else if(!strcmp(name,"float"))
       return FLOAT;
   else if(!strcmp(name,"double"))
       return DOUBLE;
   return UNKNOWN;
}
size_t check_index(int *index,int *shape,int size){
    int i;
    size_t tmp=index[0];
    for(i=0;i<size-1;i++){
        tmp=tmp*shape[i+1]+index[i+1];
    }
    return tmp;
}
inline bool check_dim_condition(size_t *idx,size_t *begin,size_t *end,int dims_size){
    int i;
/*    bool res=true;*/
    for(i=0;i<dims_size;i++){
        if(idx[i]>end[i]||idx[i]<begin[i]){
            return false;
        }
    }
    return true;
}
/*inline void get_idx_in_block(size_t *idx,unsigned int offset,size_t * begin,size_t *countdshape,size_t* int dims_size){*/
/*    get_idx_in_block(idx,offset,countdshape,begin,dims_size);*/
/*    size_t count[dims_size];*/
/*    size_t idx[dims_size];*/
/*    int i;*/
/*    get_idx(idx,blockid,newdshape,dims_size);*/
/*    for(i=0;i<dims_size;i++){*/
/*        if(idx[i]!=bound[i]-1)*/
/*            count[i]=shape[i]/bound[i];*/
/*        else*/
/*            count[i]=shape[i]-(shape[i]/bound[i])*(bound[i]-1);*/
/*    }*/
/*    get_dshape(countdshape,count,dims_size);*/
/*    get_index(countdshape);*/

    
/*}*/
inline void get_begin_count_countdshape(size_t *begin,size_t *count,size_t *countdshape,size_t id,size_t *shape,size_t *newdshape,size_t *bound,int dims_size){
/*    size_t count[dims_size];*/
    size_t idx[dims_size];
    int i;
    get_idx(idx,id,newdshape,dims_size);
    
    size_t len;
    for(i=0;i<dims_size;i++){
        len=shape[i]/bound[i];
        begin[i]=idx[i]*len;
        if(idx[i]!=bound[i]-1){
            count[i]=len;
        }else{
            count[i]=shape[i]-len*(bound[i]-1);
        }
    }
    get_dshape(countdshape,count,dims_size);
}
/*size_t validate(size_t *idx,DIMS dims,cond c){*/
/*    int i;*/
/*    for(i=0;i<c.minsize;i++){*/
/*       dims.dimvals[i</c>dx[c.minmap[i]]];*/
        
/*    }*/
/*}*/

/*
 * print the data to a string;
 */
inline void print_to_buf(char *s,TYPE type,void * data){
    switch(type){
        case BYTE:
            sprintf(s,"%c,",*(char *)data);
            break;
        case SHORT:
            sprintf(s,"%d,",*(short *)data);
            break;
        case INT:
            sprintf(s,"%d,",*(int *)data);
            break;
        case LONG:
            sprintf(s,"%ld,",*(long *)data);
            break;
        case FLOAT:
            sprintf(s,"%f,",*(float *)data);
            break;
        case DOUBLE:
            sprintf(s,"%lf,",*(double *)data);
            break;
        defaut:
            printf("unknown type\n");
    }
}
/*
 * get the size for the output row.
 */
int get_row_size(DIMS *dims,int *cols,int cols_size){
    int i;
    int size=0;
    for(i=0;i<cols_size;i++){
        size+=get_type_size(dims->types[cols[i]]);
    }
    size+=get_type_size(dims->var_type);
    return size;
}
void get_offsets(int *offset,int *sizes,DIMS *dims,int *cols,int cols_size){
    if(cols_size==0){
        offset[0]=0;
        sizes[0]=get_type_size(dims->var_type);
        return ;
    }
    int i;
    offset[0]=0;
    sizes[0]=get_type_size(dims->types[cols[0]]);
    for(i=1;i<cols_size;i++){
        sizes[i]=get_type_size(dims->types[cols[i]]);
        offset[i]=offset[i-1]+sizes[i-1]; 
    }
    sizes[cols_size]=get_type_size(dims->var_type);
    offset[cols_size]=offset[cols_size-1]+sizes[cols_size-1];
        
}

/*inline void to_batch_buff(void * buff,size_t *offset,size_t maxsize,void *src, size_t len,FILE *ofp){*/
/*    if(len>maxsize){*/
/*        printf("batch_buff size is %d, but the data size is %d!\n",maxsize,len);*/
/*        return;*/
/*    }*/
/*    if(*offset+len>maxsize){*/
/*        fwrite(buff,*offset,1,ofp);*/
/*        memcpy((char *)buff,src,len);*/
/*        *offset=len;*/
/*    }else{*/
/*        memcpy((char *)buff+*offset,src,len);*/
/*        *offset+=len;*/
/*    }*/
/*}*/
/*void flush_batch_buff(char *buff, size_t *offset,size_t maxsize,FILE *ofp){*/
/*    if(*offset>0){*/
/*        fwrite(buff,*offset,1,ofp);*/
/*    }*/
/*}*/
inline void write_to_buff(DIMS *dims,int *cols, int col_size,size_t *idx,int *typesizes,int *offsets,void *val,int vsize,char * buf,size_t * boffset,FILE *ofp){
    int j=0;
    for(j=0;j<col_size;j++){
        to_batch_buff(buf+offsets[j],boffset,BATCH_BUFF_SIZE,(char*)dims->dimvals[cols[j]]+idx[cols[j]]*typesizes[j],typesizes[j],ofp);
/*        memcpy((char*)(buf+offsets[j]),(char*)dims->dimvals[cols[j]]+idx[cols[j]]*typesizes[j],typesizes[j]);*/
    }
    to_batch_buff(buf+offsets[j],boffset,BATCH_BUFF_SIZE,(char*)val,typesizes[j],ofp);
/*    memcpy((char*)(buf+offsets[j]),val,vsize);*/
}
inline void print_to_buff(DIMS *dims,int *cols, int col_size,size_t *idx,int *typesizes,void *val,char * buf){
    int j=0;
    int buf_pos=0;
    for(j=0;j<col_size;j++){
        print_to_buf(buf+buf_pos,dims->types[cols[j]],(char *)dims->dimvals[cols[j]]+idx[cols[j]]*typesizes[j]); 
        buf_pos=strlen(buf);
    }
    print_to_buf(buf+buf_pos,dims->var_type,val);
}
inline void to_buff(DIMS *dims,int *cols, int col_size,size_t *idx,int *typesizes,int *offsets,void *val,int vsize,char * buf,size_t *boffset, char *rbuf,size_t row_size,  FILE* ofp,MODE m){
    if(m==BINARY){
        write_to_buff(dims,cols,col_size,idx,typesizes,offsets,val,vsize,buf,boffset,ofp);
/*        fwrite(buf,1,row_size,ofp);*/
    }else{
/*        char *rbuff=(char *)calloc(1,row_size);*/
        bzero(rbuf,row_size*10);
        print_to_buff(dims,cols,col_size,idx,typesizes,val,rbuf);
        int len=strlen(rbuf);
        rbuf[len-1]='\n';
        to_batch_buff(buf,boffset,BATCH_BUFF_SIZE,rbuf,len,ofp);
        
/*        fwrite(buf,1,strlen(buf),ofp);*/
    
    }
}
size_t pre_id=0;
size_t post_id=0;
bool first_read=true;
inline void read_from_buff(void *buff,size_t i,block_info * binfo,char *read_buff,FILE *fp,int vsize,size_t window_size,size_t block_num,size_t all_size){
    if(i>post_id||first_read){
        size_t tail;
        size_t len;
        pre_id=i;
        first_read=false;
        if(i+window_size<block_num){
            tail=i+window_size;
            post_id=tail-1;
            len=binfo[tail].boffset-binfo[i].boffset;

        }else{
           tail=block_num; 
           post_id=tail-1;
           len=all_size-binfo[i].boffset;
        }
/*        printf("i %d offset %d window_size %d pre_id %d post_id %d len %d all_size %d\n",i,binfo[i].boffset, window_size,pre_id,post_id,len,all_size);*/
        fseek(fp,binfo[i].boffset*vsize,SEEK_SET);
        fread(read_buff,vsize,len,fp);
/*        fseek(ifp,binfo[i].boffset*isize,SEEK_SET);*/
/*        fread(iread_buff,isize,len,ifp);*/
        *(void **)buff=&read_buff[0];
/*        *(void **)ibuff=&iread_buff[0];*/
        
    }else{
        size_t offset=binfo[i].boffset-binfo[pre_id].boffset;
/*        printf("i %d offset %d not read window_size %d pre_id %d post_id %d\n",i, offset,window_size,pre_id,post_id);*/
/*        *(char **)buff=&read_buff[0]+offset*vsize;*/
/*        *(char **)ibuff=&iread_buff[0]+offset*isize;*/
        *(void **)buff=&read_buff[offset*vsize];
/*        *(void **)ibuff=&iread_buff[offset*isize];*/
/*        printf("address off %d\n",(*(char **)ibuff)-(&iread_buff[0]));*/
    }

}
size_t ipre_id=0;
size_t ipost_id=0;
bool ifirst_read=true;
inline void read_from_ibuff(void *ibuff,size_t i,block_info * binfo,char *iread_buff,FILE *ifp,int isize,size_t iwindow_size,size_t block_num,size_t all_size){
    if(i>ipost_id||ifirst_read){
        size_t tail;
        size_t len;
        ipre_id=i;
        ifirst_read=false;
        if(i+iwindow_size<block_num){
            tail=i+iwindow_size;
            ipost_id=tail-1;
            len=binfo[tail].boffset-binfo[i].boffset;

        }else{
           tail=block_num; 
           ipost_id=tail-1;
           len=all_size-binfo[i].boffset;
        }
        fseek(ifp,binfo[i].boffset*isize,SEEK_SET);
        fread(iread_buff,isize,len,ifp);
        *(void **)ibuff=&iread_buff[0];
        
    }else{
        size_t offset=binfo[i].boffset-binfo[ipre_id].boffset;
        *(void **)ibuff=&iread_buff[offset*isize];
    }

}

int block_query(std::set<int> &dblocks,size_t*begins, size_t*ends,size_t *shape,size_t *bound,int dims_size){

    int head[dims_size];  
    size_t newshape[dims_size];
    size_t newdshape[dims_size];
    size_t count[dims_size];
    size_t countdshape[dims_size];
    size_t idx[dims_size];
    bool headfull[dims_size];
    bool tailfull[dims_size];
    int len[dims_size];
    int i,j;
    get_new_shape(newshape,bound,shape,dims_size);
    get_dshape(newdshape,newshape,dims_size);

    for(i=0;i<dims_size;i++){
        len[i]=shape[i]/bound[i];
        head[i]=begins[i]/len[i];
        count[i]=ends[i]/len[i]-head[i]+1;
/*        if(begins[i]%len[i]==0){*/
/*            headfull[i]=true;*/
/*        }else{*/
/*            headfull[i]=false;*/
/*        }*/
/*        if(ends[i]%bound[i]==len[i]-1||ends[i]==shape[i]-1){*/
/*            tailfull[i]=true;*/
/*        }else{*/
/*            tailfull[i]=false;*/
/*        }*/
    }
    int all_size=1;
    for(i=0;i<dims_size;i++){
        all_size*=count[i];
    }
/*    printf("dquery size %d\n",all_size);*/
    get_dshape(countdshape,count,dims_size);
    int pos;
    U_int *g_mask=(U_int *)calloc(dims_size,sizeof(U_int));
    for(i=0;i<dims_size;i++){
        g_mask[i]=1<<dims_size-1-i;
    }
    Hcode h;
    h.hcode=(U_int *)calloc(dims_size,sizeof(U_int));
    Point pt;
    pt.hcode=(U_int *)calloc(dims_size,sizeof(U_int));
    
    for(i=0;i<all_size;i++){
       get_idx(idx,i,countdshape,dims_size);
       for(j=0;j<dims_size;j++){
           idx[j]+=head[j];
       }
       if(ly==HCURVE){
           bzero(h.hcode,sizeof(U_int)*dims_size);
           bzero(pt.hcode,sizeof(U_int)*dims_size);
           for(j=0;j<dims_size;j++){
               pt.hcode[j]=idx[j];
           }
           /*encode Hilbert curve position*/
           H_encode(h,pt,dims_size,g_mask);
           pos=h.hcode[0];
       }else{
           pos=get_index(idx,newdshape,dims_size);
       }
       dblocks.insert(pos);
/*       printf("dquery pos %d\n",pos);*/
    }
    free(h.hcode);
    free(pt.hcode);
    free(g_mask);
    return 0;
}
void print_res(FILE *fp){
    int i=0;
    double data; 
    while(!feof(fp)){
        if(fread(&data,sizeof(double),1,fp)>0){
            i++;
            printf("%lf,",data);
            if(i%3==0){ 
                printf("\n");
            }
        }
    };
    printf("count %d\n",i); 
}
inline int compare(const void *a,const void *b){
   double res=(*(node*)a).val-(*(node*)b).val;
   if(res>0) return 1;
   if(res<0) return -1;
   if(res==0) return 0;
}



/*
 * read a value from a file using fseek
*/
inline void read_from_file_with_offset(FILE * fp,size_t offset,size_t size,size_t pos,double *data){
        fseek(fp,offset+size*pos,SEEK_SET);
        fread(data,size,1,fp);
/*        printf("%f %d\n",data->val,data->repeat);*/
}



/*
 * binary search for left bound using fseek
*/
inline size_t flsearch_with_offset(FILE * fp,size_t offset,size_t size,size_t len,double val,bool equal){
/*    printf("lsearch\n");*/
    int lp=0;
    int rp=len-1;
    int mid;
    double data;
    while(rp>lp){
        mid=rp-((rp-lp)>>1);
/*        printf("mid %d\n",mid);*/
        read_from_file_with_offset(fp,offset,size,mid,&data);
        if(data>=val)
            rp=mid-1;
        else
            lp=mid;
/*        printf("lsearch %d %d %d\n",lp,mid ,rp);*/
    }
/*    printf("rp %d val %lf\n",rp,data[rp].val);*/
    read_from_file_with_offset(fp,offset,size,lp,&data);
    double tmp=data;
    if(lp==0){
        if(equal&&tmp==val){
            return 0;
        }
        if(val<tmp)
            return 0;
    }
    int i;
/*    printf("%f\n",data[lp]);*/
    for(i=lp+1;i<len;i++){
        read_from_file_with_offset(fp,offset,size,i,&data);
        if(data>tmp){
            if(data==val){
               if(equal){
                    return i;
               }else{
                    tmp=data;
                    while(i<len){
                        read_from_file_with_offset(fp,offset,size,i,&data);
                        if(data>tmp){
                            return i;
                        }
                        i++;
                    }
                    return -1;
               }
            }else{
                return i;
            }
        }
    }
    return -1;
}

/*
 * binary search for right bound using fseek
*/
inline size_t frsearch_with_offset(FILE * fp,size_t offset,size_t size,size_t len,double val,bool equal){
/*    printf("rsearch\n");*/
    int lp=0;
    int rp=len-1;
    int mid;
    double data;
    while(rp>lp){
        mid=lp+((rp-lp)>>1);
        read_from_file_with_offset(fp,offset,size,mid,&data);
        if(data>val)
            rp=mid;
        else
            lp=mid+1;
    }
    read_from_file_with_offset(fp,offset,size,rp,&data);
    double tmp=data;
    if(rp==len-1){
        if(equal&&tmp==val){
            return rp;
        }
        if(val>tmp){
            return rp;
        }
    }
    int i;
    for(i=rp-1;i>=0;i--){
        read_from_file_with_offset(fp,offset,size,i,&data);
        if(data<tmp){
            if(data==val){
               if(equal){
                    return i;
               }else{
                    tmp=data;
                    while(i>=0){
                        read_from_file_with_offset(fp,offset,size,i,&data);
                        if(data<tmp){
                            return i;
                        }
                        i--;
                    }
                    return -1;
               }
            }else{
                return i;
            }
        }
    }
    return -1;
}

/*
 * binary search using fseek
 */
inline int fbsearch(FILE * fp,size_t offset,size_t size,size_t len,double min,double max,bool min_equal,bool max_equal,result *res){
   if( min>max||(min==max)&&(min_equal!=true||max_equal!=true)){
      return -1; 
   }
   struct timeval tbegin,tend;
   gettimeofday(&tbegin,NULL);
   res->begin=flsearch_with_offset(fp,offset,size,len,min,min_equal);
   res->end=frsearch_with_offset(fp,offset,size,len,max,max_equal);
   gettimeofday(&tend,NULL);
   if(res->begin!=-1&&res->end!=-1&&res->end>=res->begin){
/*       double data; */
/*       read_from_file_with_offset(fp,offset,sizeof(double),cres->begin,&data);*/
/*       res->begin=data.idx;*/
/*       read_from_file_with_offset(fp,offset,sizeof(double),cres->end,&data);*/
/*       res->end=data.idx+data.repeat-1;*/
/*       printf("hit number:%ld\n",res->end-res->begin+1);*/
/*       printf("begin %lf %lf end %lf %lf\n",data[res->begin-1].val,data[res->begin].val,data[res->end].val,data[res->end+1].val);*/
/*       printf("bsearch time:%f\n",tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000);*/
       return 0;
   }
   return -1;
}
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
int set_begin_end(size_t  *begin, size_t *end,DIMS &dims,cond &conds){
    int i;
    result res;
    for(i=0;i<dims.dims_size;i++){
        if(conds.valid[i]==true){
            if(0<=binary_search((double*)(dims.dimvals[i]),dims.shape[i],conds.min[i],conds.max[i],conds.minequal[i],conds.maxequal[i],&res)){
               begin[i]=res.begin;
               end[i]=res.end;
/*               printf("set_begin_end begin %d end %d\n",begin[i],end[i]);*/
            }else{
                return -1;
            }
        }else{
            begin[i]=0;
            end[i]=dims.shape[i]-1;
        }
    }
    return 0;
}

int main(int argc,char ** argv){
    struct timeval tbegin, tend;
    struct timeval read_tbegin,read_tend;
    double readtime=0;
    double decodetime=0;
    double indextime=0;
    double rsearchtime=0;
    double bsearchtime=0;
    gettimeofday(&tbegin,NULL);
/*    int X_LIMIT;*/
/*    sscanf(argv[1],"%d",&X_LIMIT);*/
    double min=DBL_MIN;
    double max=DBL_MAX;
    bool min_equal=true;
    bool max_equal=true;
/*    char lb,rb;*/
/*    printf("%s\n",argv[2]);*/
/*    sscanf(argv[2],"%c%lf,%lf%c",&lb,&min,&max,&rb);*/
/*    if(lb=='('){*/
/*        min_equal=false;*/
/*    }*/
/*    if(rb==')'){*/
/*        max_equal=false;*/
/*    }*/

/*    node* data=(node*)calloc(sizeof(node),X_LIMIT*Y*Z);*/
    if(argc<3){
        printf("Usage :%s array_file -d dim_name \"[dmin,dmax]\" -v var_name \"[vmin,vmax]\" -o dim1,dim2,varname output_file\n",argv[0]);
        printf("\tYou can give zero or several dimension ranges\n");
        printf("\tYou can use %s array_file *. to scan all the array\n",argv[0]);
        printf("\tIf -o and output_file is not specified, then the program only print the hit number\n");
        exit(1);
    }

    FILE *fp=fopen(argv[1],"r");
    char ifilename[128]={0};
    sprintf(ifilename,"%s_bidx",argv[1]);
    FILE *ifp=fopen(ifilename,"r");
    sprintf(ifilename,"%s_meta",argv[1]);
    FILE *mfp=fopen(ifilename,"r");
    sprintf(ifilename,"%s_binfo",argv[1]);
    FILE *bfp=fopen(ifilename,"r");
   
    /* parse the bmeta file start!*/ 
    size_t i,j;
    char line[512]={0};
    int dims_size;
    fgets(line,sizeof(line),mfp);
    sscanf(line,"Dimension size=%d",&dims_size);
    char varname[128]={0};
    char vtypename[128]={0};
    char tempname[128]={0};
    char outputname[256]={0};
    char **dnames=(char **)calloc(dims_size,sizeof(char *));
    size_t *shape=(size_t *)calloc(dims_size,sizeof(size_t));
    size_t *bound=(size_t *)calloc(dims_size,sizeof(size_t));
    TYPE *types=(TYPE *)calloc(dims_size,sizeof(TYPE));
    TYPE var_type;
    fgets(line,sizeof(line),mfp);
    for(i=0;i<dims_size;i++){
        memset(line,0,sizeof(line));
        fgets(line,sizeof(line),mfp);
        dnames[i]=(char *)calloc(128,sizeof(char *));
        sscanf(line,"%s\t%s\t%d\t%d",dnames[i],tempname,&shape[i],&bound[i]);
        types[i]=get_type(tempname);
        memset(tempname,0,128);
    }
    fgets(line,sizeof(line),mfp);
    memset(line,0,sizeof(line));
    fgets(line,sizeof(line),mfp);
    sscanf(line,"%s\t%s",varname,vtypename);
    var_type=get_type(vtypename);
    memset(line,0,sizeof(line));
    fgets(line,sizeof(line),mfp);
    sscanf(line,"Block Arrangement=%s",tempname);
    if(!strcmp(tempname,"linear")){
        ly=LINEAR;
    }else if(!strcmp(tempname,"hcurve")){
        ly=HCURVE;
    }
    MODE m=BINARY;
    /* parse the bmeta file end!*/ 

    /*parse the input arguments start!*/
    cond conds;
    output_cols ocols;
    bool has_dim_condition=false;
    bool has_var_condition=false;
    bool need_output=false;
    bool need_dims=false;
    bool no_tree_index=false;
    init_conditions(&conds,dims_size);
    init_output_cols(&ocols,dims_size);
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
            if(strcmp(argv[i+1],varname)!=0){
                printf("variable %s doesnot exists in file %s\n",argv[i+1],argv[1]);
            }else{
                parse_condition(argv[i+2],&min,&min_equal,&max,&max_equal);
                has_var_condition=true;
            }
            i+=2;
        }
        if(strcmp(argv[i],"-o")==0){
            if(i+2<argc){
                char *p;
                char *cstr=NULL;
                cstr=(char *)calloc(1,strlen(argv[i+1])+1);
                strcpy(cstr,argv[i+1]);
                p = strtok(cstr,",");
                while(p!=NULL)
                {
/*                    res.push_back(p);*/
                    if(strcmp(p,varname)==0){
                        ocols.varname=(char *)calloc(1,strlen(varname)+1);
                        strcpy(ocols.varname,varname);
                    }else{
                        for(int s=0;s<dims_size;s++){
                           if(strcmp(dnames[s],p)==0){
                              ocols.cols[s]=true;  
                              ocols.emptycol=false;
                           }  
                        }
                    }
                    p = strtok(NULL,",");
                }
                if(cstr!=NULL)
                    free(cstr);
                strcpy(outputname,argv[i+2]);
            if((ocols.emptycol==false||ocols.varname!=NULL)&&strlen(outputname)>0)
                need_output=true;
            }
        }
        if(strcmp(argv[i],"-nt")==0){
           no_tree_index=true; 
        }
        if(strcmp(argv[i],"-mt")==0){
            m=TEXT;
        }
    }
    if(has_dim_condition||ocols.emptycol==false){
       need_dims=true; 
    }
    /*parse the input arguments start!*/

/*   parse_condition(argv[2],&min,&min_equal,&max,&max_equal); */
    size_t *newshape = (size_t *)calloc(dims_size,sizeof(size_t));
    size_t *newdshape = (size_t *)calloc(dims_size,sizeof(size_t));
    size_t *dshape = (size_t *)calloc(dims_size,sizeof(size_t));
    get_new_shape(newshape,bound,shape,dims_size);
    get_dshape(newdshape,newshape,dims_size);
    get_dshape(dshape,shape,dims_size);
    size_t block_num=1;
    size_t all_size=1;
    for(i=0;i<dims_size;i++){
        block_num*=newshape[i];
        all_size*=shape[i];
    }

    block_info *binfo=(block_info*)calloc(block_num,sizeof(block_info));
    fread(binfo,sizeof(block_info),block_num,bfp);
/*    for(i=0;i<8;i++){*/
/*        vns[i].min=i;*/
/*        vns[i].max=i+1;*/
/*        vns[i].val=i;*/
/*    }*/
/*    init_rnodes(rnodes,0,0,512,0,vns,8,max_level);*/
    std::set<int> *vset=NULL;
    std::set<int> *dset=NULL;
    std::set<int> *fset=NULL;
    if(has_var_condition){
        gettimeofday(&read_tbegin,NULL);
        if(!no_tree_index){
            vnode vns[block_num];
        /*    vnode *vns=(vnode*)calloc(block_num,sizeof(vnode));*/
            double gmin=DBL_MAX,gmax=DBL_MIN;
            for(i=0;i<block_num;i++){
                if(binfo[i].min<gmin)
                    gmin=binfo[i].min;
                if(binfo[i].max>gmax)
                    gmax=binfo[i].max;
                vns[i].min=binfo[i].min;
                vns[i].max=binfo[i].max;
                vns[i].val=i;
        /*        printf("block_id %d min %lf max %lf\n",i,binfo[i].min,binfo[i].max);*/
            }  
            int max_level=10;
            int rnodes_size=get_tree_size(max_level);
        /*    printf("max_level %d rnodes_size %d\n",max_level,rnodes_size);*/
            rnode rnodes[rnodes_size];
            init_rnodes(rnodes,0,gmin,gmax,0,vns,block_num,max_level);

            vset=new std::set<int>();
            
            struct timeval tmpbegin, tmpend;
            gettimeofday(&tmpbegin,NULL);
            rquery(*vset,min,max,rnodes,0,0,max_level);
            gettimeofday(&tmpend,NULL);
            
            rsearchtime+=tmpend.tv_sec-tmpbegin.tv_sec+1.0*(tmpend.tv_usec-tmpbegin.tv_usec)/1000000;
            printf("vset size %d\n",(*vset).size());
        }else{
           vset=new std::set<int>();
           for(i=0;i<block_num;i++){
               if(!(binfo[i].min>max||binfo[i].max<min)){
                   vset->insert(i);
               }
           }
           printf("vset size %d\n",(*vset).size());
        }
        gettimeofday(&read_tend,NULL);
        indextime+=read_tend.tv_sec-read_tbegin.tv_sec+1.0*(read_tend.tv_usec-read_tbegin.tv_usec)/1000000;
    }

    size_t *dbegins=(size_t *)calloc(dims_size,sizeof(size_t));
    size_t *dends=(size_t *)calloc(dims_size,sizeof(size_t));
    DIMS dims;
    FILE **fps;
    char tname[256]={0};
    if(need_dims){
        fps=(FILE **)calloc(dims_size,sizeof(FILE*));
        for(i=0;i<dims_size;i++){
            fps[i]=fopen(dnames[i],"r");
            bzero(tname,sizeof(tname));
            sprintf(tname,"%s.%s",argv[1],dnames[i]);
            fps[i]=fopen(tname,"r");
/*            fps[i]=fopen(dnames[i],"r");*/
        }
        init_dims(&dims,dims_size,shape,types,var_type,fps);
        for(i=0;i<dims_size;i++){
            fclose(fps[i]);
        }
    }else{
        init_dims(&dims,dims_size,shape,types,var_type,NULL);
    }
    if(has_dim_condition){
        set_begin_end(dbegins,dends,dims,conds);
        printf("dbegins %d %d %d\n",dbegins[0],dbegins[1],dbegins[2]);
        printf("dends %d %d %d\n",dends[0],dends[1],dends[2]);
/*        for(i=0;i<dims_size;i++){*/
/*            printf("%d %d\n",dbegins[i],dends[i]);*/
/*        }*/
        dset=new std::set<int>();
        
        gettimeofday(&read_tbegin,NULL);
        block_query(*dset,dbegins,dends,shape,bound,dims_size);
        gettimeofday(&read_tend,NULL);
        decodetime+=read_tend.tv_sec-read_tbegin.tv_sec+1.0*(read_tend.tv_usec-read_tbegin.tv_usec)/1000000;
        printf("dset size %d\n",(*dset).size());
    }
    if(vset==NULL&&dset==NULL){
/*        printf("argv[2] %s\n",argv[0]);*/
        if(strcmp(argv[2],"-a")==0){
            dset=new std::set<int>();
            for(i=0;i<dims_size;i++){
                dbegins[i]=0;
                dends[i]=shape[i]-1;
            }
            for(i=0;i<block_num;i++){
                dset->insert(i);
            }
            fset=dset;
            /*to do scan all data*/
/*            set_begin_end(dbegins,dends,dims,conds);*/
        }else{
            printf("Please specify a condition or use %s %s -a to scan all data\n",argv[0],argv[1]);
            return 0;
        }
    }else if(vset==NULL){
        fset=dset;
    }else if(dset==NULL){
        fset=vset;
    }else{
        fset=new std::set<int>();
        set_intersection(vset->begin(), vset->end(), dset->begin(), dset->end(), inserter(*fset, fset->begin()));
        printf("fset size %d\n",(*fset).size());
    }
    size_t block_size=get_max_block_size(bound,shape,dims_size);
    double *buff=(double *)calloc(block_size,sizeof(double)); 
    unsigned int *ibuff=(unsigned int *)calloc(block_size,sizeof(unsigned int)); 
    size_t idx[dims_size];
/*    size_t iidx[dims_size];*/
    size_t count[dims_size];
    size_t countdshape[dims_size];
    size_t offs[dims_size];
    result res;
    size_t len;
    size_t hits=0;
    int vsize=get_type_size(var_type);// for value
    int isize=sizeof(unsigned int); // for index value
    int label=0;
    int pre=0;
    int retval;
    size_t avg_block_size=vsize*get_block_size(bound,shape,dims_size);
    printf("avg_block_size %ld\n",avg_block_size);
    if(avg_block_size<=BLOCK_THRESHOLD){
        printf("WITHIN BLOCK_THRESHOLD\n");
    }else{
        printf("BEYOND BLOCK_THRESHOLD\n");
    
    }
    char *read_buff=(char *)calloc(READ_BUFF_SIZE,sizeof(char));
    char *iread_buff=(char *)calloc(READ_BUFF_SIZE,sizeof(char));
    int window_size=READ_BUFF_SIZE/(block_size*vsize);
    int iwindow_size=READ_BUFF_SIZE/(block_size*isize);
    if(window_size==0||iwindow_size==0){
        printf("small window size\n");
    }
    U_int *g_mask=(U_int *)calloc(dims_size,sizeof(U_int));
    for(i=0;i<dims_size;i++){
        g_mask[i]=1<<dims_size-1-i;
    }
    Hcode h;
    Point pt;
    h.hcode=(U_int *)calloc(dims_size,sizeof(U_int));
    pt.hcode=(U_int *)calloc(dims_size,sizeof(U_int));
/*    window_size=1;*/
    /*init output_pos more to do*/
/*    MODE m=TEXT;*/
/*    MODE m=BINARY;*/
    FILE *ofp=fopen(outputname,"w");
    int dsizes[dims_size];
    int cols[dims_size];
    int typesizes[dims_size];
    int offsets[dims_size];
    int col_size=0;
    int row_size=0,row_buf_size;
    char *row_buff;
    char *batch_buff;
    size_t batch_offset=0;
    if(need_output){
        for(i=0;i<dims_size;i++){
            if(ocols.cols[i]==true){
                cols[col_size++]=i;
            }
        }
/*        for(i=0;i<dims_size;i++){*/
/*            cols[i]=i;      */
/*        }*/
        get_offsets(offsets,typesizes,&dims,cols,col_size);
        row_size=get_row_size(&dims,cols,col_size);
        row_buf_size=row_size*10;
        row_buff=(char *)calloc(row_buf_size,sizeof(char));
        batch_buff=(char *)calloc(BATCH_BUFF_SIZE,sizeof(char));
    }
    int hpos;
    for(std::set<int>::iterator iter=fset->begin();iter!=fset->end();iter++){
        gettimeofday(&read_tbegin,NULL);
        if(ly==HCURVE){
            bzero(h.hcode,sizeof(U_int)*dims_size);
            bzero(pt.hcode,sizeof(U_int)*dims_size);
            h.hcode[0]=*iter;
            H_decode(pt,h,dims_size,g_mask);
            for(j=0;j<dims_size;j++){
                idx[j]=pt.hcode[j];
            }
            i=*iter;
            hpos=get_index(idx,newdshape,dims_size);
        }else{
            i=*iter;
        }
        gettimeofday(&read_tend,NULL);
        decodetime+=read_tend.tv_sec-read_tbegin.tv_sec+1.0*(read_tend.tv_usec-read_tbegin.tv_usec)/1000000;

        if(i!=block_num-1){
            len=binfo[i+1].boffset-binfo[i].boffset;
        }else{
            len=all_size-binfo[i].boffset;
        }
        if(vset!=NULL){
            /* read block data*/
            if(avg_block_size<=BLOCK_THRESHOLD){
                gettimeofday(&read_tbegin,NULL);
                if(window_size>0){
                    read_from_buff(&buff,i,binfo,read_buff,fp,vsize,window_size,block_num,all_size);
                    if(need_dims){
                        read_from_ibuff(&ibuff,i,binfo,iread_buff,ifp,isize,iwindow_size,block_num,all_size);
                    }
                }else{
                    if(label!=0){
                        if(i!=pre+1){
                            fseek(fp,binfo[i].boffset*vsize,SEEK_SET);
                            fread(buff,vsize,len,fp);
                            if(need_dims){
                                fseek(ifp,binfo[i].boffset*isize,SEEK_SET);
                                fread(ibuff,isize,len,ifp);
                            }
                        }else{
                            fread(buff,vsize,len,fp);
                            if(need_dims){
                                fread(ibuff,isize,len,ifp);
                            }
                        }
                        pre=i;
                    }else{
                        fseek(fp,binfo[i].boffset*vsize,SEEK_SET);
                        fread(buff,vsize,len,fp);
                        if(need_dims){
                            fseek(ifp,binfo[i].boffset*isize,SEEK_SET);
                            fread(ibuff,isize,len,ifp);
                        }
                        pre=i; 
                    }
                }
/*                read_from_buff(buff,ibuff,i,block_info * binfo,max_window,read_buff,iread_buff,fp,ifp);*/
                gettimeofday(&read_tend,NULL);
                readtime+=read_tend.tv_sec-read_tbegin.tv_sec+1.0*(read_tend.tv_usec-read_tbegin.tv_usec)/1000000;
                gettimeofday(&read_tbegin,NULL);
                retval=binary_search(buff,len,min,max,min_equal,max_equal,&res);
/*                printf("begin %d, end %d\n",res.begin,res.end);*/
                gettimeofday(&read_tend,NULL);
                bsearchtime+=read_tend.tv_sec-read_tbegin.tv_sec+1.0*(read_tend.tv_usec-read_tbegin.tv_usec)/1000000;
            }else{
                //for large blocks, do bsearch with fseek
                gettimeofday(&read_tbegin,NULL);
                retval=fbsearch(fp,binfo[i].boffset*vsize,vsize,len,min,max,min_equal,max_equal,&res);
                gettimeofday(&read_tend,NULL);
                bsearchtime+=read_tend.tv_sec-read_tbegin.tv_sec+1.0*(read_tend.tv_usec-read_tbegin.tv_usec)/1000000;
                
            }
            if(dset==NULL){
                if(retval>=0){
/*                    gettimeofday(&read_tbegin,NULL);*/
/*                        for(j=0;j<len;j++){*/
/*                            if(buff[j]>=min&&buff[j]<=max){*/
/*                                hits++;*/
/*                            }*/
/*                            if((buff[j]==min&&!min_equal)||buff[j]==max&&!max_equal){*/
/*                                hits--;*/
/*                            }*/
/*                        }*/
/*                    gettimeofday(&read_tend,NULL);*/
/*                    bsearchtime+=read_tend.tv_sec-read_tbegin.tv_sec+1.0*(read_tend.tv_usec-read_tbegin.tv_usec)/1000000;*/
                    hits+=res.end-res.begin+1;
/*                        gettimeofday(&read_tbegin,NULL);*/
/*                        fseek(fp,(binfo[i].boffset+res.begin)*vsize,SEEK_SET);*/
/*                        fread(buff+res.begin,vsize,res.end-res.begin+1,fp);*/
/*                        fseek(ifp,(binfo[i].boffset+res.begin)*isize,SEEK_SET);*/
/*                        fread(ibuff+res.begin,isize,res.end-res.begin+1,ifp);*/
/*                        gettimeofday(&read_tend,NULL);*/
/*                        readtime+=read_tend.tv_sec-read_tbegin.tv_sec+1.0*(read_tend.tv_usec-read_tbegin.tv_usec)/1000000;*/
                    if(need_output){
                        get_begin_count_countdshape(offs,count,countdshape,i,shape,newdshape,bound,dims_size);
                        gettimeofday(&read_tbegin,NULL);
                        if(avg_block_size<=BLOCK_THRESHOLD){

                        }else{
                            fseek(fp,(binfo[i].boffset+res.begin)*vsize,SEEK_SET);
                            fread(buff+res.begin,vsize,res.end-res.begin+1,fp);
                            if(need_dims){
                                fseek(ifp,(binfo[i].boffset+res.begin)*isize,SEEK_SET);
                                fread(ibuff+res.begin,isize,res.end-res.begin+1,ifp);
                            }
                        }
                        gettimeofday(&read_tend,NULL);
                        readtime+=read_tend.tv_sec-read_tbegin.tv_sec+1.0*(read_tend.tv_usec-read_tbegin.tv_usec)/1000000;
                        for(j=0;j<res.end-res.begin+1;j++){
                            get_idx_in_block(idx,ibuff[j+res.begin],countdshape,offs,dims_size);
                            to_buff(&dims,cols, col_size,idx,typesizes,offsets,&(buff[res.begin+j]),vsize,batch_buff,&batch_offset,row_buff,row_size,ofp,m);
                        }
                    }
                }
            }else{
                if(retval>=0){
                    if(ly==HCURVE){
                        get_begin_count_countdshape(offs,count,countdshape,hpos,shape,newdshape,bound,dims_size);
                    }else{
                        get_begin_count_countdshape(offs,count,countdshape,i,shape,newdshape,bound,dims_size);
                    }
                    gettimeofday(&read_tbegin,NULL);
                    if(avg_block_size<=BLOCK_THRESHOLD){

                    }else{
                        fseek(fp,(binfo[i].boffset+res.begin)*vsize,SEEK_SET);
                        fread(buff+res.begin,vsize,res.end-res.begin+1,fp);
                        if(need_dims){
                            fseek(ifp,(binfo[i].boffset+res.begin)*isize,SEEK_SET);
                            fread(ibuff+res.begin,isize,res.end-res.begin+1,ifp);
                        }
                    }
                    gettimeofday(&read_tend,NULL);
                    readtime+=read_tend.tv_sec-read_tbegin.tv_sec+1.0*(read_tend.tv_usec-read_tbegin.tv_usec)/1000000;
                    for(j=0;j<res.end-res.begin+1;j++){
                        get_idx_in_block(idx,ibuff[j+res.begin],countdshape,offs,dims_size);
                        if(check_dim_condition(idx,dbegins,dends,dims_size)){
                            if(need_output){
                                to_buff(&dims,cols, col_size,idx,typesizes,offsets,&(buff[res.begin+j]),vsize,batch_buff,&batch_offset,row_buff,row_size,ofp,m);
                            }
                            hits++;
                        }
                    }

                }
            }
        }else{ //only with dimensional conditions
            gettimeofday(&read_tbegin,NULL);
            if(window_size>0){
                read_from_buff(&buff,i,binfo,read_buff,fp,vsize,window_size,block_num,all_size);
                read_from_ibuff(&ibuff,i,binfo,iread_buff,ifp,isize,iwindow_size,block_num,all_size);
            }else{
                if(label!=0){
                    if(i!=pre+1){
                        fseek(fp,binfo[i].boffset*vsize,SEEK_SET);
                        fread(buff,vsize,len,fp);
                        fseek(ifp,binfo[i].boffset*isize,SEEK_SET);
                        fread(ibuff,isize,len,ifp);
                    }else{
                        fread(buff,vsize,len,fp);
                        fread(ibuff,isize,len,ifp);
                    }
                    pre=i;
                }else{
                    fseek(fp,binfo[i].boffset*vsize,SEEK_SET);
                    fread(buff,vsize,len,fp);
                    fseek(ifp,binfo[i].boffset*isize,SEEK_SET);
                    fread(ibuff,isize,len,ifp);
                    pre=i; 
                }
            }
            gettimeofday(&read_tend,NULL);
            readtime+=read_tend.tv_sec-read_tbegin.tv_sec+1.0*(read_tend.tv_usec-read_tbegin.tv_usec)/1000000;
            get_begin_count_countdshape(offs,count,countdshape,i,shape,newdshape,bound,dims_size);
        
            bool contained=true;
            for(j=0;j<dims_size;j++){
                if(!(dbegins[j]<=offs[j]&&dends[j]>=(offs[j]+count[j]))){
                    contained=false;
                    break;
                }
            }
            if(contained){
/*                hits+=count[0]*countdshape[dims_size-1];*/
                hits+=len;
                if(need_output){
                    for(j=0;j<len;j++){
                        get_idx_in_block(idx,ibuff[j],countdshape,offs,dims_size);
                        to_buff(&dims,cols, col_size,idx,typesizes,offsets,&(buff[j]),vsize,batch_buff,&batch_offset,row_buff,row_size,ofp,m);
                    }
                }
/*                printf("contained %d\n",count[0]*countdshape[dims_size-1]);*/
            }else{
                for(j=0;j<len;j++){
                    get_idx_in_block(idx,ibuff[j],countdshape,offs,dims_size);
                    if(check_dim_condition(idx,dbegins,dends,dims_size)){
                        if(need_output){
                            to_buff(&dims,cols, col_size,idx,typesizes,offsets,&(buff[j]),vsize,batch_buff,&batch_offset,row_buff,row_size,ofp,m);
                        }
                        hits++;
                    }
                }
            }
        }
        label++;

    }
/*    int phits=1;*/
/*    for(i=0;i<dims_size;i++){*/
/*        phits=phits*(dends[i]-dbegins[i]+1);*/
/*    }*/
    printf("hits %d\n",hits);
    if(need_output){
        flush_batch_buff(batch_buff, &batch_offset,BATCH_BUFF_SIZE,ofp);
        fclose(ofp);
    }
    

/*    gettimeofday(&read_tbegin,NULL);*/
/*    gettimeofday(&read_tend,NULL);*/
/*    if(argc>=5)*/
/*        fclose(ofp);*/
/*    destory_dims(&dims);*/
    fclose(fp);
    fclose(ifp);
    fclose(mfp);
/*    ofp=fopen(argv[4],"r");*/
/*    print_res(ofp);*/
/*    fclose(ofp);*/
    gettimeofday(&tend,NULL);
/*    printf("all time is %fs and scan time is %fs\n",tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000,read_tend.tv_sec-read_tbegin.tv_sec+1.0*(read_tend.tv_usec-read_tbegin.tv_usec)/1000000);*/
    printf("all time is %lfs\nread time is %lfs\ntree index time is %lfs\nrsearch time is %lfs\ndecode time is %lfs\nbsearch time is %lfs\n\n",tend.tv_sec-tbegin.tv_sec+1.0*(tend.tv_usec-tbegin.tv_usec)/1000000,readtime,indextime,rsearchtime, decodetime,bsearchtime);
    return 0;
}
