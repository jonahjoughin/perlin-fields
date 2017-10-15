#include <stdlib.h>
#include <math.h>


#include "perlin.h"
#include "particle.h"

#define PI 3.14159265358979323846

float getDX(int width, int height, float x,float y,float noise,int mode) {
  switch(mode){
    case 0:
      return cos(noise*PI*2);
    case 1:
      return cos(noise*PI*8)+1;
    case 2:
      return cos(noise*PI*2)+(x-3*width/4)/(width/3);
    case 3:
      return cos(noise*PI*2)+0.5;
    default:
      return cos(noise*PI*2);
  }
}
float getDY(int width, int height, float x,float y,float noise,int mode){
  switch(mode){
    case 0:
      return sin(noise*PI*2);
    case 1:
      return tan(noise*PI*8)/2;
    case 2:
      return sin(noise*PI*2)+(y-2*height/3)/(width/3);
    case 3:
      return sin(noise*PI*2);
    default:
      return sin(noise*PI*2);
  }
}

Particle newParticle(int width, int height, int noiseScale, int mode) {
  float x = rand()%(width+400)-200;
  float y = rand()%(height+400)-200;
  float noise = perlin2d(x/noiseScale,y/noiseScale,1,1);
  float dx = getDX(width,height,x,y,noise,mode);
  float dy = getDX(width,height,x,y,noise,mode);
  Particle p = { x, y, x, y, dx, dy, mode };
  return p;
}
void getParticles(Particle * particles, int numParticles, int width, int height, int noiseScale, int mode){
  for (int i = 0;i<numParticles;i++){
    particles[i] = newParticle(width,height,noiseScale,mode);
  }
}
void updateParticles(Particle * particles, int numParticles,int width,int height,int noiseScale){
  for (int i = 0;i<numParticles;i++){
    Particle p = particles[i];
    p.prevX = p.x;
    p.prevY = p.y;
    p.x+=p.dx;
    p.y+=p.dy;
    float noise = perlin2d(p.x/noiseScale,p.y/noiseScale,1,1);
    p.dx = getDX(width,height,p.x,p.y,noise,p.mode);
    p.dy = getDY(width,height,p.x,p.y,noise,p.mode);
    if (p.x < -200 || p.y < -200 || p.x > width+200 || p.y > height+200){
      p = newParticle(width,height,noiseScale,p.mode);
    }
    particles[i] = p;
  }
}
