#include <stdio.h>
#include <stdlib.h>
#include <snappy-c.h>
void compress(char *infile,char*outfile){
    FILE *ifp=fopen(infile,"r");
    FILE *ofp=fopen(outfile,"w");
    fseek(ifp,0,SEEK_END);
    size_t len=ftell(ifp);
    char *buff=(char *)calloc(len,1);
    char *cbuff=(char *)calloc(len,1);
    size_t clen;    
    fseek(ifp,0,SEEK_SET);
    fread(buff,len,1,ifp);
    snappy_compress(buff,len,cbuff,&clen);
    fwrite(cbuff,clen,1,ofp);
    free(buff);
    free(cbuff);
    fclose(ifp);
    fclose(ofp);
}
void decompress(char *infile,char*outfile){
    FILE *ifp=fopen(infile,"r");
    FILE *ofp=fopen(outfile,"w");
    fseek(ifp,0,SEEK_END);
    size_t clen=ftell(ifp);
    char *cbuff=(char *)calloc(clen,1);
    size_t len;    
    fseek(ifp,0,SEEK_SET);
    fread(cbuff,clen,1,ifp);
    snappy_uncompressed_length(cbuff,clen,&len);
/*    printf("len %d\n",len);*/
    char *buff=(char *)calloc(len,1);
    if(snappy_uncompress(cbuff,clen,buff,&len)!=SNAPPY_OK)
        printf("uncompressed failed!\n");
    fwrite(buff,len,1,ofp);
    free(buff);
    free(cbuff);
    fclose(ifp);
    fclose(ofp);
    
}
int main(int argc, char *argv[]){
   if(argc>=4){
      char op;
      sscanf(argv[1],"-%c",&op);
/*      printf("%c\n",op);*/
      if(op=='c'){
        compress(argv[2],argv[3]);
      }else if(op=='d'){
         decompress(argv[2],argv[3]);
      }
   }
}
