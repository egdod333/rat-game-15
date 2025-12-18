#include <util.h>
#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>//goddd i need so many libs for like the most simple thing
#include <assets.h>

void readstyle(windowstyle* write,const char* name){//15
  char* path=malloc(128);//prolly enough i'll be fine. unsafe buffer write ahead
  getcwd(path,128);
  ui8 len=strlen(path);
  strcat(path,"/assets/style/");
  strcat(path,name);
  FILE* f=fopen(path,"r");
  free(path);
  fread(write,sizeof(windowstyle),1,f);
}
void writestyle(windowstyle* read,const char* name){
  char* path=malloc(13+strlen(name));strcat(path,name);
  FILE* write=fopen(path,"w");
  fwrite(read,sizeof(windowstyle),1,write);
  fclose(write);
  free(path);
}