#include <vector>

// helper function addPoint
static void addPoint(std::vector<float> &xyz, std::vector<float> &uv, float point[2])
{
    xyz.insert(xyz.end(), point, point + 2); xyz.push_back(0.0f);
    uv.insert(uv.end(), point, point + 2);
} 

/** meshGeneration
 * function that generates an n x n square of triangles
 * @param   n   number of squares on a side
 * returns a vector of xyz tri positions followed by
 * corresponding uv positions
 **/
std::vector<float> meshGeneration(int n)
{
    std::vector<float> xyz;
    std::vector<float> uv;
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            float topLeft[2]        = { (float) i/n,       (float) j/n };
            float topRight[2]       = { (float) (i + 1)/n, (float) j/n };
            float bottomLeft[2]     = { (float) i/n,       (float) (j + 1)/n };
            float bottomRight[2]    = { (float) (i + 1)/n, (float) (j + 1)/n };

            addPoint(xyz, uv, topLeft);
            addPoint(xyz, uv, bottomLeft);
            addPoint(xyz, uv, bottomRight);
            addPoint(xyz, uv, topLeft);
            addPoint(xyz, uv, topRight);
            addPoint(xyz, uv, bottomRight);
        } 
    } 
    xyz.insert(xyz.end(), uv.begin(), uv.end());
    return xyz;
} 

