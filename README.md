# Perlin Fields
Generative particle fields using perlin noise

## Sample
[![Perlin Fields](https://i.imgur.com/jtXQeXT.jpg)]()


## Getting Started
These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.
### Prerequisites
```
Cairo
```
### Prerequisite Installation
https://www.cairographics.org/download/
### Compiling
```bash
gcc -o main -I /opt/local/include/cairo -L /opt/local/lib -lcairo main.c perlin.c particle.c color.c
```
### Running
```bash
./main output /path/to/output.png
```
### Command Line Aruments
- width: set width of output in px
- height: set height of output in px
- hue: set hue of background (0-1)
- sat: set saturation of background (0-1)
- val: set value of background (0-1)
- steps: set number of steps particles take
- numParticles: set number of particles
- noiseScale: set scale of perlin noise used
- mode: set mode of particle motion
- output: set output image path
- scale: scale output image (adjusts width, height, and noiseScale automatically)
