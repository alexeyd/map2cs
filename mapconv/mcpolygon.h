#ifndef __MC_POLYGON_H__
#define __MC_POLYGON_H__

#include <crystalspace.h>

#include "mcmaptexturedplane.h"

const double MAX_WORLD_COORD = 65536.0;
const double MIN_WORLD_COORD = -65536.0;

class mcPolygon
{
  protected:
    const mcMapTexturedPlane *m_baseplane;
    csArray<csDVector3> m_vertices;
    csArray<csTriangle> m_triangles;

    void CutTriangle(size_t index, csArray<csTriangle> &new_triangles);

  public:
    mcPolygon();
    mcPolygon(const mcMapTexturedPlane *baseplane);
    mcPolygon(const mcPolygon &other);

    ~mcPolygon();

    const mcPolygon& operator = (const mcPolygon &other);

    void GetChopped(const csDPlane &plane);
    void Triangulate(double max_edge_length);

    const mcMapTexturedPlane* GetBaseplane() const;
    const csArray<csDVector3>& GetVertices() const;
    const csArray<csTriangle>& GetTriangles() const;
};

#endif // __MC_POLYGON_H__

