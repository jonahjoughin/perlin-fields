#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cairo.h>
#include <math.h>
#include <string.h>

#include "perlin.h"
#include "particle.h"

void drawBackground(cairo_t *cr,int w,int h,int v,double hue, double hueRange, double sat, double val);
void drawPoints(cairo_t *cr, Particle * particles, int n, int xor);
float noise(float x, float y);
void progressBar(char * label, int steps, double progress);
void set_source_hsv(cairo_t *cr,double h, double s, double v);
void set_source_blend(cairo_t *cr,double h, double s, double v, double a);

int
main (int argc, char *argv[])
{

        //2384x3371
        char defaultPath[] = "filename.png";
        char *inputPath = malloc(sizeof(char)*64);
        char *path = defaultPath;
        //int w = 14043;
        int w = 24000;
        //int h = 9933;
        int h = 15000;
        int v = 800;
        int n = 256000;
        int t = 0;
        int steps = 1000;
        int xor = 1;
        double hue = 0.5;
        double hueRange = 0.05;
        double sat = 0.6;
        double val = 0.6;
        for(int i=0; i<argc; ++i) {
          if (strcmp(argv[i],"-w")==0) w = atoi(argv[i+1]);
          if (strcmp(argv[i],"-h")==0) h = atoi(argv[i+1]);
          if (strcmp(argv[i],"--hue")==0) hue = atof(argv[i+1]);
          if (strcmp(argv[i],"--hueRange")==0) hueRange = atof(argv[i+1]);
          if (strcmp(argv[i],"--sat")==0) sat = atof(argv[i+1]);
          if (strcmp(argv[i],"--val")==0) val = atof(argv[i+1]);
          if (strcmp(argv[i],"--steps")==0) steps = atoi(argv[i+1]);
          if (strcmp(argv[i],"-n")==0) n = atoi(argv[i+1]);
          if (strcmp(argv[i],"-v")==0) v = atoi(argv[i+1]);
          if (strcmp(argv[i],"-t")==0) t = atoi(argv[i+1]);
          if (strcmp(argv[i],"-f")==0) {
            strcpy(inputPath,argv[i+1]);
            path = inputPath;
          }
          if (strcmp(argv[i],"-s")==0) {
            double s = atof(argv[i+1]);
            w*=s;
            h*=s;
            v*=s;
            n*=(s*s);
          }
        }
        if (val < 0.2) xor = 0;
        Particle * particles = (Particle*) malloc(sizeof(Particle)*n);
        //RGB * pixelBounds = (RGB*) malloc(sizeof(RGB)*w*h);
        srand(time(NULL));
        cairo_surface_t *surface =
            cairo_image_surface_create (CAIRO_FORMAT_ARGB32, w, h);
        cairo_t *cr =
            cairo_create (surface);
        drawBackground(cr,w,h,v,hue,hueRange,sat,val);
        cairo_push_group(cr);
        getParticles(particles,n,w,h,v,t);
        for (int i = 0;i<steps;i++){
          if (i%10 == 0) {
            double progress = (double)i/(double)steps;
            progressBar("Drawing Particles",25,progress);
            //printf("\r\e[KDrawing particles %d%%",i*100/l);
            fflush(stdout);
          }
          drawPoints(cr,particles,n,xor);
          updateParticles(particles,n,w,h,v,t);
        }
        progressBar("Drawing Particles",25,1);
        cairo_pop_group_to_source (cr);
        cairo_paint_with_alpha (cr, 1);

        cairo_destroy (cr);
        cairo_surface_write_to_png (surface, path);
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
      set_source_hsv (cr, (perlin2d((float)j/v,(float)i/v,1,10)*hueRange+hue)*360, sat, val);
      cairo_rectangle (cr, j, i, 1, 1);
      cairo_fill (cr);
    }
  }
  progressBar("Drawing Particles",25,1);
}
void drawPoints(cairo_t *cr, Particle * particles, int n, int xor) {
  if (xor) cairo_set_operator(cr,CAIRO_OPERATOR_XOR);
  cairo_set_source_rgba(cr,1,1,1,0.05);
  for (int i = 0;i<n;i++){

    Particle p = particles[i];
    //set_source_blend (cr, (perlin2d(floor(p.x)/200,floor(p.y)/200,1,10)*0.2+0.5)*360, 0.6, 0.6,0.05);

    cairo_move_to(cr,p.x_,p.y_);
    cairo_line_to(cr,p.x,p.y);
    cairo_stroke(cr);
  }
}

void progressBar(char * label, int steps, double progress) {
  if (progress == 0) {
    printf("\r\e[K\033[0m");
    fflush(stdout);
  }
  else if (progress == 1) {
    printf("\r\e[KComplete");
    fflush(stdout);
  }
  else {
    int currentStep = steps*progress;
    if (currentStep!=0) printf("\r\e[K%s: \033[22;32m[",label);
    else printf("\r\e[K%s: \033[22;32m[\033[0m",label);
    char * progressBar = malloc(sizeof(char)*(steps+1));
    for (int i = 0;i<steps;i++) {
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

void set_source_hsv(cairo_t *cr,double h, double s, double v) {
	double r = 0, g = 0, b = 0;

	if (s == 0)
	{
		r = v;
		g = v;
		b = v;
	}
	else
	{
		int i;
		double f, p, q, t;

		if (h == 360)
			h = 0;
		else
			h /= 60;

		i = (int)trunc(h);
		f = h - i;

		p = v * (1.0 - s);
		q = v * (1.0 - (s * f));
		t = v * (1.0 - (s * (1.0 - f)));

		switch (i)
		{
		case 0:
			r = v;
			g = t;
			b = p;
			break;

		case 1:
			r = q;
			g = v;
			b = p;
			break;

		case 2:
			r = p;
			g = v;
			b = t;
			break;

		case 3:
			r = p;
			g = q;
			b = v;
			break;

		case 4:
			r = t;
			g = p;
			b = v;
			break;

		default:
			r = v;
			g = p;
			b = q;
			break;
		}
    cairo_set_source_rgb (cr, r, g, b);
	}
}
void set_source_blend(cairo_t *cr,double h, double s, double v, double a) {
  double r = 0, g = 0, b = 0;

	if (s == 0)
	{
		r = v;
		g = v;
		b = v;
	}
	else
	{
		int i;
		double f, p, q, t;

		if (h == 360)
			h = 0;
		else
			h /= 60;

		i = (int)trunc(h);
		f = h - i;

		p = v * (1.0 - s);
		q = v * (1.0 - (s * f));
		t = v * (1.0 - (s * (1.0 - f)));

		switch (i)
		{
		case 0:
			r = v;
			g = t;
			b = p;
			break;

		case 1:
			r = q;
			g = v;
			b = p;
			break;

		case 2:
			r = p;
			g = v;
			b = t;
			break;

		case 3:
			r = p;
			g = q;
			b = v;
			break;

		case 4:
			r = t;
			g = p;
			b = v;
			break;

		default:
			r = v;
			g = p;
			b = q;
			break;
		}
    r = r+(1-r)*0.5;
    g = g+(1-g)*0.5;
    b = b+(1-b)*0.5;
  }
  cairo_set_source_rgba (cr, r, g, b, a);
}
