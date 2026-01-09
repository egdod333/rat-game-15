#define MAIN ":3"
#define RATATOUILLE_NCURSES "terminal illness"
#include <r@@ui.h>
#include <iostream>
#include <stdio.h>
#include <cstring>
int main(int argc,char *argv[]){
  ui::init();
  ui::textcomponent hello(new char[64],NULL,8,16,2,20);
  int i=0;
  int t=0;
  while(i!='\\'){
    clear();
    hello.draw();
    refresh();
    hello.refresh();
    i=getch();
    if(i!=127){
      hello.text[t]=i;
      t++;
    }else{
      t--;
      hello.text[t]=0;
    }
  }
  ui::stop();
  return 0;
}