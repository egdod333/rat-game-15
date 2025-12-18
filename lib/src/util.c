#include <stdio.h>
#include <stdlib.h>
FILE* LOG;
void utilend(){
  if(LOG){
    fclose(LOG);
    LOG=NULL;
  }
}
void utilstart(){
  if(!LOG){
    LOG=fopen("log.txt","w");
    atexit(utilend);
  }
}