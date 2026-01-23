#ifndef RATATOUILLE
#define RATATOUILLE "kickass graphics"
#ifdef RATATOUILLE_NCURSES
#include <curses.h>
#endif
#include <functional>
#include <type_traits>
#include <vector>
#include <cmath>
#include <cstring>
typedef unsigned short int rat_size;
typedef signed short int sign_rat_size;
typedef float map_size;
namespace render {//https://yuriygeorgiev.com/2022/08/17/polygon-based-software-rendering-engine/
  template<typename T>\
  // concept arithmetic=std::is_arithmetic_v<T>;
  concept arithmetic=requires(T a,T b){a+b;a-b;a*b;a/b;};
  template<typename T>
  concept integral=std::is_integral_v<T>;
  template<typename T>
  concept comparable=requires(T a,T b){a<=>b;};

  template<typename T>
  requires arithmetic<T>&&comparable<T> struct vec3{
    T x,y,z;
    vec3<T> operator+(const vec3<T>& v){return vec3<T>(v.x+x,v.y+y,v.z+z);}
    vec3<T> operator*(const vec3<T>& v){return vec3<T>(v.x*x,v.y*y,v.z*z);}
    vec3<T> operator-(const vec3<T>& v){return vec3<T>(v.x-x,v.y-y,v.z-z);}
    vec3<T> operator/(const vec3<T>& v){return vec3<T>(v.x/x,v.y/y,v.z/z);}
    auto operator<=>(const vec3<T>& r) const {
      auto c=(x<=>r.x);if(c!=0){return c;}
      c=(y<=>r.y);if(c!=0){return c;}
      return z<=>r.z;
    }//https://stackoverflow.com/questions/47466358/what-is-the-spaceship-three-way-comparison-operator-in-c
    bool operator==(const vec3<T>& r)const{return((x==r.x)&&(y==r.y)&&(z==r.z));}
    bool operator<(const vec3<comparable auto>& r){return (x==r.x)?((y==r.y)?z<r.z:y<r.y):x<r.x;}
    bool operator>(const vec3<comparable auto>& r){return (x==r.x)?((y==r.y)?z>r.z:y>r.y):x>r.x;}
  };//how do i put this on a gpu if the player has one
  template<arithmetic T> struct lin3{vec3<T> a,b;};
  template<arithmetic T> struct tri3{vec3<T> a,b,c;int color;};
  template<arithmetic T> struct vec2{T x,y;};
  template<arithmetic T> struct lin2{vec3<T> a,b;};
  template<arithmetic T> struct tri2{vec2<T> a,b,c;int color;};
  struct twotriangles{tri3<map_size> a,b;bool two;};//...

  template<arithmetic T> inline void rot(vec3<T>& v,char d){
    float r1=cos(d/128.0*M_PI),r2=sin(d/128.0*M_PI);
    float x=(v.x*r1)-(v.y*r2);
    v.y=v.y*r1+v.x*r2;
    v.x=x;
  }
  template<arithmetic T> inline std::make_signed_t<T> triarea(T x0,T y0,T x1,T y1,T x2,T y2){
    using st=std::make_signed_t<T>;
    return((x0 * ((st)y1-y2)) + (x1 * ((st)y2-y0)) + (x2 * ((st)y0-y1)));
  }
  template<arithmetic T> inline vec2<rat_size> toScreenSpace(const vec3<T>& v,rat_size sx,rat_size sy){
    return((vec2<rat_size>){
      (rat_size)((v.y/v.x+1)*sx/2),
      (rat_size)((v.z/v.x+1)*sy/2)
    });
  }
  template<arithmetic T> inline tri2<rat_size> toScreenSpace(const tri3<T>& t,rat_size sx,rat_size sy){
    return((tri2<rat_size>){
      .a=toScreenSpace(t.a,sx,sy),
      .b=toScreenSpace(t.b,sx,sy),
      .c=toScreenSpace(t.c,sx,sy)
    });
  }
  template<arithmetic T> inline vec3<T> clipTo(const vec3<T>& a,const vec3<T>& b,const arithmetic auto& j){
    if(a.x==b.x){
      throw 1234;
    }
    return((vec3<T>){
      (T)j,
      (j-a.x)*((a.y-b.y)/(a.x-b.x))+a.y,
      (j-a.x)*((a.z-b.z)/(a.x-b.x))+a.z
    });
  }
  // twotriangles clipToCamera(const tri3<map_size>& t);

