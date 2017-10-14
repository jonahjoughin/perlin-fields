#ifndef PARTICLE_H
#define PARTICLE_H

typedef struct Particle {
   float x_;
   float x;
   float y_;
   float y;
   float dx;
   float dy;
} Particle;

float getDX(int w, int h, float x,float y,float n,int t);
float getDY(int w, int h, float x,float y,float n,int t);
Particle newParticle(int w, int h, int v, int t);
void getParticles(Particle * arr, int size, int w, int h, int v, int t);
void updateParticles(Particle * arr, int size,int w,int h,int v,int t);

#endif
