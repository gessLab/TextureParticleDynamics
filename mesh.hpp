/*
 * Author: Henry Jochaniewicz
 * Date Modified: November 5, 2025
 */

#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <vector>

/** addPoint
 * function that adds a single point to a vector of xyz positions and a vector of uv positions
 * where the point (x, y) gets mapped to (x, 0, y) in the xyz vector and (x, y0) in the uv one
 * @param   posXYZ      vector<float> of xyz positions
 * @param   texCoordUV  vector<float> of texture coordinate uv positions
 * @param   newPos[2]   float array of the (x, y) coordinate point
 **/
std::vector<float> generateMesh(int n, float min, float max);

#endif

