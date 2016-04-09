/*
   Info on Readme.txt
*/

#include "ddsbase.h"
#include "OpenglSkeleton.h"
#include <iostream>

#include "Volume.h"
#include "March.h"


float isovalue = 0;
float lambda = 0.2;

// If true, shifts mesh to be centered around origin
bool shifting = false;

// If true, negate normals
bool negating = false;

// Simple point class
class Point {
public:
  Point(int x, int y, int z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }
  int x;
  int y;
  int z;
};


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

void negNormals(Triangle& t) {
    t.n1[0] *= -1;
    t.n1[1] *= -1;
    t.n1[2] *= -1;

    t.n2[0] *= -1;
    t.n2[1] *= -1;
    t.n2[2] *= -1;

    t.n3[0] *= -1;
    t.n3[1] *= -1;
    t.n3[2] *= -1;
}

void marchAllTheCubes(float isoval) {
    // Clear old soup -- so inefficient, caching is much better
    triangles.clear();

    // Stores which cells have snapped points
    std::vector<Point> editList;

    // Clean up from last time
    Volume::resetSnapVolume();

    // Step 1: Snap points that are close to grid (solution to part a)
    for (unsigned int i = 0; i < Volume::width - 1; i++) { // x
        for (unsigned int j = 0; j < Volume::height - 1; j++) { // y
            for (unsigned int k = 0; k < Volume::depth - 1; k++) { // z
                if(March::snapPoints(i, j, k, isoval, lambda)) {
                  editList.push_back(Point(i, j, k));
                }
            }
        }
    }

    // Step 2: Run Marching Cubes with snapped volume data
    for (unsigned int i = 0; i < Volume::width - 1; i++) { // x
        for (unsigned int j = 0; j < Volume::height - 1; j++) { // y
            for (unsigned int k = 0; k < Volume::depth - 1; k++) { // z
                March::marchCube(i, j, k, isoval, triangles, Volume::getSnapPoint);
            }
        }
    }

    // THIS IMPLEMENTATION GENERATES A LOT OF COMPILE ERRORS; SIMPLY SAID, IT DOESN'T YET WORK
    // Step 3: Move isosurface points that lie on grid vertices back to orig. place
    /*
    for(Triangle &tri : triangles) {
      double p [3] = {0};
      double epsilon = 0.0001;

      std::vector<Triangle> cubes;

      cubes.clear();
      // Get original MC points if we have a grid-aligned point in this triangle
      if(modf(tri.p1[0], &p[0]) < epsilon ||
         modf(tri.p1[1], &p[1]) < epsilon ||
         modf(tri.p1[2], &p[2]) < epsilon) {
           March::marchCube(p[0], p[1], p[2], isoval, cubes, Volume::getPoint);

         float min = 1.0; // Essentially any other value is going to be lower
         int i, idx = 0, p = 0;

         std::vector<Triangle>::iterator it;

         for(it = cubes.begin(); it < cubes.end(); it++, i++) {
           float d1 = sqrt(pow(it->p1[0] - p[0], 2) +
                           pow(it->p1[1] - p[1], 2) +
                           pow(it->p1[2] - p[2], 2));

           float d2 = sqrt(pow(it->p2[0] - p[0], 2) +
                           pow(it->p2[1] - p[1], 2) +
                           pow(it->p2[2] - p[2], 2));

           float d3 = sqrt(pow(it->p3[0] - p[0], 2) +
                           pow(it->p3[1] - p[1], 2) +
                           pow(it->p3[2] - p[2], 2));

           if(d1 < min) {
              idx = i;
              min = d1;
              p = 1;
           }

           if(d2 < min) {
              idx = i;
              min = d2;
              p = 2;
           }

           if(d3 < min) {
              idx = i;
              min = d3;
              p = 3;
           }
         }

         switch (p) {
           case 1:
            p[0] = cubes[idx].p1[0];
            p[1] = cubes[idx].p1[1];
            p[2] = cubes[idx].p1[2];
            break;

          case 2:
           p[0] = cubes[idx].p2[0];
           p[1] = cubes[idx].p2[1];
           p[2] = cubes[idx].p2[2];
           break;

           case 3:
            p[0] = cubes[idx].p3[0];
            p[1] = cubes[idx].p3[1];
            p[2] = cubes[idx].p3[2];
            break;
         }
      }

      cubes.clear();
      if(modf(tri.p2[0], &p[0]) < epsilon ||
         modf(tri.p2[1], &p[1]) < epsilon ||
         modf(tri.p2[2], &p[2]) < epsilon) {
           March::marchCube(p[0], p[1], p[2], isoval, cubes, Volume::getPoint);

           float min = 1.0; // Essentially any other value is going to be lower
           int i, idx = 0, p = 0;

           std::vector<Triangle>::iterator it;

           for(it = cubes.begin(); it < cubes.end(); it++, i++) {
             float d1 = sqrt(pow(it->p1[0] - p[0], 2) +
                             pow(it->p1[1] - p[1], 2) +
                             pow(it->p1[2] - p[2], 2));

             float d2 = sqrt(pow(it->p2[0] - p[0], 2) +
                             pow(it->p2[1] - p[1], 2) +
                             pow(it->p2[2] - p[2], 2));

             float d3 = sqrt(pow(it->p3[0] - p[0], 2) +
                             pow(it->p3[1] - p[1], 2) +
                             pow(it->p3[2] - p[2], 2));

             if(d1 < min) {
                idx = i;
                min = d1;
                p = 1;
             }

             if(d2 < min) {
                idx = i;
                min = d2;
                p = 2;
             }

             if(d3 < min) {
                idx = i;
                min = d3;
                p = 3;
             }
           }

           switch (p) {
             case 1:
              p[0] = cubes[idx].p1[0];
              p[1] = cubes[idx].p1[1];
              p[2] = cubes[idx].p1[2];
              break;

            case 2:
             p[0] = cubes[idx].p2[0];
             p[1] = cubes[idx].p2[1];
             p[2] = cubes[idx].p2[2];
             break;

             case 3:
              p[0] = cubes[idx].p3[0];
              p[1] = cubes[idx].p3[1];
              p[2] = cubes[idx].p3[2];
              break;
           }
        }
      }

      // TODO Same as above

      cubes.clear();
      if(modf(tri.p3[0], &p[0]) < epsilon ||
         modf(tri.p3[1], &p[1]) < epsilon ||
         modf(tri.p3[2], &p[2]) < epsilon) {
           March::marchCube(p[0], p[1], p[2], isoval, cubes, Volume::getPoint);

           float min = 1.0; // Essentially any other value is going to be lower
           int i, idx = 0, p = 0;

           std::vector<Triangle>::iterator it;

           for(it = cubes.begin(); it < cubes.end(); it++, i++) {
             float d1 = sqrt(pow(it->p1[0] - p[0], 2) +
                             pow(it->p1[1] - p[1], 2) +
                             pow(it->p1[2] - p[2], 2));

             float d2 = sqrt(pow(it->p2[0] - p[0], 2) +
                             pow(it->p2[1] - p[1], 2) +
                             pow(it->p2[2] - p[2], 2));

             float d3 = sqrt(pow(it->p3[0] - p[0], 2) +
                             pow(it->p3[1] - p[1], 2) +
                             pow(it->p3[2] - p[2], 2));

             if(d1 < min) {
                idx = i;
                min = d1;
                p = 1;
             }

             if(d2 < min) {
                idx = i;
                min = d2;
                p = 2;
             }

             if(d3 < min) {
                idx = i;
                min = d3;
                p = 3;
             }
           }

           switch (p) {
             case 1:
              p[0] = cubes[idx].p1[0];
              p[1] = cubes[idx].p1[1];
              p[2] = cubes[idx].p1[2];
              break;

            case 2:
             p[0] = cubes[idx].p2[0];
             p[1] = cubes[idx].p2[1];
             p[2] = cubes[idx].p2[2];
             break;

             case 3:
              p[0] = cubes[idx].p3[0];
              p[1] = cubes[idx].p3[1];
              p[2] = cubes[idx].p3[2];
              break;
           }
        }
      }

      // TODO Same as above
    }
    */

    if(shifting) {
        // Offset triangles
        for (int l = 0; l < triangles.size(); ++l) {
            // Subtract half of xyz-ranges from each point
            shift(triangles[l]);
        }
    }

    if(negating) {
        // Offset triangles
        for (int l = 0; l < triangles.size(); ++l) {
            // Subtract half of xyz-ranges from each point
            negNormals(triangles[l]);
        }
    }

    // Step 4: Rendering the isosurface. (solution to d)
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

        case 'n':
            negating = !negating;
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
