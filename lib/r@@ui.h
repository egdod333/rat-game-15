#ifndef RATATOUILLE
#define RATATOUILLE 0.0
#include <vector>
#include <curses.h>
#include <functional>
typedef NCURSES_SIZE_T nsize;
namespace ui {
  enum border_type {
    VERTICAL,
    HORIZONTAL,
    CORNER,
  };
  class component {
    WINDOW* win;//need a way to localize this to the ncurses version
    public:
    nsize& x0,y0,x1,y1;
    nsize last_x0,last_y0,last_x1,last_y1;
    const char* text;//disallow any non printing except \n
    const char* title;
    std::function<char(border_type,nsize)> borderprovider;
    component(const char* text,const char* name,nsize height,nsize width,nsize y,nsize x);
    component(const component& c);
    component(void);//i like having the little void in there it looks nice and neat
   ~component(void);//me: why does this tea have steam shaped like a skull coming off of it
    void corner();  //my loyal eunuch: i'm sure it tastes fine your majesty
    void draw(void);//borders n text or whatever
    void drawWrapped();
    void refresh(void);
  };
  class componentgroup {
    std::vector<component> items;
  };
  void init(void);
  void stop(void);
}
#endif