#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cairo.h>
#include <string.h>

#include "perlin.h"
#include "particle.h"
#include "color.h"

void drawBackground(cairo_t *cr,int width,int height,int noiseScale,double hue, double hueRange, double sat, double val, int xoff, int yoff);
void drawPoints(cairo_t *cr, Particle * particles, int numParticles, int useXORBlending, int useDarkParticles);
void drawParticle(cairo_t *cr, Particle particle);
void progressBar(char * label, int bars, int progress, int total);

int main (int argc, char *argv[]) {
  //Set rand seed
  srand(time(NULL));
  //Initialize variables
  char *defaultOutputPath = "./perlin-fields.png";
  char *passedOutputPath = malloc(sizeof(char)*64);
  char *outputPath = defaultOutputPath;
  int outputWidth = 24000;
  int outputHeight = 15000;
  int noiseScale = 270;
  int numParticles = 256000;
  int particleFieldMode = 0;
  int particleSteps = 1000;
  int useXORBlending = 1;
  int useDarkParticles = 0;
  int xoff = rand()%100;
  int yoff = rand()%100;
  double hue = 0.5;
  double hueRange = 0.05;
  double sat = 0.6;
  double val = 0.6;
  //Set variables from passed arguments
  for(int i=0; i<argc; ++i) {
    if (strcmp(argv[i],"width")==0) outputWidth = atoi(argv[i+1]);
    if (strcmp(argv[i],"height")==0) outputHeight = atoi(argv[i+1]);
    if (strcmp(argv[i],"hue")==0) hue = atof(argv[i+1]);
    if (strcmp(argv[i],"hueRange")==0) hueRange = atof(argv[i+1]);
    if (strcmp(argv[i],"sat")==0) sat = atof(argv[i+1]);
    if (strcmp(argv[i],"val")==0) val = atof(argv[i+1]);
    if (strcmp(argv[i],"steps")==0) particleSteps = atoi(argv[i+1]);
    if (strcmp(argv[i],"numParticles")==0) numParticles = atoi(argv[i+1]);
    if (strcmp(argv[i],"noiseScale")==0) noiseScale = atoi(argv[i+1]);
    if (strcmp(argv[i],"mode")==0) particleFieldMode = atoi(argv[i+1]);
    if (strcmp(argv[i],"output")==0) {
      strcpy(passedOutputPath,argv[i+1]);
      outputPath = passedOutputPath;
    }
    if (strcmp(argv[i],"scale")==0) {
      double s = atof(argv[i+1]);
      outputWidth*=s;
      outputHeight*=s;
      noiseScale*=s;
      numParticles*=(s*s);
    }
  }
  //Use blending with greater alpha if background is high contrast
  if (val < 0.2 || val > 0.9) useXORBlending = 0;
  if (val > 0.9) useDarkParticles = 1;

  Particle * particles = (Particle*) malloc(sizeof(Particle)*numParticles);
  getParticles(particles,numParticles,outputWidth,outputHeight,noiseScale,particleFieldMode,xoff,yoff);

  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, outputWidth, outputHeight);
  cairo_t *cr = cairo_create (surface);

  drawBackground(cr,outputWidth,outputHeight,noiseScale,hue,hueRange,sat,val,xoff,yoff);
  //Draws particles in separate layer before recompositing in order to blend alpha properly
  cairo_push_group(cr);
  for (int i = 0;i<particleSteps;i++){
    //Log Progress
    if (!i%10-9) progressBar("Drawing Particles",25,i+1,particleSteps);
    drawPoints(cr,particles,numParticles,useXORBlending, useDarkParticles);
    updateParticles(particles,numParticles,outputWidth,outputHeight,noiseScale,xoff,yoff);
  }

  //Push particles onto main layer
  cairo_pop_group_to_source (cr);
  cairo_paint_with_alpha (cr, 1);
  //Write to output path and exit
  cairo_destroy (cr);
  cairo_surface_write_to_png (surface, outputPath);
  cairo_surface_destroy (surface);
  return 0;
}

void drawBackground(cairo_t *cr, int width, int height, int noiseScale, double hue, double hueRange, double sat, double val, int xoff, int yoff) {
  for (int y=0;y<height;y++){
    //Log progress
    if (!y%10-9) progressBar("Drawing background",25,y+1,height);
    for (int x=0;x<width;x++){
      //Calculate hsv color and convert to rgb
      HSV sourceHSV = {
        perlin2d((float)x/noiseScale+xoff,(float)y/noiseScale+yoff,1,10)*hueRange+hue,
        sat,
        val
      };
      RGB sourceRGB = hsvToRGB(sourceHSV);
      //Draw pixel at x,y
      cairo_set_source_rgb(cr,sourceRGB.r,sourceRGB.g,sourceRGB.b);
      cairo_rectangle (cr, x, y, 1, 1);
      cairo_fill (cr);
    }
  }
}

void drawPoints(cairo_t *cr, Particle * particles, int numParticles, int useXORBlending, int useDarkParticles) {
  //Set blending mode and particle color
  if (useXORBlending) cairo_set_operator(cr,CAIRO_OPERATOR_XOR);
  if (useDarkParticles) cairo_set_source_rgba(cr,0,0,0,0.05);
  else cairo_set_source_rgba(cr,1,1,1,0.05);

  for (int i = 0;i<numParticles;i++) drawParticle(cr,particles[i]);
}

void drawParticle(cairo_t *cr, Particle particle) {
  //Draw line from particle's previous position to current position
  cairo_move_to(cr,particle.prevX,particle.prevY);
  cairo_line_to(cr,particle.x,particle.y);
  cairo_stroke(cr);
}

void progressBar(char * label, int bars, int progress, int total) {
  double decimalProgress = (double)progress/total;
  //Clear line and reset to default color
  if (decimalProgress == 0) printf("\r\e[K\033[0m");
  //Clear line and signal completino
  else if (decimalProgress == 1) printf("\r\e[K%s: \033[22;32mComplete\033[0m\n",label);
  //Show progress bar
  else {
    //Clear line and how label
    printf("\r\e[K%s: \033[22;32m[",label);

    char * progressBar = malloc(sizeof(char)*(bars+1));
    int currentStep = bars*decimalProgress;

    for (int i = 0;i<bars;i++) {
      if (i==currentStep)printf("\033[0m=");
      else if (i+1==currentStep) printf(">");
      else printf("=");
    }
    //Ensure reset to default color
    printf("]\033[0m");
    //Show progress as percentage
    int percentProgress = (int)(decimalProgress*100);
    if (decimalProgress != 1) printf(" %d%%",percentProgress);
  }
  fflush(stdout);
}
