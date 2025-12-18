#ifndef RATATOUILLE
#define RATATOUILLE
#include <util.h>
#include <curses.h>
#include <pthread.h>
typedef enum cpattern{
  ROW,COLUMN,GRID
}cpattern;
typedef struct point{
  ui16 x;ui16 y;
}point;
typedef struct soint{
  i16 x;i16 y;
}soint;
typedef struct componentdata{
  point min,max;
  ui16 textlength;
  ui8 ccount;
} componentdata;
typedef struct component{
  struct component* parent;
  struct component* children;
  char* text;
  cpattern pattern;
  componentdata data;
  void(*interact)(void);
} component;
extern component* selected;
extern void(*inputhandler)(ui32);
extern void(*drawfunction)(void);
extern ui8 readinput,dodrawing,redraw;
extern windowstyle style_basic,style_selected;
component ncfull(point min,point max,component* parent,component *children,ui8 ccount,cpattern pat,char* text);
component newcom(point min,point max,component* parent,char* text);
void cmmsdraw(component* c,windowstyle* s,point min,point max,point start);
void csdraw(component* c,windowstyle* s,point p);
void cdraw(component* c,point p);
void cselect(component* c);
void* whileread(void*);
void* whiledraw(void*);
pthread_t startreadinput();
pthread_t startdrawing();
void stopreadinput();
void stopdrawing();
WINDOW* guistart();
void guiend();
void setinputoverride(ui8 i);
#endif