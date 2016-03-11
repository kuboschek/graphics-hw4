/*
   Info on Readme.txt
*/

#include "ddsbase.h"
#include "OpenglSkeleton.h"
#include <iostream>

#include "Volume.h"
#include "March.h"


float isovalue = 0;

// If true, shifts mesh to be centered around origin
bool shifting = false;

// Triangle soup stored here
std::vector<Triangle> triangles;

/**
 * @brief Shifts all triangles by
 */
void shift(Triangle& t) {
    float w2 = Volume::width / 2;
    float h2 = Volume::height / 2;
    float d2 = Volume::depth / 2;

    t.p1[0] -= w2;
    t.p1[1] -= h2;
    t.p1[2] -= d2;

    t.p2[0] -= w2;
    t.p2[1] -= h2;
    t.p2[2] -= d2;

    t.p3[0] -= w2;
    t.p3[1] -= h2;
    t.p3[2] -= d2;
}

void marchAllTheCubes(float isoval) {
    // Clear old soup -- so inefficient, caching is much better
    triangles.clear();

    // March everything
    // n^3 -- so what?
    for (unsigned int i = 0; i < Volume::width - 1; i++) { // x
        for (unsigned int j = 0; j < Volume::height - 1; j++) { // y
            for (unsigned int k = 0; k < Volume::depth - 1; k++) { // z
                March::marchCube(i, j, k, isoval, triangles);
            }
        }
    }

    if(shifting) {
        // Offset triangles
        for (int l = 0; l < triangles.size(); ++l) {
            // Subtract half of xyz-ranges from each point
            shift(triangles[l]);
        }
    }

    OpenGLSkeleton::SetSurface(&triangles);
}

// Same as on frame work 1
void keypress(unsigned char key, int x, int y) {
    switch (key) {
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

        case 's':
            shifting = !shifting;
            break;
    }

    if (isovalue < 0)
        isovalue = 0;

    if (isovalue > 255)
        isovalue = 255;

    marchAllTheCubes(isovalue);

    std::cout << "New isovalue: " << isovalue << std::endl;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Please specify dataset filename" << std::endl;
        return -1;
    }

    Volume::loadVolume(argv[1]);

    if (!Volume::loaded()) {
        std::cerr << "Could not load " << argv[1] << "." << std::endl;
        return -1;
    }

    if (Volume::components > 1) {
        std::cerr << "More than one component per voxel not supported." << std::endl;
        return -1;
    }


    OpenGLSkeleton::Initialize(argc, argv);

    OpenGLSkeleton::SetKeyboardInteraction(&keypress);

    // Run once on startup
    marchAllTheCubes(isovalue);

    OpenGLSkeleton::SetSurface(&triangles);

    OpenGLSkeleton::Run();

    OpenGLSkeleton::Redraw();
}
