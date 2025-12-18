#include <util.h>
#include <ctype.h>
#include <r@@ui.h>
#include <stdio.h>
#include <assets.h>
#include <curses.h>
#include <stdlib.h>
#include <pthread.h>
ui16 mx,my;
static WINDOW* scr;
static component c1;
void input(ui32 i){
  static ui8 mode=0;
  static ui8 sel=0;
  static ui32 cx,cy;
  static windowstyle style=(windowstyle){.corners="+",.top="-",.bottom="-",.left="|",.right="|"};
  static ui8 l[5]={1,1,1,1,1};
  static char* c[5]={style.corners,style.top,style.bottom,style.left,style.right};
  static point k=(point){.x=16,.y=8};
  if(i=='`'){
    mode=(mode+1)%2;
  }else if(i==9){
    stopreadinput();
  }
  if(mode==1){
    switch(i){
      case KEY_UP:
      k.y--;
      break;
      case KEY_DOWN:
      k.y++;
      break;
      case KEY_LEFT:
      k.x--;
      break;
      case KEY_RIGHT:
      k.x++;
      break;
    }
    clear();
    mvprintw(0,0,"(%u,%u)->(%u,%u)",
      c1.children[1].data.min.x,
      c1.children[1].data.min.y,
      c1.children[1].data.max.x,
      c1.children[1].data.max.y);
    goto DRAW_ENDSTEP;
  }else if(mode==0){
    if(isprint(i)){
      if(l[sel]!=4){
        c[sel][l[sel]]=i;
        l[sel]++;
      }
    }else{
      if(i==KEY_UP){
        sel=(sel+4)%5;
      }else if(i==KEY_DOWN){
        sel=(sel+1)%5;
      }else if((i==KEY_BACKSPACE)||(i==127)){
        if(l[sel]>1){
          c[sel][l[sel]-1]='\0';
          l[sel]--;
        }else{
          c[sel][0]=' ';
          l[sel]=0;
        }
      }
    }
    clear();
    mvaddstr(0,0,"tab to exit, ` to change modes, arrows to navigate, the rest to type");
    mvprintw(1,0,"\
MODE=%u\n\
- corner:%.4s\n\
- top   :%.4s\n\
- bottom:%.4s\n\
- left  :%.4s\n\
- right :%.4s\n"\
    ,mode,style.corners,style.top,style.bottom,style.left,style.right);
    mvaddch(2+sel,0,'x');
    goto DRAW_ENDSTEP;
  }
  DRAW_ENDSTEP://inelegant but idrc
  csdraw(&c1,&style,k);
}
void styleeditor(){
  utilstart();
  scr=guistart();
  c1=(component){
    .data=(componentdata){
      .min=(point){.x=16,.y=8},
      .max=(point){.x=16,.y=16},
      .text="wowafdskhjkjhgkjhgkjhgkjhgkjhgkjhg",
      .ccount=2
    },
    .parent=NULL,
    .children=(component[]){
      (component){
        .data=(componentdata){
          .min=(point){.x=6,.y=3},
          .max=(point){.x=12,.y=6},
          .text="wow123456",
          .ccount=0
        },
        .parent=NULL,
        .children=NULL,
        .pattern=ROW
      },
      (component){
        .data=(componentdata){
          .min=(point){.x=6,.y=6},
          .max=(point){.x=16,.y=8},
          .text="wow243567",
          .ccount=0
        },
        .parent=NULL,
        .children=NULL,
        .pattern=ROW
      }
    },
    .pattern=ROW
  };
  inputhandler=input;
  input(0);
  setinputoverride(0);
  pthread_join(startreadinput(),NULL);
}
void gendefaultstyles(){
  puts("basic");
  FILE* write=fopen("assets/style/style_basic","w");
  fwrite(&(windowstyle){
    .corners="/\\\\/",
    .top="-",
    .bottom="-",
    .left="|",
    .right="|",
  },sizeof(windowstyle),1,write);
  fclose(write);
  puts("selected");
  write=fopen("assets/style/style_selected","w");
  fwrite(&(windowstyle){
    .corners="\\//\\",
    .top="-=-=",
    .bottom="=-=-",
    .left="[][]",
    .right="][][",
  },sizeof(windowstyle),1,write);
  fclose(write);
}
int main(int argc,char *argv[]){//passes an empty arg in index 0 for some reason, figure it out later
  ui32 option;
  if(argc==1){
    printf("\
ASSET MANAGER OPTIONS\n\
(0):cancel\n\
(1):generate default styles\n\
(2):style editor\n\
(3):image editor\n\
option:");
    scanf("%u",&option);//input just randomly blocks sometimes
  }else{
    option=atoi(argv[1]);
  }
  switch(option){
    case 0:
      printf("canceled, aborting\n");
      exit(0);
    break;
    case 1:
      printf("generating defaults\n");
      gendefaultstyles();
      printf("done\n");
    break;
    case 2:
      printf("style editor\n");
      styleeditor();
      printf("done\n");
    break;
    default:
      printf("unknown or unimplemented option, aborting\n");
      exit(0);
    break;
  }
}
