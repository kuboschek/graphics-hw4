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
    float calcIntersectionOffset(float isovalue, int fi, int fj) {
        if (fabs(fi - fj) < CUTOFF)
            return 0.5;

        if (fabs(isovalue - fi) < CUTOFF)
            return 0;

        if (fabs(isovalue - fj) < CUTOFF)
            return 1;

        return (float) (fi - isovalue) / (fi - fj);
    }

    // Only allow numerical types in this template
    template<
            typename T,
            typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
    >
    T linInterp(T i, T j, float offset) {
        return i * (1 - offset) + j * offset;
    }

    int getCubePoint(int xo, int yo, int zo, int vertex) {
        if (vertex > 7)
            return -1;

        return Volume::getPoint(xo + mc_vertices[vertex][0],
                                yo + mc_vertices[vertex][1],
                                zo + mc_vertices[vertex][2]);
    }

    int getMarchCase(int xo, int yo, int zo, int isovalue) {
        int mc_case = 0;

        // Iterate through all vertices of this cell
        for (int i = 0; i < sizeof(mc_vertices) / sizeof(int); i++) {
            if (getCubePoint(xo, yo, zo, i) > isovalue) {
                SBI(mc_case, i);
            } else {
                CBI(mc_case, i);
            }
        }

        // Clamp max value
        return (mc_case & 0xFF);
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
        for (int i = 0; i < sizeof(mc_edges[0]) / sizeof(int); i++) {
            int v0 = mc_edges[i][0];
            int v1 = mc_edges[i][1];

            if ((GBI(mc_case, v0) - GBI(mc_case, v1)) != 0) {
                int fi = getCubePoint(xo, yo, zo, v0);
                int fj = getCubePoint(xo, yo, zo, v1);

                intersect[i] = calcIntersectionOffset(isovalue, fi, fj);
            }
        }

        // Delta {x,y,z} for differencing
        int d = 1;

        // Generate gradients at each vertex
        for (int i = 0; i < sizeof(mc_vertices) / sizeof(mc_vertices[0]); ++i) {
            // Base coordinates
            int b[3] = {xo, yo, zo};

            // Vertex coordinates
            int p[3] = {
                    xo + mc_vertices[i][0],
                    yo + mc_vertices[i][1],
                    zo + mc_vertices[i][2]
            };

            // Bounds of the volume
            int max[3] = {Volume::width, Volume::height, Volume::depth};

            // Run differencing in isolated fashion for all three axes
            for (int j = 0; j < 3; ++j) {
                // Multiplier, to select only current axis for differencing
                int m[3] = {j == 0 ? 1 : 0, j == 1 ? 1 : 0, j == 2 ? 1 : 0};

                if(p[j] > 0 && p[j] < (max[j] - 1)) {
                    // Central differencing
                    gradient[i][j] = Volume::getPoint(p[0] + m[0] * d, p[1] + m[1] * d, p[2] + m[2] * d) +
                            Volume::getPoint(p[0] - m[0] * d, p[1] - m[1] * d, p[2] - m[2] * d);

                    gradient[i][j] /= (2 * d);
                } else if(p[j] == 0) {
                    // Forward differencing
                    gradient[i][j] = (Volume::getPoint(p[0] + m[0] * d, p[1] + m[1] * d, p[2] + m[2] * d) -
                            Volume::getPoint(p[0],p[1],p[2]))
                                     / d;
                } else {
                    // Backward differencing
                    gradient[i][j] = (Volume::getPoint(p[0],p[1],p[2]) -
                            Volume::getPoint(p[0] - m[0] * d, p[1] - m[1] * d, p[2] - m[2] * d))
                                     / d;
                }
            }
        }

        int pointCtr = 0;

        // Build triangles
        for (int i = 0; i < 16; i += 3) {
            if (mc_cases[mc_case][i] == -1) // Magic value -1: End of list
                break;

            int pointCtr = 0; // The point in the triangle that we are on
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

                // position = base cell coordinates + local vertex coordinates
                // add the computed intersection value to one of the positions
                for (int k = 0; k < 3; k++) {
                    if (mc_vertices[v1][k] != mc_vertices[v2][k]) {
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
}
