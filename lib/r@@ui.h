#ifndef RATATOUILLE
#define RATATOUILLE "kickass graphics"
#include <vector>
// #ifdef RATATOUILLE_NCURSES
#include <curses.h>
// #endif
#include <functional>
#include <type_traits>
#include <vector>
typedef unsigned short int rat_size;
namespace render {//https://yuriygeorgiev.com/2022/08/17/polygon-based-software-rendering-engine/
  template<typename T>
  concept arithmetic=std::is_arithmetic_v<T>;
  template<typename T>
  concept integral=std::is_integral_v<T>;
  
  template<arithmetic T>
  struct vec3{T x,y,z;};//how do i put this on a gpu if the player has one
  template<arithmetic T>
  struct lin3{vec3<T> a,b;};
  template<arithmetic T>
  struct tri3{vec3<T> a,b,c;int color;};
  template<arithmetic T>
  struct vec2{T x,y;};
  template<arithmetic T>
  struct lin2{vec3<T> a,b;};
  template<arithmetic T>
  struct tri2{vec2<T> a,b,c;int color;};
  
  extern std::vector<lin3<float>> map;
  
  void init();
  void stop();
}
namespace ui {//reason everything is noexcept is that if it stops in the middle of something the terminal
  using namespace render;
  enum border_type {//gets lobotomized and needs a restart (annoying as fuck)
    VERTICAL,//(if you write something that crashes or segfaults or whatever i get to kill and eat you)
    HORIZONTAL,
    CORNER,
  };
  extern char(*defaultborderprovider)(border_type,rat_size) noexcept;
  class component{
    protected:
#ifdef RATATOUILLE_NCURSES
    WINDOW* c_win;//need a way to localize this to the ncurses version
#endif
    public:
    rat_size& x0,y0,x1,y1;
    // rat_size last_x0,last_y0,last_x1,last_y1;
    char* title;//probably disallow any non printing except \n
    char(*borderprovider)(border_type,rat_size) noexcept;
    component(char* name,rat_size height,rat_size width,rat_size y,rat_size x) noexcept;
    component(const char* name,rat_size height,rat_size width,rat_size y,rat_size x) noexcept;
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
    textcomponent(char* text,char* name,rat_size height,rat_size width,rat_size y,rat_size x) noexcept;
    textcomponent(const char* text,const char* name,rat_size height,rat_size width,rat_size y,rat_size x) noexcept;
    textcomponent(const component& c) noexcept;
    textcomponent(const textcomponent& tc) noexcept;
    textcomponent(void) noexcept;
   ~textcomponent(void) noexcept;
    void draw(void) const noexcept override;
  };
  class cameracomponent:public component{
    void putPixel(vec2<integral auto> p,char color) const;
    void drawLine(vec2<integral auto> a,vec2<integral auto> b,char color) const;
    public:
    cameracomponent(char* name,rat_size height,rat_size width,rat_size y,rat_size x) noexcept;
    cameracomponent(const char* name,rat_size height,rat_size width,rat_size y,rat_size x) noexcept;
    cameracomponent(const component& c) noexcept;
    cameracomponent(const cameracomponent& cc) noexcept;
    cameracomponent(void) noexcept;
   ~cameracomponent(void) noexcept;
    void draw(void) const noexcept override;
  };
  // class componentgroup {//do later
    // std::deque<component> items;//double end queue. i sure do hope we're memory safe
    // template<component... elements>
    // componentgroup()
  // };
  struct {
    component* focused;
    unsigned char scractive:1;//bitfield packing is impl defined (worry) (use masking or whatever instead)
    short int screen_width,screen_height;
  } state = {};
  void init(void) noexcept;
  void stop(void) noexcept;
}
#endif