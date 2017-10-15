#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cairo.h>
#include <string.h>

#include "perlin.h"
#include "particle.h"
#include "color.h"

void drawBackground(cairo_t *cr,int w,int h,int v,double hue, double hueRange, double sat, double val);
void drawPoints(cairo_t *cr, Particle * particles, int numParticles, int useXORBlending);
void progressBar(char * label, int bars, double progress);

int main (int argc, char *argv[]) {
        srand(time(NULL));
        char *defaultOutputPath = "./filename.png";
        char *passedOutputPath = malloc(sizeof(char)*64);
        char *outputPath = defaultOutputPath;
        int outputWidth = 24000;
        int outputHeight = 15000;
        int noiseScale = 270;
        int numParticles = 256000;
        int particleFieldMode = 0;
        int particleSteps = 1000;
        int useXORBlending = 1;
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
        //Use blending with greater alpha if background is dark
        if (val < 0.2) useXORBlending = 0;
        Particle * particles = (Particle*) malloc(sizeof(Particle)*numParticles);
        cairo_surface_t *surface =
            cairo_image_surface_create (CAIRO_FORMAT_ARGB32, outputWidth, outputHeight);
        cairo_t *cr =
            cairo_create (surface);
        drawBackground(cr,outputWidth,outputHeight,noiseScale,hue,hueRange,sat,val);
        cairo_push_group(cr);
        getParticles(particles,numParticles,outputWidth,outputHeight,noiseScale,particleFieldMode);
        for (int i = 0;i<particleSteps;i++){
          if (i%10 == 0) {
            double progress = (double)i/(double)particleSteps;
            progressBar("Drawing Particles",25,progress);
            //printf("\r\e[KDrawing particles %d%%",i*100/l);
            fflush(stdout);
          }
          drawPoints(cr,particles,numParticles,useXORBlending);
          updateParticles(particles,numParticles,outputWidth,outputHeight,noiseScale,particleFieldMode);
        }
        progressBar("Drawing Particles",25,1);
        cairo_pop_group_to_source (cr);
        cairo_paint_with_alpha (cr, 1);

        cairo_destroy (cr);
        cairo_surface_write_to_png (surface, outputPath);
        cairo_surface_destroy (surface);
        printf("\n");
        return 0;
}

void drawBackground(cairo_t *cr,int w,int h, int v, double hue, double hueRange, double sat, double val) {
  for (int i=0;i<h;i++){
    if (i%10 == 0) {
      double progress = (double)i/(double)h;
      progressBar("Drawing background",25,progress);
    }
    for (int j=0;j<w;j++){
      HSV sourceHSV = {
        (perlin2d((float)j/v,(float)i/v,1,10)*hueRange+hue)*360,
        sat,
        val
      };
      RGB sourceRGB = hsvToRGB(sourceHSV);
      cairo_set_source_rgb(cr,sourceRGB.r,sourceRGB.g,sourceRGB.b);
      cairo_rectangle (cr, j, i, 1, 1);
      cairo_fill (cr);
    }
  }
  progressBar("Drawing Particles",25,1);
}
void drawPoints(cairo_t *cr, Particle * particles, int numParticles, int useXORBlending) {
  if (useXORBlending) cairo_set_operator(cr,CAIRO_OPERATOR_XOR);
  cairo_set_source_rgba(cr,1,1,1,0.05);
  for (int i = 0;i<numParticles;i++){
    Particle p = particles[i];
    cairo_move_to(cr,p.x_,p.y_);
    cairo_line_to(cr,p.x,p.y);
    cairo_stroke(cr);
  }
}

void progressBar(char * label, int bars, double progress) {
  if (progress == 0) {
    printf("\r\e[K\033[0m");
    fflush(stdout);
  }
  else if (progress == 1) {
    printf("\r\e[KComplete");
    fflush(stdout);
  }
  else {
    int currentStep = bars*progress;
    if (currentStep!=0) printf("\r\e[K%s: \033[22;32m[",label);
    else printf("\r\e[K%s: \033[22;32m[\033[0m",label);
    char * progressBar = malloc(sizeof(char)*(bars+1));
    for (int i = 0;i<bars;i++) {
      if (i+1<currentStep) printf("=");
      else if (i+1==currentStep) printf(">\033[0m");
      else printf("=");
    }
    printf("]");
    printf("\033[0m");
    int percent = (int)(progress*100);
    if (progress != 1) printf(" %d%%",percent);
    fflush(stdout);
  }
}
