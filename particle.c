#include <stdlib.h>
#include <math.h>


#include "perlin.h"
#include "particle.h"

#define PI 3.14159265358979323846

float getDX(int w, int h, float x,float y,float n,int t) {
  switch(t){
    case 0:
      return cos(n*PI*2);
      break;
    case 1:
      return cos(n*PI*8)+1;
      break;
    case 2:
      return cos(n*PI*2)+(x-3*w/4)/(w/3);
      break;
    case 3:
      return cos(n*PI*2)+0.5;
      break;
    default:
      return cos(n*PI*2);
      break;
  }
}
float getDY(int w, int h, float x,float y,float n,int t){
  switch(t){
    case 0:
      return sin(n*PI*2);
      break;
    case 1:
      return tan(n*PI*8)/2;
      break;
    case 2:
      return sin(n*PI*2)+(y-2*h/3)/(w/3);
      break;
    case 3:
      return sin(n*PI*2);
      break;
    default:
      return sin(n*PI*2);
      break;
  }
}

Particle newParticle(int w,int h, int v, int t) {
  float x = rand()%(w+400)-200;
  float y = rand()%(h+400)-200;
  float n = perlin2d(x/v,y/v,1,1);
  float dx = getDX(w,h,x,y,n,t);
  float dy = getDY(w,h,x,y,n,t);
  Particle p = { x,
           x,
           y,
           y,
           dx,
           dy
         };
  return p;
}
void getParticles(Particle * arr, int size, int w, int h, int v, int t){
  for (int i = 0;i<size;i++){
    arr[i] = newParticle(w,h,v,t);
  }
}
void updateParticles(Particle * arr, int size,int w,int h,int v, int t){
  for (int i = 0;i<size;i++){
    Particle p = arr[i];
    p.x_ = p.x;
    p.y_ = p.y;
    p.x+=p.dx;
    p.y+=p.dy;
    float n = perlin2d(p.x/v,p.y/v,1,1);
    p.dx = getDX(w,h,p.x,p.y,n,t);
    p.dy = getDY(w,h,p.x,p.y,n,t);
    if (p.x < -200 || p.y < -200 || p.x > w+200 || p.y > h+200){
      p = newParticle(w,h,v,t);
    }
    arr[i] = p;
  }
}
