#ifndef RATATOUILLE_NCURSES
#define RATATOUILLE_NCURSES "yello"
#include <curses.h>//c++ curses is a fuck: tries to include etip.h when it's etip.h.in??? which is stupid
#include <iostream>//anyways r@@ui is now a bridge to whatever graphics shit we're doing. on linux it'll
#include <cassert>
#include <cstring>
#include <r@@ui.h>// be to c (n)curses. this is for multiplatform viability. will write shell scripts for whatever
#include <functional>
namespace ui {//https://pubs.opengroup.org/onlinepubs/007908799/xcurses/curses.h.html
  using namespace render;
  WINDOW* mainWin;
  char(*defaultborderprovider)(border_type,rat_size) noexcept=[](border_type t,rat_size i)noexcept->char{
    switch(t){
      case CORNER:return '+';
      case VERTICAL:return '|';
      case HORIZONTAL:return '-';
    }
    return '?';
  };

  component::component(char* ptitle,rat_size height,rat_size width,rat_size y,rat_size x) noexcept:
    c_win(newwin(height,width,y,x)),
    title(ptitle),
    x0((rat_size&)c_win->_begx),y0(c_win->_begy),x1(c_win->_maxx),y1(c_win->_maxy),
    borderprovider(defaultborderprovider)
    // ,last_x0(x0),last_y0(y0),last_x1(x1),last_y1(y1)
  {}
  textcomponent::textcomponent(char* ptext,char* ptitle,rat_size height,rat_size width,rat_size y,rat_size x) noexcept:
    component(ptitle,height,width,y,x),
    text(ptext){}
  cameracomponent::cameracomponent(char* ptitle,rat_size height,rat_size width,rat_size y,rat_size x) noexcept:
  component(ptitle,height,width,y,x){}

  component::component(const char* ptitle,rat_size height,rat_size width,rat_size y,rat_size x) noexcept:
    c_win(newwin(height,width,y,x)),
    title(new char[strlen(ptitle)+1]),
    x0((rat_size&)c_win->_begx),y0(c_win->_begy),x1(c_win->_maxx),y1(c_win->_maxy),
    borderprovider(defaultborderprovider)
    // ,last_x0(x0),last_y0(y0),last_x1(x1),last_y1(y1)
  {
    strcpy(title,ptitle);
  }
  textcomponent::textcomponent(const char* ptext,const char* ptitle,rat_size height,rat_size width,rat_size y,rat_size x) noexcept:
    component(ptitle,height,width,y,x),
    text(new char[strlen(ptext)+1]){
      strcpy(text,ptext);
    }
  cameracomponent::cameracomponent(const char* ptitle,rat_size height,rat_size width,rat_size y,rat_size x) noexcept:
    component(ptitle,height,width,y,x){}

  component::component(const component& c) noexcept:
    c_win(newwin(c.y1,c.x1,c.y0,c.x0)),
    title(strcpy(new char[strlen(c.title)+1],c.title)),
    x0((rat_size&)c_win->_begx),y0(c_win->_begy),x1(c_win->_maxx),y1(c_win->_maxy),
    borderprovider(c.borderprovider)
    // ,last_x0(x0),last_y0(y0),last_x1(x1),last_y1
  {}
  textcomponent::textcomponent(const component& c) noexcept:
    component(c){}
  textcomponent::textcomponent(const textcomponent& tc) noexcept:
    component(tc),
    text(strcpy(new char[strlen(tc.text)+1],tc.text)){}//switch all these to mallocs and frees if im being real
  cameracomponent::cameracomponent(const component& c) noexcept:
    component(c){}//shouldn't need another one because no extra variables

  component::component() noexcept:component(static_cast<char*>(NULL),4,16,3,3){}
  textcomponent::textcomponent() noexcept:textcomponent(static_cast<char*>(NULL),static_cast<char*>(NULL),4,16,3,3){}

  component::~component() noexcept {delwin(c_win);}
  textcomponent::~textcomponent() noexcept {/*this destructor called, then parent class*/}
  cameracomponent::~cameracomponent() noexcept{}

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
  void cameracomponent::draw() const noexcept {
    corner();
    std::for_each(map.begin(),map.end(),[this](const lin3<float> l){
      vec2<rat_size> a={
        .x=(rat_size)(((l.a.y/l.a.x)+1)/2*x1),
        .y=(rat_size)(((l.a.z/l.a.x)+1)/2*y1)
      },b={
        .x=(rat_size)(((l.b.y/l.b.x)+1)/2*x1),
        .y=(rat_size)(((l.b.z/l.b.x)+1)/2*y1)
      };
      drawLine(a,b,1);
      putPixel(a,2);
      putPixel(b,2);
    });
    
    // kill(myself)
  }

  void component::refresh() const noexcept {wrefresh(c_win);}

  // void component::focus(){state.focused=&this;}

  void cameracomponent::putPixel(vec2<integral auto> p,char color) const {
    wattron(c_win,COLOR_PAIR(color));
    mvwaddch(c_win,p.y,p.x,'+');
  }
  
  void cameracomponent::drawLine(vec2<integral auto> a,vec2<integral auto> b,char color) const {
    signed short int
      sx=(a.x<b.x)?1:-1,
      sy=(a.y<b.y)?1:-1,
      dx=(a.x>b.x)?a.x-b.x:b.x-a.x,
      dy=(a.y>b.y)?a.y-b.y:b.y-a.y,
      err=(dx>dy)?dx/2:dy/2,e2=0;
    int i=0;
    // printf("s=(%u,%u),d=(%u,%u),err=%u",sx,sy,dx,dy,err);
    while(i<200){
      // printf("(%u,%u),",a.x,a.y);
      this->putPixel(a,1);
        printf("%u",a.x);
      if(a.x==b.x){
        if(a.y==b.y){break;}}
      e2=err;
      if(e2>-dx){err-=dy;a.x+=sx;}
      if(e2< dy){err+=dx;a.y+=sy;}
      i++;
    }
  }
  
  void init() noexcept {
    mainWin = initscr();
    cbreak();
    keypad(mainWin,TRUE);
    noecho();
    curs_set(0);
    refresh();
    state.screen_height=mainWin->_maxy;state.screen_width=mainWin->_maxx;
    state.scractive=1;
  }

  void stop() noexcept {
    state.scractive=0;
    endwin();
  }
}
namespace render {
  std::vector<lin3<float>> map;
  void genMap(){
    map.push_back(
      (lin3<float>){
        (vec3<float>){ 1, 0.5, 3},
        (vec3<float>){ 4, 0.5, 3}}
    );
    map.push_back(
      (lin3<float>){
        (vec3<float>){ 1,-0.5, 2},
        (vec3<float>){ 4,-0.5, 2}}
    );
  }
  
  void init(){
    assert(::ui::state.scractive);//idiot
    assert(has_colors());//we need color :/
    start_color();
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    init_pair(2,COLOR_RED,COLOR_MAGENTA);
    // attron(COLOR_PAIR(1));
    // wprintw(ui::mainWin,"max pairs=%u",COLOR_PAIRS);
    genMap();
  }
  void stop(){
    //kill
  }
}
#endif