#include <iostream>

#ifndef __APPLE__
#include <cmath>
#endif

#include "Triangle.h"
#include "MarchCases.h"


// Set bit, clear bit, get bit
#define SBI(x, b) (x |= (1 << b))
#define CBI(x, b) (x &= ~(1 << b))
#define GBI(x, b) (x & (1 << b))

namespace March {
  float calcIntersectionOffset(int isovalue, int fi, int fj) {
    return (float) (fi - isovalue) / (fi - fj);
  }

  int getCubePoint(int xo, int yo, int zo, int vertex) {
    if(vertex > 7)
      return -1;

    return Volume::getPoint(xo + mc_vertices[vertex][0],
                            yo + mc_vertices[vertex][1],
                            zo + mc_vertices[vertex][2]);
  }

  int getMarchCase(int xo, int yo, int zo, int isovalue) {
    char mc_case;

    // Iterate through all vertices of this cell
    for(int i = 0; i < sizeof(mc_vertices) / sizeof(int); i++) {
      if(getCubePoint(xo, yo, zo, i) > isovalue) {
        SBI(mc_case, i);
      } else {
        CBI(mc_case, i);
      }
    }

    return mc_case;
  }


  std::vector<Triangle> marchCube(int xo, int yo, int zo, int isovalue) {
    // Output vector of triangles
    std::vector<Triangle> out;

    // Marching cubes case
    char mc_case = getMarchCase(xo,yo,zo,isovalue);

    // Offset of the intersection on any of the 12 edges
    float intersect[12];

    // Iterate through all edges to compute intersection points
    for(int i = 0; i < sizeof(mc_edges) / sizeof(int); i++) {
      int v0 = mc_edges[i][0];
      int v1 = mc_edges[i][1];

      if(GBI(mc_case, v0) != GBI(mc_case, v1)) {
        int fi = getCubePoint(xo, yo, zo, v0);
        int fj = getCubePoint(xo, yo, zo, v1);

        intersect[i] = calcIntersectionOffset(isovalue, fi, fj);
      }
    }

    int pointCtr = 0;

    // Build triangles
    for(int i = 0; i < 16; i += 3) {
      if(mc_cases[mc_case][i] == -1) // Magic value -1: End of list
        break;

      int pointCtr = 0; // The point in the triangle that we are on
      Triangle t;       // The triangle we're going to add to the soup

      // Iterate through all 3 edges, set triangle points
      for(int j = 0; j < 3; j++) {
        int edge = mc_cases[mc_case][i + j];

        // Get vertex numbers of edge
        int v1 = mc_edges[edge][0];
        int v2 = mc_edges[edge][1];

        // Get local coordinates for vertex
        int* p1 = mc_vertices[v1];
        int* p2 = mc_vertices[v2];

        float point[3] = {
          xo + p1[0],
          yo + p1[1],
          zo + p1[2],
        };

        // position = base cell coordinates + local vertex coordinates
        // add the computed intersection value to one of the positions
        for(int k = 0; k < 3; k++) {
          if(p1[k] != p2[k]) {
            point[k] += intersect[edge];
            break;
          }
        }

        // Add point to triangle
        switch (j) {
          case 0:
            std::copy(point, point + 3, t.p1);
            break;

          case 1:
            std::copy(point, point + 3, t.p2);
            break;

          case 2:
            std::copy(point, point + 3, t.p3);
            break;
        }
      } // Triangle builder - for j

      // Add triangle to output soup
      out.push_back(t);

    } // Cube marcher - for i

  }

  return out;
}
