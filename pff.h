#ifndef PRETTYCOLOR
#define PRETTYCOLOR

#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <poll.h>
#include "raylib-5.5/src/raylib.h"

#define RANDOMNESS 4
#define GRIDSIZE 64 
#define PIXELSIZE 4
#define PIXELBITS simpleLog2(PIXELSIZE)
struct pop {
    float r;
    float g;
    float b;
    struct pop* neighbor[4];
    float strength[4];
};
Image createWindow();
uint8_t simpleLog2(int );
void randomize(struct pop* pos);
float randRange(float start, float end);
void navigate(struct pop* current);
void average(struct pop* pos);
void displayConsole(struct pop(*)[GRIDSIZE]);
void displayScreen(struct pop(*)[GRIDSIZE] , Image);
float V3dotProduct(float x[3], float y[3]);
void V3normalize(float x[3], float y[3]);
#endif
