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


  std::vector<Triangle> marchCube(int xo, int yo, int zo, int isovalue) {
    // Output vector of triangles
    std::vector<Triangle> out;

    // Marching cubes case
    char mc_case;

    // Offset of the intersection on any of the 12 edges
    float intersect[12];

    // Iterate through all vertices of this cell
    for(int i = 0; i < sizeof(mc_vertices) / sizeof(int); i++) {
      if(getCubePoint(xo, yo, zo, i) > isovalue) {
        SBI(mc_case, i);
      } else {
        CBI(mc_case, i);
      }
    }

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

    // Build triangles
    for(int i = 0; i < 16; i += 3) {
      // Special value -1 is used to signify end of triangle list
      if(mc_cases[mc_case][i] == -1)
        break;

      // Iterate through all 3 edges, set triangle points
      for(int j = 0; j < 3; j++) {
        int edge = mc_cases[mc_case][i + j];

        // Get vertex numbers of edge
        int v1 = mc_edges[i + j][0];
        int v2 = mc_edges[i + j][1];

        // Get offsets for vertex
        // position = base cell coordinates + local vertex coordinates

        // add the computed intersection value to one of the positions

      }

      // TODO Find vertices corresponding to


      Triangle t;

      t.p1
    }


  }
}
