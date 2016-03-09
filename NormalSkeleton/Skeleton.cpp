/*
   Info on Readme.txt
*/

#include "ddsbase.h"
#include "OpenglSkeleton.h"
#include <iostream>

#include "Volume.h"
#include "March.h"

/*
// Sorry for these globals
unsigned int width, height, depth, components;
float scalex, scaley, scalez;
unsigned char* volumeData;
*/

float isovalue = 150;
std::vector<Triangle> triangles;

void marchAllTheCubes(float isoval) {
  // Clear old soup -- so inefficient, caching is much better
  triangles.clear();

  // March everything
  // n^3 -- so what?
  for (unsigned int i = 0; i < Volume::width - 1; i++) { // x
    for (unsigned int j = 0; j < Volume::height - 1; j++) { // y
      for (unsigned int k = 0; k < Volume::depth - 1; k++) { // z
        March::marchCube(i,j,k,isoval,triangles);
      }
    }
  }

  std::cout << ".";

  OpenGLSkeleton::SetSurface(&triangles);
}

// Same as on frame work 1
void keypress(unsigned char key, int x, int y){
    switch(key) {
    case 'u':
      isovalue += 0.1;
      break;

    case 'j':
      isovalue -= 0.1;
      break;

    case 'i':
      isovalue += 1.0;
      break;

    case 'k':
      isovalue -= 1.0;
      break;

    case 'o':
      isovalue += 10;
      break;

    case 'l':
      isovalue -= 10;
      break;
    }

    if(isovalue < 0)
        isovalue = 0;

    if(isovalue > 255)
        isovalue = 255;

    marchAllTheCubes(isovalue);

    std::cout << "New isovalue: " << isovalue << std::endl;
}

int main(int argc, char** argv){
  if(argc < 2) {
    std::cerr << "Please specify dataset filename" << std::endl;
    return -1;
  }

   // Reading the data
   //volumeData = readPVMvolume(argv[1], &width, &height, &depth, &components, &scalex, &scaley, &scalez);
   Volume::loadVolume(argv[1]);

   if(!Volume::loaded()) {
     std::cerr << "Could not load " << argv[1] << "." << std::endl;
     return -1;
   }

   if(Volume::components > 1) {
     std::cerr << "More than one component per voxel not supported." << std::endl;
     return -1;
   }

   std::cout << "W, H, D: "
             << Volume::width << " "
             << Volume::height << " "
             << Volume::depth << std::endl;



   OpenGLSkeleton::Initialize(argc, argv);

   OpenGLSkeleton::SetKeyboardInteraction(&keypress);


   OpenGLSkeleton::SetSurface(&triangles);

   OpenGLSkeleton::Run();

   OpenGLSkeleton::Redraw();
}
