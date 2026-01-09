#ifndef RATATOUILLE
#define RATATOUILLE "kickass graphics"
#include <vector>
#ifdef RATATOUILLE_NCURSES
#include <curses.h>
#endif
#include <functional>//reason everything is noexcept is that if it stops in the middle of something the terminal
typedef NCURSES_SIZE_T nsize;//gets lobotomized and needs a restart (annoying as fuck) (if you write something
namespace ui {//that crashes or segfaults or whatever i get to kill and eat you)
  enum border_type {
    VERTICAL,
    HORIZONTAL,
    CORNER,
  };
  class component{
    protected:
#ifdef RATATOUILLE_NCURSES
    WINDOW* c_win;//need a way to localize this to the ncurses version
#endif
    public:
    nsize& x0,y0,x1,y1;
    // nsize last_x0,last_y0,last_x1,last_y1;
    char* title;//probably disallow any non printing except \n
    char(*borderprovider)(border_type,nsize) noexcept;
    component(char* name,nsize height,nsize width,nsize y,nsize x) noexcept;
    component(const char* name,nsize height,nsize width,nsize y,nsize x) noexcept;
    component(const component& c) noexcept;
    component(void) noexcept;//i like having the little void in there it looks nice and neat
   ~component(void) noexcept;//me: why does this tea have steam shaped like a skull coming off of it
    void corner() const noexcept;  //my loyal eunuch: i'm sure it tastes fine your majesty
    virtual void draw(void) const noexcept;//borders n text or whatever
    void refresh(void) const noexcept;
  };
  class textcomponent:public component{
    public:
    char* text;
    //formatting bullshittery goes in here somewhere
    textcomponent(char* text,char* name,nsize height,nsize width,nsize y,nsize x) noexcept;
    textcomponent(const char* text,const char* name,nsize height,nsize width,nsize y,nsize x) noexcept;
    textcomponent(const component& c) noexcept;
    textcomponent(const textcomponent& tc) noexcept;
    textcomponent(void) noexcept;
   ~textcomponent(void) noexcept;
    void draw(void) const noexcept;
  };
  class componentgroup {//do later
    // std::deque<component> items;//double end queue. i sure do hope we're memory safe
    // template<component... elements>
    // componentgroup()
  };
  void init(void) noexcept;
  void stop(void) noexcept;
}
#endif