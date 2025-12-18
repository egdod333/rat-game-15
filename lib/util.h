#ifndef UTIL
#define UTIL
#include <stdint.h>
#include <stdio.h>
#define lprint(x) if(LOG)fprintf(LOG,x);fflush(LOG)
#define lprintf(x,...) if(LOG)fprintf(LOG,x,__VA_ARGS__);fflush(LOG)
typedef uint8_t ui8;
typedef uint16_t ui16;
typedef uint32_t ui32;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
extern FILE* LOG;
typedef struct windowstyle{
  char corners[4];//upper left, upper right, bottom left, bottom right
  char top[4];//left-to-right and up-to-down
  char bottom[4];
  char left[4];
  char right[4];
}windowstyle;
typedef char grayscale16x16[16][16];//for later
void utilstart();
void utilend();
#endif