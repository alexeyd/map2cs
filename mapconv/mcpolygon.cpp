#include "mcpolygon.h"

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


mcPolygon::mcPolygon()
{
  m_baseplane = NULL;
}


mcPolygon::mcPolygon(const mcMapTexturedPlane *baseplane)
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


mcPolygon::mcPolygon(const mcPolygon &other)
{
  *this = other;
}


const mcPolygon& mcPolygon::operator = (const mcPolygon &other)
{
  if(&other == this)
  {
    return *this;
  }

  m_baseplane = other.m_baseplane;
  m_vertices = other.m_vertices;
  m_triangles = other.m_triangles;

  return *this;
}


mcPolygon::~mcPolygon()
{
}

/* Shinigami chop!!! */
void mcPolygon::GetChopped(const csDPlane &plane)
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


void mcPolygon::CutTriangle(size_t index, csArray<csTriangle> &new_triangles)
{
  csTriangle &tri = m_triangles.Get(index);

  csDVector3 a = m_vertices[tri[0]] + 
                 (m_vertices[tri[1]] - m_vertices[tri[0]]) * 0.5;

  csDVector3 b = m_vertices[tri[1]] + 
                 (m_vertices[tri[2]] - m_vertices[tri[1]]) * 0.5;

  csDVector3 c = m_vertices[tri[2]] + 
                 (m_vertices[tri[0]] - m_vertices[tri[2]]) * 0.5;

  size_t a_index, b_index, c_index;
  bool found_a = false, found_b = false, found_c = false;

  for(size_t i = 0; i < m_vertices.GetSize(); ++i)
  {
    if(csDVector3::Norm(a - m_vertices.Get(i)) < SMALL_EPSILON)
    {
      found_a = true;
      a_index = i;
    }

    if(csDVector3::Norm(b - m_vertices.Get(i)) < SMALL_EPSILON)
    {
      found_b = true;
      b_index = i;
    }

    if(csDVector3::Norm(c - m_vertices.Get(i)) < SMALL_EPSILON)
    {
      found_c = true;
      c_index = i;
    }

    if(found_a && found_b && found_c)
    {
      break;
    }
  }

  if(!found_a)
  {
    a_index = m_vertices.GetSize();
    m_vertices.Push(a);
  }

  if(!found_b)
  {
    b_index = m_vertices.GetSize();
    m_vertices.Push(b);
  }

  if(!found_c)
  {
    c_index = m_vertices.GetSize();
    m_vertices.Push(c);
  }

  new_triangles.Push(csTriangle(tri[0], a_index, c_index));
  new_triangles.Push(csTriangle(a_index, tri[1], b_index));
  new_triangles.Push(csTriangle(a_index, b_index, c_index));
  new_triangles.Push(csTriangle(c_index, b_index, tri[2]));
}


void mcPolygon::Triangulate(double max_edge_length)
{
  size_t vert_size = m_vertices.GetSize();

  m_triangles.Empty();

  if(vert_size < 3)
  {
    return;
  }

  /* First pass */
  for(size_t i = 2; i < vert_size; ++i)
  {
    m_triangles.Push( csTriangle(0, i-1, i) );
  }

  /* Second pass */
  for(;;)
  {
    size_t split_index_a, split_index_b;
    float len;

    bool edges_ok = true;

    for(size_t i = 0; i < m_triangles.GetSize(); ++i)
    {
      csTriangle &triangle = m_triangles.Get(i);
      
      csDVector3 edge1 = m_vertices[triangle[1]] - m_vertices[triangle[0]];
      csDVector3 edge2 = m_vertices[triangle[2]] - m_vertices[triangle[1]];
      csDVector3 edge3 = m_vertices[triangle[0]] - m_vertices[triangle[2]];

      if(edge1.Norm() > max_edge_length ||
         edge2.Norm() > max_edge_length ||
         edge3.Norm() > max_edge_length)
      {
        edges_ok = false;
      }
    }

    if(edges_ok)
    {
      break;
    }
    else
    {
      csArray<csTriangle> new_triangles;

      for(size_t i = 0; i < m_triangles.GetSize(); ++i)
      {
        CutTriangle(i, new_triangles);
      }

      m_triangles = new_triangles;
    }
  }
}


const mcMapTexturedPlane* mcPolygon::GetBaseplane() const
{
  return m_baseplane;
}


const csArray<csDVector3>& mcPolygon::GetVertices() const
{
  return m_vertices;
}


const csArray<csTriangle>& mcPolygon::GetTriangles() const
{
  return m_triangles;
}


