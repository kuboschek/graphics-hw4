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

#define CUTOFF 0.00001

namespace March {
    const static int X = 0;
    const static int Y = 1;
    const static int Z = 2;

    float calcIntersectionOffset(float isovalue, int fi, int fj) {
        return fabs((fi - isovalue) / (fi - fj));
    }

    float linInterp(float i, float j, float offset) {
        return i * (1 - offset) + j * offset;
    }

    int getCubePoint(int xo, int yo, int zo, int vertex) {
        if (vertex > 7)
            return -1;

        return Volume::getPoint(xo + mc_vertices[vertex][X],
                                yo + mc_vertices[vertex][Y],
                                zo + mc_vertices[vertex][Z]);
    }

    int getMarchCase(int xo, int yo, int zo, int isovalue) {
        /*
        int mc_case = 0;

        // Iterate through all vertices of this cell
        for (int i = 0; i < 8; i++) {
            if (getCubePoint(xo, yo, zo, i) > isovalue) {
                SBI(mc_case, i);
            }
        }

        // Clamp max value
        return (mc_case & 0xFF);
        */


        int pos = 0;
        int Case = 0;

        for(int k = zo; k < zo+2; k++){
          for(int j = yo; j < yo+2; j++){
            for(int i = xo; i < xo+2; i++){
              int value = Volume::getPoint(i,j,k);
              int isInside = value <= isovalue ? 0 : 1;
              Case = Case | (isInside << pos++);
            }
          }
        }

        return Case;
    }


    void marchCube(int xo, int yo, int zo, float isovalue, std::vector<Triangle> &out) {
        // Marching cubes case
        int mc_case = getMarchCase(xo, yo, zo, isovalue);

        // Cube is completely outside / inside the object
        if (mc_case == 0 || mc_case == 255)
            return;

        // Offset of the intersection on any of the 12 edges
        float intersect[12] = {0};

        // Gradients at each vertex
        float gradient[8][3] = {0};

        // Iterate through all edges to compute intersection points
        for (int i = 0; i < 12; i++) {
            int v0 = mc_edges[i][0];
            int v1 = mc_edges[i][1];

            int fi = getCubePoint(xo, yo, zo, v0);
            int fj = getCubePoint(xo, yo, zo, v1);

            intersect[i] = calcIntersectionOffset(isovalue, fi, fj);
        }

        // Delta {x,y,z} for differencing
        int d = 1;

        // Generate gradients at each vertex
        for (int i = 0; i < sizeof(mc_vertices) / sizeof(mc_vertices[0]); ++i) {
            // Base coordinates
            int b[3] = {xo, yo, zo};

            // Vertex coordinates
            int p[3] = {
                    xo + mc_vertices[i][X],
                    yo + mc_vertices[i][Y],
                    zo + mc_vertices[i][Z]
            };

            // Bounds of the volume
            int max[3] = {Volume::width - 1, Volume::height - 1, Volume::depth - 1};

            // Run differencing in isolated fashion for all three axes
            for (int j = 0; j < 3; ++j) {
                // Multiplier, to select only current axis for differencing
                int m[3] = {j == 0 ? 1 : 0, j == 1 ? 1 : 0, j == 2 ? 1 : 0};

                if(p[j] > 0 && p[j] < max[j]) {
                    // Central differencing
                    gradient[i][j] = Volume::getPoint(p[X] + m[X] * d, p[Y] + m[Y] * d, p[Z] + m[Z] * d) -
                            Volume::getPoint(p[X] - m[X] * d, p[Y] - m[Y] * d, p[Z] - m[Z] * d);

                    gradient[i][j] /= (2 * d);
                } else if(p[j] == 0) {
                    // Forward differencing
                    gradient[i][j] = (Volume::getPoint(p[X] + m[X] * d, p[Y] + m[Y] * d, p[Z] + m[Z] * d) -
                            Volume::getPoint(p[X],p[Y],p[Z]))
                                     / (2 * d);
                } else {
                    // Backward differencing
                    gradient[i][j] = (Volume::getPoint(p[X],p[Y],p[Z]) -
                            Volume::getPoint(p[X] - m[X] * d, p[Y] - m[Y] * d, p[Z] - m[Z] * d))
                                     / (2 * d);
                }
            }
        }

        // Build triangles
        for (int i = 0; i < 16; i += 3) {
            if (mc_cases[mc_case][i] == -1) // Magic value -1: End of list
                break;

            Triangle t;       // The triangle we're going to add to the soup

            // Iterate through all 3 edges, set triangle points
            for (int j = 0; j < 3; j++) {
                int edge = mc_cases[mc_case][i + j];

                // Get vertex numbers of edge
                int v1 = mc_edges[edge][0];
                int v2 = mc_edges[edge][1];

                float point[3] = {
                        xo + mc_vertices[v1][0],
                        yo + mc_vertices[v1][1],
                        zo + mc_vertices[v1][2],
                };

                float otherPoint[3] = {
                        xo + mc_vertices[v2][0],
                        yo + mc_vertices[v2][1],
                        zo + mc_vertices[v2][2],
                };

                float yetAnotherPoint[3] = {0};


                float normal[3] = {
                  // Interpolation between gradients at verteces
                  linInterp(gradient[v1][0], gradient[v2][0], intersect[edge]),
                  linInterp(gradient[v1][1], gradient[v2][1], intersect[edge]),
                  linInterp(gradient[v1][2], gradient[v2][2], intersect[edge]),
                };

                // position = base cell coordinates + local vertex coordinates
                for (int k = 0; k < 3; k++) {
                  yetAnotherPoint[k] = linInterp(point[k], otherPoint[k], intersect[edge]);
                  double dump;
                }

                // Add point to triangle
                switch (j) {
                    case 0:
                        std::copy(yetAnotherPoint, yetAnotherPoint + 3, t.p1);
                        std::copy(normal, normal + 3, t.n1);
                        break;

                    case 1:
                        std::copy(yetAnotherPoint, yetAnotherPoint + 3, t.p2);
                        std::copy(normal, normal + 3, t.n2);
                        break;

                    case 2:
                        std::copy(yetAnotherPoint, yetAnotherPoint + 3, t.p3);
                        std::copy(normal, normal + 3, t.n3);
                        break;
                }
            } // Triangle builder - for j

            // Add triangle to output soup
            out.push_back(t);

        } // Cube marcher - for i
    }
}
