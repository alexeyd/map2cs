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

#include "mpoly.h"

CMapPolygon::CMapPolygon()
{
  m_baseplane = NULL;
}


CMapPolygon::CMapPolygon(const CMapTexturedPlane *baseplane)
{
  m_baseplane = baseplane;
}


CMapPolygon::CMapPolygon(const CMapPolygon &other)
{
  *this = other;
}


const CMapPolygon& CMapPolygon::operator = (const CMapPolygon &other)
{
  if(&other == this)
  {
    return *this;
  }

  m_baseplane = other.m_baseplane;
  m_vertices = other.m_vertices;

  return *this;
}


CMapPolygon::~CMapPolygon()
{
}


const csArray<csVector3>& CMapPolygon::GetVertices() const
{
  return m_vertices;
}


const CMapTexturedPlane* CMapPolygon::GetPlane() const
{
  return m_baseplane;
}


void CMapPolygon::AddVertex(const csVector3 &v)
{
  for(size_t i = 0; i < m_vertices.GetSize(); ++i)
  {
    if(csVector3::Norm(m_vertices[i] - v) < SMALL_EPSILON)
    {
      return;
    }
  }

  m_vertices.Push(v);
}


/* Check if p2 is right of (p0,p1) segment */
static inline float
IsRight(const csArray<csVector2> &verts, int i0, int i1, int i2)
{
  csVector2 e1 = verts[i2] - verts[i0];
  csVector2 e2 = verts[i1] - verts[i0];
  return e1.x * e2.y - e2.x * e1.y;
}
 

static void 
CorrectOrder(const csArray<csVector2> &verts, csArray<int> &inds)
{
  csArray<int> inds_deq;
  int top, bot;

  inds.DeleteAll();
  inds_deq.SetSize(2*verts.GetSize()+1);

  bot = verts.GetSize()-2;
  top = bot+3;

  inds_deq[bot] = inds_deq[top] = 2;

  if (IsRight(verts, 0, 1, 2) > 0) 
  {
    inds_deq[bot+1] = 0;
    inds_deq[bot+2] = 1;
  }
  else 
  {
    inds_deq[bot+1] = 1;
    inds_deq[bot+2] = 0;
  }

  for (int i=3; i < verts.GetSize(); i++) 
  {
    while (IsRight(verts, inds_deq[bot], inds_deq[bot+1], i) <= 0)
    {
      ++bot;
    }

    inds_deq[--bot] = i;

    while (IsRight(verts, inds_deq[top-1], inds_deq[top], i) <= 0)
    {
      --top;
    }

    inds_deq[++top] = i;
  }

  for (int i = 0; i <= top - bot; ++i)
  {
    inds.Push(inds_deq[bot + i]);
  }
}


// Project everything on a base plane and
// find the correct vertex order
void CMapPolygon::Finalize()
{
  csPlane3 plane(m_baseplane->GetPlane());
  csVector3 ox, oy, oz;
  csVector3 origin;

  if(m_vertices.GetSize() < 3)
  {
    m_vertices.DeleteAll();
    return;
  }

  origin = -plane.D() * plane.GetNormal();

  oz = plane.GetNormal();
  ox = csVector3::Unit(m_vertices[0] - origin);
  oy.Cross(oz, ox);
  oy.Normalize();

  CS::Math::Matrix4 base(ox.x, oy.x, oz.x, origin.x,
                         ox.y, oy.y, oz.y, origin.y,
                         ox.z, oy.z, oz.z, origin.z,
                         0.0 , 0.0 , 0.0 , 1.0);
  base.Invert();


  csArray<csVector2> v;
  csArray<int> inds;

  for(size_t i = 0; i < m_vertices.GetSize(); ++i)
  {
    csVector3 transformed(m_vertices[i] * base.GetTransform());
    v.Push(csVector2(transformed.x, transformed.y));
  }

  CorrectOrder(v, inds);

  csArray<csVector3> ordered_verts;

  for(size_t i = 0; i < inds.GetSize(); ++i)
  {
    ordered_verts.Push(m_vertices[inds[i]]);
  }

  m_vertices = ordered_verts;
}

