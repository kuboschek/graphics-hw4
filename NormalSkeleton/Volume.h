#include <iostream>

#ifndef __APPLE__
#include <cmath>
#endif

namespace Volume {
  int width, height, depth;
  unsigned char * volume;

  void setVolume(int w, int h, int d, unsigned char * v) {
    width = w;
    height = h;
    depth = d;
    volume = v;
  }

  unsigned int getPoint(int x, int y, int z) {
    if((x > width) || (y > height) || (z > depth)) {
      std::cerr << "Point outside volume: " << x << ";" << y << ";" << z << ";" << std::endl;
    }

    return volume[x + y * width + z * width * height];
  }
}