  extern std::vector<tri3<map_size>> map;
  extern double fov;

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
  template<typename T>
  concept constornotstr=std::is_same_v<T,const char*>||std::is_same_v<T,char*>||(T::value==NULL);
  class component{
    protected:
#ifdef RATATOUILLE_NCURSES
    WINDOW* c_win;//need a way to localize this to the ncurses version
#endif
    public:
    rat_size& x_0,y_0,x_1,y_1;
    // rat_size last_x0,last_y0,last_x1,last_y1;
    char* title;//probably disallow any non printing except \n
    char(*borderprovider)(border_type,rat_size) noexcept;
    template<constornotstr T>
    component(T name,rat_size height,rat_size width,rat_size y,rat_size x) noexcept;
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
    // textcomponent(char* text,char* name,rat_size height,rat_size width,rat_size y,rat_size x) noexcept;
    // textcomponent(const char* text,const char* name,rat_size height,rat_size width,rat_size y,rat_size x) noexcept;
    template<typename T,typename U>
    requires (constornotstr<T>,constornotstr<U>)
    textcomponent(T text,U name,rat_size height,rat_size width,rat_size y,rat_size x) noexcept;
    textcomponent(const component& c) noexcept;
    textcomponent(const textcomponent& tc) noexcept;
    textcomponent(void) noexcept;
   ~textcomponent(void) noexcept;
    void draw(void) const noexcept override;
  };
  class cameracomponent:public component{
    void putPixel(rat_size x,rat_size y,char color,char c) const;
    void putPixel(vec2<rat_size> p,char color,char c) const {
      auto [x,y]=p;
      putPixel(x,y,color,c);
    }
    void drawLine(rat_size x0,rat_size y0,rat_size x1,rat_size y1,char color) const;
    void drawLine(const vec2<rat_size>& a,const vec2<rat_size>& b,char color) const {drawLine(a.x,a.y,b.x,b.y,color);}
    void drawLine(const lin2<rat_size>& l,char color) const {drawLine(l.a.x,l.a.y,l.b.x,l.b.y,color);}
    void drawTri(rat_size x0,rat_size y0,rat_size x1,rat_size y1,rat_size x2,rat_size y2,char color, char ch) const;
    void drawTri(const vec2<rat_size>& a,const vec2<rat_size>& b,const vec2<rat_size>& c,char color,char ch) const {drawTri(a.x,a.y,b.x,b.y,c.x,c.y,color,ch);}
    void drawTri(const tri2<rat_size>& t,char color,char ch) const {drawTri(t.a.x,t.a.y,t.b.x,t.b.y,t.c.x,t.c.y,color,ch);}
    void drawTri(map_size x0,map_size y0,map_size z0,map_size x1,map_size y1,map_size z1,map_size x2,map_size y2,map_size z2,char color) const;
    void drawTri(const vec3<map_size>& a,const vec3<map_size>& b,const vec3<map_size>& c,char color) const {drawTri(a.x,a.y,a.z,b.x,b.y,b.z,c.x,c.y,c.z,color);}
    void drawTri(const tri3<map_size>& t,char color) const {drawTri(t.a.x,t.a.y,t.a.z,t.b.x,t.b.y,t.b.z,t.c.x,t.c.y,t.c.z,color);}
    public:
    vec3<map_size> cPos{0,0,0};
    unsigned char cRot=0;//you can only have 256 rotations
    template<constornotstr T>
    cameracomponent(T name,rat_size height,rat_size width,rat_size y,rat_size x) noexcept;
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
#ifdef RATATOUILLE_NCURSES//template function DEFINITIONS need to be visible to the translation unit they're used in
namespace ui {//as well as the declaractions. which is mildly annoying. but we ball
  template<constornotstr T>
  component::component(T ptitle,rat_size height,rat_size width,rat_size y,rat_size x) noexcept:
    c_win(newwin(height,width,y,x)),
    title(const_cast<char*>(std::is_const_v<T>?strcpy(new char[strlen(ptitle)+1],ptitle):ptitle)),
    x_0((rat_size&)c_win->_begx),y_0(c_win->_begy),x_1(c_win->_maxx),y_1(c_win->_maxy),
    borderprovider(defaultborderprovider)/*,
    last_x0(x_0),last_y0(y_0),last_x1(x_1),last_y1(y_1)*/{}
  template<typename T,typename U>
  requires (constornotstr<T>,constornotstr<U>)
  textcomponent::textcomponent(T ptext,U ptitle,rat_size height,rat_size width,rat_size y,rat_size x) noexcept:
    component(ptitle,height,width,y,x),
    text(const_cast<char*>(std::is_const_v<U>?strcpy(new char[strlen(ptext)+1],ptext):ptext)){}//this is so readable. trust me.
  template<constornotstr T>
  cameracomponent::cameracomponent(T ptitle,rat_size height,rat_size width,rat_size y,rat_size x) noexcept:
  component(ptitle,height,width,y,x){}
}
#endif
#endif