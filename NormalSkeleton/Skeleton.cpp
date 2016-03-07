/*
   Info on Readme.txt
*/

#include "ddsbase.h"
#include "OpenglSkeleton.h"
#include <iostream>


// Same as on frame work 1
void keypress(unsigned char key, int x, int y){

  
}

int main(int argc, char** argv){

   // Reading the data
   unsigned int width, height, depth, components;
   float scalex, scaley, scalez;
   unsigned char* volumeData = readPVMvolume(argv[1], &width, &height, &depth, &components, &scalex, &scaley, &scalez);
  
   OpenGLSkeleton::Initialize(argc, argv);
   
   OpenGLSkeleton::SetKeyboardInteraction(&keypress);
   
   std::vector<OpenGLSkeleton::Triangle> triangles;
   
    
   OpenGLSkeleton::SetSurface(&triangles);
   
   OpenGLSkeleton::Run();

   // OpenGLSkeleton::Redraw() can be called to force a redraw.     
}

