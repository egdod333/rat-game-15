#define MAIN
#include <util.h>
#include <math.h>
#include <r@@ui.h>
#include <ctype.h>
#include <stdio.h>
#include <curses.h>
#include <assets.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
ui16 mx,my;
WINDOW* scr;
component greg;
point k;
extern component* selected;
void inputcallback(ui32 i){
  if(i=='\\'){
    stopdrawing();
    stopreadinput();
  }
  switch(i){
    case KEY_UP:
    k.y--;
    break;
    case KEY_DOWN:
    k.y++;
    break;
    case KEY_LEFT:
    k.x--;
    break;
    case KEY_RIGHT:
    k.x++;
    break;
  }
  redraw=1;
}
void drawcallback(){
  clear();
  cdraw(&greg,k);
  refresh();
}
void accepttutorial(){
  greg.text=":3";
  greg.data.ccount=0;
  cselect(&greg);
}
void denytutorial(){
  utilend();
  guiend();
}
int main(int argc,char *argv[]){
  utilstart();
  scr=guistart();
  readstyle(&style_basic,"style_basic");
  readstyle(&style_selected,"style_selected");
  getmaxyx(scr,my,mx);
  greg=(component){
    .parent=NULL,
    .text="\
Welcome to Rat Game 15!!!\n\
Do you want the tutorial?\n\
Use number keys to pick an option and enter to select it.\n\
Hint: backspace can go back",
    .children=(component[]){
      newcom((point){.x=6,.y=3},(point){.x=8,.y=3},&greg,"accept"),
      newcom((point){.x=6,.y=3},(point){.x=3,.y=3},&greg,"deny")
    },
    .pattern=ROW,
    .data=(componentdata){
      .min=(point){.x=15,.y=5},
      .max=(point){.x=80,.y=16},
      .ccount=2,
      .textlength=137
    }
  };
  greg.children[0].interact=accepttutorial;
  greg.children[1].interact=denytutorial;
  cselect(&greg);
  k=(point){.x=2,.y=2};
  inputhandler=inputcallback;
  drawfunction=drawcallback;
  startdrawing();
  pthread_join(startreadinput(),NULL);
KILL_ME:
  guiend();
  utilend();
  return 0;
}