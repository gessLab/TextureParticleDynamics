/*
 * Author: Henry Jochaniewicz
 * Date Modified: November 5, 2025
 */

#include <vector>
#include "mesh.hpp"

/** addPoint
 * function that adds a single point to a vector of xyz positions and a vector of uv positions
 * where the point (x, y) gets mapped to (x, 0, y) in the xyz vector and (x, y0) in the uv one
 * @param   posXYZ      vector<float> of xyz positions
 * @param   texCoordUV  vector<float> of texture coordinate uv positions
 * @param   min         minimum value to transform coordinate
 * @param   max         maximum value to transform coordinate
 * @param   newPos[2]   float array of the (x, y) coordinate point
 **/
void addPoint(std::vector<float> &posXYZ, std::vector<float> &texCoordUV, float min, float max, const float newPos[2])
{
    const float factor = max - min;
    posXYZ.push_back(newPos[0] * factor + min);
    posXYZ.push_back(0.0f);
    posXYZ.push_back(newPos[1] * factor + min);
    texCoordUV.insert(texCoordUV.end(), newPos, newPos + 2);
}

/** meshGeneration
 * function that generates an n x n square of triangles from
 * the coordinates (min, min) to (max, max)
 * @param   n   number of squares on a side
 * @param   min minimum coordinate to generate mesh
 * @param   max maximum coordinate to generate mesh
 * return   a vector of xyz tri positions followed by
 *          corresponding uv positions
 **/
std::vector<float> generateMesh(int n, float min, float max)
{
    std::vector<float> posXYZ;
    std::vector<float> texCoordUV;

    const float numSquares = static_cast<float>(n);

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            float x = static_cast<float>(i);
            float y = static_cast<float>(j);

            // we map from [0, 1] => [min, max]
            float topLeft[2]     = { x       / numSquares, y / numSquares };
            float topRight[2]    = { (x + 1) / numSquares, y / numSquares };
            float bottomLeft[2]  = { x       / numSquares, (y + 1) / numSquares };
            float bottomRight[2] = { (x + 1) / numSquares, (y + 1) / numSquares };

            addPoint(posXYZ, texCoordUV, min, max, topLeft);
            addPoint(posXYZ, texCoordUV, min, max, bottomLeft);
            addPoint(posXYZ, texCoordUV, min, max, topRight);
            addPoint(posXYZ, texCoordUV, min, max, topRight);
            addPoint(posXYZ, texCoordUV, min, max, bottomLeft);
            addPoint(posXYZ, texCoordUV, min, max, bottomRight);
        } 
    } 
    posXYZ.insert(posXYZ.end(), texCoordUV.begin(), texCoordUV.end());
    return posXYZ;
} 

