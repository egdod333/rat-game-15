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
  ui::cameracomponent wow("camera test",30,100,3,3);
  int i=0;
  render::vec3<float> pjeanort{0,0,0};
  while(i!='\\'){
    switch(i){
      case 'w':pjeanort.x+=1;break;
      case 'd':pjeanort.y+=1;break;
      case 'a':pjeanort.y-=1;break;
      case 's':pjeanort.x-=1;break;
      // case 258:down;break;
      // case 259:up;break;
      case 260:wow.cRot+=16;break;
      case 261:wow.cRot-=16;break;
    }
    rot(pjeanort,wow.cRot);
    wow.cPos=wow.cPos+pjeanort;
    wow.draw();
    wow.refresh();
    i=getch();
    ui::penis=i;
  }
  ui::stop();
  return 0;
}