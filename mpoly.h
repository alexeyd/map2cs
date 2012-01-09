/*
    Map2cs: a convertor to convert the frequently used MAP format, into
    something, that can be directly understood by Crystal Space.

    Copyright (C) 1999 Thomas Hieber (thieber@gmx.net)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __MPOLY_H__
#define __MPOLY_H__

#include <crystalspace.h>

#include "texplane.h"

const double MAX_WORLD_COORD = 65536.0;
const double MIN_WORLD_COORD = -65536.0;

class mcPolygon
{
  protected:
    const CMapTexturedPlane *m_baseplane;
    csArray<csDVector3> m_vertices;
    csArray<csTriangle> m_triangles;

    void CutTriangle(size_t index, csArray<csTriangle> &new_triangles);

  public:
    mcPolygon();
    mcPolygon(const CMapTexturedPlane *baseplane);
    mcPolygon(const mcPolygon &other);

    ~mcPolygon();

    const mcPolygon& operator = (const mcPolygon &other);

    void GetChopped(const csDPlane &plane);
    void Triangulate(double max_edge_length);

    const CMapTexturedPlane* GetBaseplane() const;
    const csArray<csDVector3>& GetVertices() const;
    const csArray<csTriangle>& GetTriangles() const;
};

#endif // __MPOLY_H__

