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

static inline int DominantAxis(const csDVector3 &v)
{
  if ( (fabs(v.x) > fabs(v.y)) && 
       (fabs(v.x) > fabs(v.z)) )
  {
    return CS_AXIS_X;
  }
  else if (fabs(v.y) > fabs(v.z))
  {
    return CS_AXIS_Y;
  }

  return CS_AXIS_Z;
}


CMapPolygon::CMapPolygon()
{
  m_baseplane = NULL;
}


CMapPolygon::CMapPolygon(const CMapTexturedPlane *baseplane)
{
  m_baseplane = baseplane;

  const csDPlane *plane = &(baseplane->GetPlane());

  csDVector3 up, right;
  csDVector3 vert, origin;

  switch( DominantAxis(plane->Normal()) )
  {
    case CS_AXIS_X:
    case CS_AXIS_Y:
      up.x = 0.0;
      up.y = 0.0;
      up.z = 1.0;
      break;

    case CS_AXIS_Z:
      up.x = 1.0;
      up.y = 0.0;
      up.z = 0.0;
      break;

    default:
      CS_ASSERT(0);
      break;
  }

  double bend_norm = up * plane->Normal();
  csDVector3 bend_vec = plane->Normal() * bend_norm;
  up -= bend_vec;
  up.Normalize();

  right.Cross(up, plane->Normal());
  right.Normalize();

  up *= MAX_WORLD_COORD * 2.0;
  right *= MAX_WORLD_COORD * 2.0;

  origin = -plane->Normal() * plane->D();

  vert = origin - right + up;
  m_vertices.Push(vert);

  vert = origin + right + up;
  m_vertices.Push(vert);

  vert = origin + right - up;
  m_vertices.Push(vert);

  vert = origin - right - up;
  m_vertices.Push(vert);
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

/* Shinigami chop!!! */
void CMapPolygon::GetChopped(const csDPlane &plane)
{
  csArray<csDVector3> new_vertices;
  double c, next_c;

  csDVector3 p1, p2, p;
  double p1_dist, p2_dist, split_k;


  for(size_t i = 0; i < m_vertices.GetSize(); ++i)
  {
    c = plane.Classify(m_vertices[i]);

    if(fabs(c) < SMALL_EPSILON)
    {
      new_vertices.Push(m_vertices[i]);
      continue;
    }

    /* Normals point inwards here! */

    if(c > 0.0)
    {
      new_vertices.Push(m_vertices[i]);
    }

    next_c = plane.Classify(m_vertices[ (i + 1) % m_vertices.GetSize() ]);

    if((fabs(next_c) < SMALL_EPSILON) || 
        (c > 0.0 && next_c > 0.0)     || 
        (c < 0.0 && next_c < 0.0))
    {
      continue;
    }

    /* generate a split point */
    if(c > 0.0)
    {
      p1 = m_vertices[i];
      p2 = m_vertices[(i + 1) % m_vertices.GetSize()];
    }
    else
    {
      p1 = m_vertices[(i + 1) % m_vertices.GetSize()];
      p2 = m_vertices[i];
    }

    p1_dist = plane.Distance(p1);
    p2_dist = plane.Distance(p2);
    split_k = p1_dist / (p2_dist + p1_dist);

    p = p1 + (p2 - p1) * split_k;

    new_vertices.Push(p);
  }

  m_vertices = new_vertices;
}

