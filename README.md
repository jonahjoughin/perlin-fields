# Perlin Fields
Generative particle fields using perlin noise

## Getting Started
These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.
### Prerequisites
```
Cairo
```
### Installation
https://www.cairographics.org/download/
### Running
```bash
gcc -o main -I /opt/local/include/cairo -L /opt/local/lib -lcairo main.c perlin.c particle.c color.c
```
