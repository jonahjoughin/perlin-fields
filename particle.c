#include <stdlib.h>
#include <math.h>


#include "perlin.h"
#include "particle.h"

#define PI 3.14159265358979323846

//Get X component of particle's velocity
float getDX(int width, int height, float x,float y,float noise,int mode) {
  //Angle,rotation,x/width,y/height
  float a,r,xw,yh;
  switch(mode){
    case 0:
      return cos(noise*PI*2);
    case 1:
      return cos(noise*PI*8)+1;
    case 2:
      return cos(noise*PI*2)+(x-3*width/4)/(width/3);
    case 3:
      return sin(noise*PI*2);
    case 4:
      a = tan(noise*PI*8)*PI*2+5;
      r = sin(noise*PI*8);
      return cos(a)*r;
    case 5:
      a = cos(noise*PI*8)*PI*2+5;
      r = sin(noise*PI*8);
      return cos(a)*r;
    case 6:
      a = cos(noise*PI*8)*PI*2;
      r = sin(noise*PI*8);
      return cos(a)*r;
    case 7:
      a = tan(noise*PI*8)*PI*2;
      r = (double)height/1000;
      return cos(a)*r;
    case 8:
      return sin(noise*PI*2/2)*cos(noise*PI*2/3)*3;
    case 9:
      a = tan(noise*PI*8)*PI*2+5;
      r = sin(noise*PI*8);
      return cos(a)*r;
    default:
      return cos(noise*PI*2);
  }
}

//Get Y component of particle's velocity
float getDY(int width, int height, float x,float y,float noise,int mode){
  //Angle,rotation,x/width,y/height
  float a,r,xw,yh;
  switch(mode){
    case 0:
      return sin(noise*PI*2);
    case 1:
      return tan(noise*PI*8)/2;
    case 2:
      return sin(noise*PI*2)+(y-2*height/3)/(width/3);
    case 3:
      return tan(noise*PI*2);
    case 4:
      a = tan(noise*PI*8)*PI*4+5;
      r = sin(noise*PI*8);
      return sin(a)*r;
    case 5:
      a = cos(noise*PI*8)*PI*4+5;
      r = sin(noise*PI*8);
      return sin(a)*r;
    case 6:
      a = cos(noise*PI*8)*PI*2;
      r = sin(noise*PI*8);
      return sin(a)*r;
    case 7:
      a = tan(noise*PI*8)*PI*2;
      r = (double)height/1000;
      return sin(a)*r;
    case 8:
      return cos(noise*PI*2/2)*sin(noise*PI*2/3);
    case 9:
      yh = y/height;
      return cos(noise*PI*2/4)*sin(noise*PI*2/3)*(1+4*yh*yh*yh);
    default:
      return sin(noise*PI*2);
  }
}

//Create single particle using specified parameters
Particle newParticle(int width, int height, int noiseScale, int mode, int xoff, int yoff) {
  float x = rand()%(width+400)-200;
  float y = rand()%(height+400)-200;
  float noise = perlin2d(x/noiseScale+xoff,y/noiseScale+yoff,1,1);
  float dx = getDX(width,height,x,y,noise,mode);
  float dy = getDX(width,height,x,y,noise,mode);
  Particle p = { x, y, x, y, dx, dy, mode };
  return p;
}

//Fills Particle array particles using specified parameters
void getParticles(Particle * particles, int numParticles, int width, int height, int noiseScale, int mode, int xoff, int yoff){
  for (int i = 0;i<numParticles;i++){
    particles[i] = newParticle(width,height,noiseScale,mode,xoff,yoff);
  }
}
void updateParticles(Particle * particles, int numParticles,int width,int height,int noiseScale, int xoff, int yoff){
  for (int i = 0;i<numParticles;i++){
    //Update particles position, direction
    Particle p = particles[i];
    p.prevX = p.x;
    p.prevY = p.y;
    p.x+=p.dx;
    p.y+=p.dy;
    float noise = perlin2d(p.x/noiseScale+xoff,p.y/noiseScale+yoff,1,1);
    p.dx = getDX(width,height,p.x,p.y,noise,p.mode);
    p.dy = getDY(width,height,p.x,p.y,noise,p.mode);
    //Reset particle if out of bounds
    if (p.x < -200 || p.y < -200 || p.x > width+200 || p.y > height+200){
      p = newParticle(width,height,noiseScale,p.mode,xoff,yoff);
    }
    particles[i] = p;
  }
}
