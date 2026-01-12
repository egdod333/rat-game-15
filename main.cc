#define MAIN ":3"
#define RATATOUILLE_NCURSES "terminal illness"
#define RATATOUILLE_USE_3D "help"
#include <r@@ui.h>
#include <iostream>
#include <stdio.h>
#include <cstring>
int main(int argc,char *argv[]){
  ui::init();
  render::init();
  ui::cameracomponent wow("camera test",20,100,3,3);
  wow.draw();
  wow.refresh();
  getch();
  ui::stop();
  return 0;
}