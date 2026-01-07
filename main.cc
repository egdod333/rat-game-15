#define MAIN
// #include <util.h>
// #include <math.h>
#include <r@@ui.h>
// #include <ctype.h>
#include <stdio.h>
// #include <curses.h>
// #include <assets.h>
// #include <stdlib.h>
// #include <string.h>
// #include <pthread.h>
int main(int argc,char *argv[]){
  ui::init();
  ui::component hello("hello world, we have line wrapping with break a a a a a a a a a a a a a a a a a a a a",8,16,2,2);
  hello.drawWrapped();
  hello.refresh();
  getch();
  ui::stop();
  return 0;
}