#include <iostream>

#ifndef __APPLE__
#include <cmath>
#endif

#include "ddsbase.h"

namespace Volume {
  unsigned int components;
  unsigned int width, height, depth;
  float scalex, scaley, scalez;
  unsigned char * volume;

  void loadVolume(const char *filename) {
    volume = readPVMvolume(filename, &width, &height, &depth, &components, &scalex, &scaley, &scalez);
  }

  bool loaded() {
    return (volume != NULL);
  }

  void setVolume(int w, int h, int d, unsigned char * v) {
    width = w;
    height = h;
    depth = d;
    volume = v;
  }

  unsigned int getPoint(int x, int y, int z) {
    if((x >= width) || (y >= height) || (z >= depth)) {
      std::cerr << "Point outside volume: " << x << ";" << y << ";" << z << ";" << std::endl;
    }

    return volume[x + y * width + z * width * height];
  }
}
