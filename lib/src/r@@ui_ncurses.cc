#ifndef RATATOUILLE_NCURSES
#define RATATOUILLE_NCURSES "this shouldn\'t happen"
#endif
#include <curses.h>//c++ curses is a fuck: tries to include etip.h when it's etip.h.in??? which is stupid
#include <iostream>//anyways r@@ui is now a bridge to whatever graphics shit we're doing. on linux it'll
#include <cassert>
#include <cstring>
#include <r@@ui.h>// be to c (n)curses. this is for multiplatform viability. will write shell scripts for whatever
#include <functional>
namespace ui {//https://pubs.opengroup.org/onlinepubs/007908799/xcurses/curses.h.html
  WINDOW* mainWin;
  struct {
    component* focused;
    unsigned char scractive:1;//bitfield packing is impl defined (worry) (use masking or whatever instead)
  } state = {};
  
  char(*defaultborderprovider)(border_type,nsize) noexcept=[](border_type t,nsize i)noexcept->char{
    switch(t){
      case CORNER:return '+';
      case VERTICAL:return '|';
      case HORIZONTAL:return '-';
    }
    return '?';
  };

  component::component(char* ptitle,nsize height,nsize width,nsize y,nsize x) noexcept:
    c_win(newwin(height,width,y,x)),
    title(ptitle),
    x0(c_win->_begx),y0(c_win->_begy),x1(c_win->_maxx),y1(c_win->_maxy),
    borderprovider(defaultborderprovider)
    // ,last_x0(x0),last_y0(y0),last_x1(x1),last_y1(y1)
  {}
  textcomponent::textcomponent(char* ptext,char* ptitle,nsize height,nsize width,nsize y,nsize x) noexcept:
    component(ptitle,height,width,y,x),
    text(ptext){}

  component::component(const char* ptitle,nsize height,nsize width,nsize y,nsize x) noexcept:
    c_win(newwin(height,width,y,x)),
    title(new char[strlen(ptitle)+1]),
    x0(c_win->_begx),y0(c_win->_begy),x1(c_win->_maxx),y1(c_win->_maxy),
    borderprovider(defaultborderprovider)
    // ,last_x0(x0),last_y0(y0),last_x1(x1),last_y1(y1)
  {
    strcpy(title,ptitle);
  }
  textcomponent::textcomponent(const char* ptext,const char* ptitle,nsize height,nsize width,nsize y,nsize x) noexcept:
    component(ptitle,height,width,y,x),
    text(new char[strlen(ptext)+1]){
      strcpy(text,ptext);
    }

  component::component(const component& c) noexcept:
    c_win(newwin(c.y1,c.x1,c.y0,c.x0)),
    title(strcpy(new char[strlen(c.title)+1],c.title)),
    x0(c_win->_begx),y0(c_win->_begy),x1(c_win->_maxx),y1(c_win->_maxy),
    borderprovider(c.borderprovider)
    // ,last_x0(x0),last_y0(y0),last_x1(x1),last_y1
  {}
  textcomponent::textcomponent(const component& c) noexcept:
    component(c){}
  textcomponent::textcomponent(const textcomponent& tc) noexcept:
    component(tc),
    text(strcpy(new char[strlen(tc.text)+1],tc.text)){}//switch all these to mallocs and frees if im being real

  component::component() noexcept:component(static_cast<char*>(NULL),4,16,3,3){}
  textcomponent::textcomponent() noexcept:textcomponent(static_cast<char*>(NULL),static_cast<char*>(NULL),4,16,3,3){}

  component::~component() noexcept {delwin(c_win);}
  textcomponent::~textcomponent() noexcept {/*this destructor called, then parent class*/}

  void component::corner() const noexcept {
    mvwaddch(c_win,0,0,borderprovider(CORNER,0));
    mvwaddch(c_win,0,x1,borderprovider(CORNER,1));
    mvwaddch(c_win,y1,0,borderprovider(CORNER,2));
    mvwaddch(c_win,y1,x1,borderprovider(CORNER,3));
    if((title!=NULL)&&(strlen(title)>x1-2)){
      mvwaddnstr(c_win,0,1,title,x1-4);
      mvwaddstr(c_win,0,x1-3,"...");
    }else{
      mvwaddstr(c_win,0,1,title);
    }
  }

  void component::draw() const noexcept {corner();mvwaddnstr(c_win,1,1,"empty component",x1-1);}
  void textcomponent::draw() const noexcept {
    corner();
    unsigned int selstart=0;
    unsigned int y=1;
    unsigned int x=0;
    unsigned int length=strlen(text);
    for(unsigned int i=1;i<=length;i++){
      if((text[i]==' ')||(i==length)||(text[i]=='\n')){
        if((x+(i-selstart))>(x1-1)){
          y++;x=0;
          selstart++;
        }
          mvwaddnstr(c_win,y,x+1,&text[selstart],i-selstart);
        if(text[i]=='\n'){
          y++;i++;
          x=0;
        }else{
          x+=i-selstart;
        }
        selstart=i;
      }
    }
  }

  void component::refresh() const noexcept {wrefresh(c_win);}

  // void component::focus(){state.focused=&this;}

  void init() noexcept {
    mainWin = initscr();
    cbreak();
    keypad(mainWin,TRUE);
    noecho();
    curs_set(0);
    refresh();
    state.scractive=1;
  }

  void stop() noexcept {
    state.scractive=0;
    endwin();
  }
}