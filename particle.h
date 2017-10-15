#ifndef PARTICLE_H
#define PARTICLE_H

typedef struct Particle {
   float prevX;
   float prevY;
   float x;
   float y;
   float dx;
   float dy;
   int mode;
} Particle;

float getDX(int width, int height, float x,float y,float noise,int mode);
float getDY(int width, int height, float x,float y,float noise,int mode);
Particle newParticle(int width, int height, int noiseScale, int mode);
void getParticles(Particle * particles, int numParticles, int width, int height, int noiseScale, int mode);
void updateParticles(Particle * particles, int numParticles,int width,int height,int noiseScale);

#endif
