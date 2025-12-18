#include <util.h>
#include <ctype.h>
#include <r@@ui.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// #include <unistd.h>
#include <pthread.h>

extern ui16 mx,my;
component* selected;
void(*inputhandler)(ui32);
void(*drawfunction)(void);
ui8 readinput,dodrawing,redraw;
static pthread_t inputthread,graphicsthread;
static ui8 aio=1;//use ^v<>, 0-9 and bksp to manipulate windows
windowstyle style_basic,style_selected;

component ncfull(point min,point max,component* parent,component *children,ui8 ccount,cpattern pat,char* text){
  return(component){
    .parent=parent,
    .children=NULL,
    .pattern=ROW,
    .text=text,
    .data=(componentdata){//things that can be animated
      .min=min,.max=max,
      .textlength=strlen(text),
      .ccount=ccount
    }
  };
}
component newcom(point min,point max,component* parent,char* text){
  return ncfull(min,max,parent,NULL,0,ROW,text);
}
static ui8 lo4(const char* o){
  if(isprint(*o)){o++;}else{return 0;}//:3 kills you
  if(isprint(*o)){o++;}else{return 1;}
  if(isprint(*o)){o++;}else{return 2;}
  if(isprint(*o)){o++;}else{return 3;}
  return 4;
}
static void borderwithstyle(point start,point end,windowstyle style){
  ui8 c=lo4(style.corners);
  mvaddch(start.y,start.x,style.corners[0]);
  mvaddch(start.y,end.x,style.corners[1%c]);
  mvaddch(end.y,start.x,style.corners[2%c]);
  mvaddch(end.y,end.x,style.corners[3%c]);
  ui8 t=lo4(style.top),b=lo4(style.bottom),l=lo4(style.left),r=lo4(style.right);
  for(ui16 i=start.y+1;i<end.y;i++){
    mvaddch(i,start.x,style.left[(i-start.y-1)%l]);
    mvaddch(i,end.x,style.right[(i-start.y-1)%r]);
  }
  for(ui16 i=start.x+1;i<end.x;i++){
    mvaddch(start.y,i,style.top[(i-start.x-1)%t]);
    mvaddch(end.y,i,style.bottom[(i-start.x-1)%b]);
  }
}
void cmmsdraw(component* c,windowstyle* s,point min,point max,point start){
  if(s==NULL){
    if(c==selected){
      borderwithstyle(start,(point){.x=max.x+start.x-1,.y=max.y+start.y},style_selected);
    }else{
      borderwithstyle(start,(point){.x=max.x+start.x-1,.y=max.y+start.y},style_basic);
    }
  }else{
    borderwithstyle(start,(point){.x=max.x+start.x-1,.y=max.y+start.y},*s);
  }
  start.x++;start.y++;
  max.x-=2;max.y-=2;//borders
  if(c->text!=NULL){
    ui16 len=c->data.textlength;
    if(len!=0){
      char* t=c->text;
      char* t1=strchr(t,'\n');
      while((t1!=NULL)&&(len>0)){
        ui16 l=t1-t;
        if(l>len){l=len;}
        ui16 y=l/max.x;
        for(ui8 i=0;i<y;i++){
          mvaddnstr(start.y+i,start.x,t,(l<max.x)?l:max.x);
          lprintf("%.*s\n",(l<max.x)?l:max.x,t);
          l-=max.x;t+=max.x;len-=max.x;
        }
        mvaddnstr(start.y+y,start.x,t,l);len-=l+1;
        start.y+=y+1;max.y-=y+1;
        t=t1+1;
        t1=strchr(t+1,'\n');
      }
      // t++;
      ui16 l=strlen(t);
      if(l>len){l=len;}
      ui16 y=l/max.x;
      for(ui8 i=0;i<y;i++){
        mvaddnstr(start.y+i,start.x,t,(l<max.x)?l:max.x);
        l-=max.x;t+=max.x;
      }
      mvaddnstr(start.y+y,start.x,t,l);
      start.y+=y+1;max.y-=y+1;
    }
  }
  
  if((c->children!=NULL)&&(c->data.ccount>0)){
    switch(c->pattern){
      case ROW:{
        float total=0;
        // float val=0;
        for(ui8 i=0;i<c->data.ccount;i++){
          if(c->children[i].data.min.y>max.y){continue;}
          if((c->children[i].data.min.x+(ui16)total)>max.x){continue;}
          total+=c->children[i].data.min.x;
        }//https://www.desmos.com/calculator/lhoqmep6h7 <-proof of concept for trying for even-ness
        // for(ui8 i=0;i<c->data.ccount;i++){
        //   if(c->children[i].min.x>max.y){continue;}
        //   val+=(total)/(c->children[i].min.x);
        // }
        for(ui8 i=0;i<c->data.ccount;i++){
          if(c->children[i].data.min.y>max.y){continue;}
          ui16 pad=(c->children[i].data.min.x*max.x)/total;//(total*(max.x-total))/(c->children[i].min.x*val);
          cmmsdraw(
            &c->children[i],s,
            (point){.y=max.y,.x=pad},
            (point){.y=max.y,.x=pad},
            start
          );
          if(c==selected&&i<10){//could probably minimize comparisons but like who cares its 5 cpu cycles
            mvaddch(start.y,start.x,i+48);
          }
          start.x+=pad;
        }
      }
      break;
      case COLUMN:{
        float total=0;
        // float val=0;
        for(ui8 i=0;i<c->data.ccount;i++){
          if(c->children[i].data.min.x>max.x){continue;}
          if((c->children[i].data.min.y+(ui16)total)>max.y){continue;}
          total+=c->children[i].data.min.y;
        }//https://www.desmos.com/calculator/lhoqmep6h7
        // for(ui8 i=0;i<c->data.ccount;i++){
        //   if(c->children[i].min.x>max.x){continue;}
        //   // val+=(total)/(c->children[i].min.y);
        // }
        for(ui8 i=0;i<c->data.ccount;i++){
          if(c->children[i].data.min.x>max.x){continue;}
          ui16 pad=(c->children[i].data.min.y*max.y)/total;//(total*(max.y-total))/(c->children[i].min.y*val);
          cmmsdraw(
            &c->children[i],s,
            (point){.y=pad,.x=max.x},
            (point){.y=pad,.x=max.x},
            start
          );
          if(c==selected&&i<10){//could probably minimize comparisons but like who cares its 5 cpu cycles
            mvaddch(start.y,start.x,i+48);
          }
          start.y+=pad+1;
        }
      }
      break;
      default:
        exit(1);
    }
  }
}
void csdraw(component* c,windowstyle* s,point p){cmmsdraw(c,s,c->data.min,c->data.max,p);};
void cdraw(component* c,point p){cmmsdraw(c,NULL,c->data.min,c->data.max,p);}
void cselect(component* c){
  selected=c;
}
WINDOW* guistart(){
  WINDOW* out=initscr();
  noecho();
  // nodelay(stdscr,FALSE);
  // noraw();
  cbreak();
  refresh();//important but it doesn't look like it
  keypad(stdscr,1);//interpret more than single characters (allows esc codes to be one whole thing n whatnot)
  // curs_set(0);//doesn't work on this terminal but might for others
  atexit(guiend);
  return out;
}
void* whileread(void*){//has reserved values for window selection and scrolling
  while(readinput){
    ui32 i=getch();
    if(aio){
      if((48<=i)&&(i<=57)){
        if((selected->children!=NULL)&&(selected->data.ccount>(i-48))){
          selected=&selected->children[i-48];
        }
      }else if((i==KEY_BACKSPACE)||(i==127)){
        if((selected!=NULL)&&(selected->parent!=NULL)){selected=selected->parent;}
      }else if((i==KEY_ENTER)||i==10){
        if((selected!=NULL)&&(selected->interact!=NULL)){selected->interact();}
      }
    }
    inputhandler(i);
  }
  return NULL;
}
void* whiledraw(void*){//'\\'
  static struct timespec t;
  static struct timespec t1;
  while(dodrawing){
    clock_gettime(CLOCK_MONOTONIC,&t);
    if((t.tv_sec!=t1.tv_sec)||(t.tv_nsec-t1.tv_nsec>66666666)){//15 frames per schlecond
      t1=t;
      if(redraw){redraw=0;drawfunction();}
    }
  }
}
void guiend(){
  stopreadinput();
  stopdrawing();
  endwin();
}
pthread_t startreadinput(){
  readinput=1;
  pthread_create(&inputthread,NULL, whileread,NULL);
  return inputthread;
}
pthread_t startdrawing(){
  dodrawing=1;
  pthread_create(&graphicsthread,NULL,whiledraw,NULL);
  return graphicsthread;
}
void stopreadinput(){
  readinput=0;
}
void stopdrawing(){
  dodrawing=0;
}
void setinputoverride(ui8 i){
  aio=i;
}