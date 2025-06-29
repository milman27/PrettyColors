#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <poll.h>
#include <X11/Xlib.h>

#define RANDOMNESS 3
#define GRIDSIZE 128 


struct pop {
    float r;
    float g;
    float b;
    struct pop* neighbor[4];
    float strength[4];
};
struct WindowContext {
    Display* display;
    Window window;
};
struct WindowContext createWindow();
void randomize(struct pop* pos);
float randRange(float start, float end);
void navigate(struct pop* current);
void average(struct pop* pos);
void displayConsole(struct pop s[GRIDSIZE][GRIDSIZE]);
void displayScreen(struct pop s[GRIDSIZE][GRIDSIZE], struct WindowContext wc);
float V3dotProduct(float x[3], float y[3]);
void V3normalize(float x[3], float y[3]);
int main(int argc, char** argv){
    struct pop grid[GRIDSIZE][GRIDSIZE] = {0};
    for(unsigned int i = 0; i < GRIDSIZE; i++){
        for( unsigned int j = 0; j < GRIDSIZE; j++){
            grid[i][j].r = 100;
            grid[i][j].g = 100;
            grid[i][j].b = 100;

            grid[i][j].neighbor[0] = &grid[(i-1)&GRIDSIZE-1][j];
            grid[i][j].neighbor[1] = &grid[(i+1)%GRIDSIZE][j];
            grid[i][j].neighbor[2] = &grid[i][(j-1)%GRIDSIZE];
            grid[i][j].neighbor[3] = &grid[i][(j+1)%GRIDSIZE];

            if(i == GRIDSIZE/2 || i == GRIDSIZE/2 - 1 || i == 0 || i == GRIDSIZE - 1 || j == 0 || j == GRIDSIZE - 1 || j == GRIDSIZE/2 || j == GRIDSIZE/2 -1 ){
                grid[i][j].strength[0] = 0.04;
            }else{
                grid[i][j].strength[0] = 0.8;
            }
        }
    }
    char buffer[3];
    int count = 0;
    struct pollfd pollfd = {0 , POLLIN, 0};
    int pollio;
    struct WindowContext wc = createWindow();
    XEvent event;
    for(;;){
        //XNextEvent(wc.display, &event);
        if((count&15) == 0){
           displayScreen(grid, wc);
        }

        printf("Count: %d\n", count++);
        randomize(&grid[0][0]);
        average(&grid[0][0]);
        /*if(poll(&pollfd, (nfds_t)1, 1)){ 
            read(0,&buffer , sizeof(buffer));
            if(buffer[0] == 'q')
                break;
        }*/
    }
    navigate(&grid[0][0]);  
    return 0;

}
struct WindowContext createWindow(){
    struct WindowContext wc = {0};
    Display* display = XOpenDisplay(NULL);
    Window w = XCreateSimpleWindow(display, DefaultRootWindow(display), 100, 100, GRIDSIZE*8, GRIDSIZE*8, 1, BlackPixel(display, 0), WhitePixel(display, 0));
    XMapWindow(display, w);
    XSelectInput(display, w, ExposureMask); 
    wc.display = display;
    wc.window = w;
    return wc;
}
void randomize(struct pop* pos){
    for(int i = 0; i < GRIDSIZE; i++){
        for(int j = 0; j < GRIDSIZE; j++){
            pos->r += randRange(-1, 1)*RANDOMNESS;
            pos->g += randRange(-1, 1)*RANDOMNESS;
            pos->b += randRange(-1, 1)*RANDOMNESS;
            pos = pos->neighbor[3];
        }
        pos = pos->neighbor[1];
    }
}
void average(struct pop* pos){
    struct pop temp[GRIDSIZE][GRIDSIZE] = {0};
    for(int i = 0; i < GRIDSIZE; i++){
        for(int j = 0; j < GRIDSIZE; j++){
            int l = rand()&3;
            int t = rand()&3;
            float me[3] = {pos->r, pos->g, pos->b};
            float first[3] = {pos->neighbor[l]->r,pos->neighbor[l]->g,pos->neighbor[l]->b};
            float second[3] = {pos->neighbor[t]->r,pos->neighbor[t]->g,pos->neighbor[t]->b};
            V3normalize(first, first);
            V3normalize(me, me);
            V3normalize(second,second);
            float dot1= V3dotProduct(first, me);
            float dot2= V3dotProduct(second, me);

            temp[i][j].r = (pos->r + pos->neighbor[l]->r*pos->neighbor[l]->strength[0]*dot1 + pos->neighbor[t]->r*pos->neighbor[t]->strength[0]*dot2) /(1+pos->neighbor[l]->strength[0]*dot1+pos->neighbor[t]->strength[0]*dot2);
            temp[i][j].g = (pos->g + pos->neighbor[l]->g*pos->neighbor[l]->strength[0]*dot1 + pos->neighbor[t]->g*pos->neighbor[t]->strength[0]*dot2) /(1+pos->neighbor[l]->strength[0]*dot1+pos->neighbor[t]->strength[0]*dot2);
            temp[i][j].b = (pos->b + pos->neighbor[l]->b*pos->neighbor[l]->strength[0]*dot1 + pos->neighbor[t]->b*pos->neighbor[t]->strength[0]*dot2) /(1+pos->neighbor[l]->strength[0]*dot1+pos->neighbor[t]->strength[0]*dot2);
            pos = pos->neighbor[3];
        }
        pos = pos->neighbor[1];
    }
    for(int i = 0; i < GRIDSIZE; i++){
        for(int j = 0; j < GRIDSIZE; j++){
            pos->r = temp[i][j].r;
            pos->g = temp[i][j].g;
            pos->b = temp[i][j].b;
            pos = pos->neighbor[3];
        }
        pos = pos->neighbor[1];
    }

}
void displayScreen(struct pop s[GRIDSIZE][GRIDSIZE], struct WindowContext wc){
    GC gc = XDefaultGC(wc.display, 0);
   for(int y = 0; y < GRIDSIZE * 8; y++){
        for(int x = 0; x < GRIDSIZE * 8; x++){
            uint8_t r = floor(s[x >> 3][y >> 3].r);
            uint8_t g = floor(s[x >> 3][y >> 3].g);
            uint8_t b = floor(s[x >> 3][y >> 3].b);
            uint64_t color = (r << 16) | (g << 8) | b;
            XSetForeground(wc.display, gc, color);
            XDrawPoint(wc.display, wc.window, gc, x, y);
           // XDrawRectangle(wc.display, wc.window,gc, x*8, y*8, 8, 8);
        }
   }

}
void displayConsole(struct pop s[GRIDSIZE][GRIDSIZE]){
    printf("\n");
    for(unsigned int i = 0; i < GRIDSIZE; i++){
        printf("%2d: ", i); 
        for( unsigned int j = 0; j < GRIDSIZE; j++){
            printf("\033[38;2;%d;%d;%dm",(uint8_t)floor(s[i][j].r),(uint8_t)floor(s[i][j].g),(uint8_t)floor(s[i][j].b));
            printf("|||");
            //printf("%3.0f:%3.0f:%3.0f ", s[i][j].r, s[i][j].g, s[i][j].b);
        }
        printf("\033[39m\n");
    }
}
void navigate(struct pop* current){

    char buffer[3];

    for(;;){
        printf("Current object:\n  r:%f\n  g:%f\n  b:%f\n", current->r, current->g,current->b);
        read(0, &buffer ,sizeof(buffer));
        switch( buffer[0]) { 
            case 'h':
                current = current->neighbor[0];
                break;
            case 'j':
                current = current->neighbor[3];
                break;
            case 'k':
                current = current->neighbor[2];
                break;
            case 'l':
                current = current->neighbor[1];
                break;
            case 'q':
                return;
        }
    }
}
float V3dotProduct(float x[3], float y[3]){
   return (x[0]*y[0] + x[1]*y[1] + x[2]*y[2]);

}
void V3normalize(float x[3], float y[3]){
    float mag = sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]);
     y[0] = x[0]/mag;
     y[1] = x[1]/mag;
     y[2] = x[2]/mag;

}
float randRange(float start, float end){
    return (float)rand()/(float)RAND_MAX*(end-start) + start;
}
