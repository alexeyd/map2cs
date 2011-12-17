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

#include "brush.h"

#include <csgeom.h>

#include <ctype.h>


CMapBrush::CMapBrush()
{
}


CMapBrush::~CMapBrush()
{
}


bool CMapBrush::Read(CMapParser* parser)
{
  csString buffer;
  csString texture_name;

  if (!parser->GetSafeToken(buffer)) 
  {
    return false;
  }

  for(;;)
  {
    if(buffer == "}")
    {
      break;
    }

    if (buffer != "(")
    {
      parser->ReportError("Format error. Expected either \"(\" or \"}\""
                          ", Found\"%s\"", buffer.GetData());
      return false;
    }

    //if this brush is not finished, then another plane must follow.
    csVector3 v1, v2, v3;
    double x_off     = 0;
    double y_off     = 0;
    double rot_angle = 0;
    double x_scale = 1.0;
    double y_scale = 1.0;

    //Read the three vectors, that define the position of the plane
    if (!ReadVector(parser, v1))    return false;
    if (!parser->ExpectToken(")"))  return false;

    if (!parser->ExpectToken("("))  return false;
    if (!ReadVector(parser, v2))    return false;
    if (!parser->ExpectToken(")"))  return false;

    if (!parser->ExpectToken("("))  return false;
    if (!ReadVector(parser, v3))    return false;
    if (!parser->ExpectToken(")"))  return false;

    //Get the name of the Texture
    if (!parser->GetTextToken(buffer)) return false;
    texture_name.Replace (buffer);

    if (!parser->GetFloatToken(x_off))  return false;
    if (!parser->GetFloatToken(y_off))  return false;

    if (!parser->GetFloatToken(rot_angle)) return false;
    if (!parser->GetFloatToken(x_scale))   return false;
    if (!parser->GetFloatToken(y_scale))   return false;

    if (!parser->GetSafeToken(buffer)) return false;

    if ((buffer != "(") && (buffer != "}"))
    {
      // Looks like a Quake3 Arena Map. I don't know the meaning of these
      // numbers, but there are always three of them
      for (int i=0; i<3; i++)
      {
        bool malformed = false;
        bool intseen = false;
        const char *q = buffer.GetData();

        while (*q != '\0' && isspace(*q))
        {
          q++;
        }

        while (*q != '\0' && isdigit(*q))
        {
          q++;
          intseen = true;
        }

        malformed = (*q != '\0' && !isspace(*q));

        if (malformed || !intseen)
        {
          parser->ReportError("Invalid Numeric format. "
                              "Expected int, found \"%s\"", 
                              buffer.GetData());
          return false;
        }

        if (!parser->GetSafeToken(buffer))
        {
          return false;
        }
      }
    }

    m_planes.Push(CMapTexturedPlane(v1, v2, v3, texture_name,
                                    x_off, y_off, rot_angle, 
                                    x_scale, y_scale));
  }

  return true;
}


bool CMapBrush::ReadVector(CMapParser* parser, csVector3& v)
{
  double value = 0;

  if (!parser->GetFloatToken(value)) return false;
  v.x = value;

  if (!parser->GetFloatToken(value)) return false;
  v.y = value;

  if (!parser->GetFloatToken(value)) return false;
  v.z = value;

  return true;
}


void CMapBrush::CheckPlanes(const CMapTexturedPlane &plane1,
                            const CMapTexturedPlane &plane2,
                            const CMapTexturedPlane &plane3)
{
  csVector3 v;
  bool found_plane1, found_plane2, found_plane3;
  CMapPolygon *poly;

  if(csIntersect3::ThreePlanes(plane1.GetPlane(), 
                               plane2.GetPlane(), 
                               plane3.GetPlane(), v))
  {
    found_plane1 = false;
    found_plane2 = false; 
    found_plane3 = false;

    for(size_t i = 0; i < m_polygons.GetSize(); ++i)
    {
      poly = &(m_polygons[i]);

      if(poly->GetPlane() == &plane1)
      {
        found_plane1 = true;
        poly->AddVertex(v);
      }

      if(poly->GetPlane() == &plane2)
      {
        found_plane2 = true;
        poly->AddVertex(v);
      }

      if(poly->GetPlane() == &plane3)
      {
        found_plane3 = true;
        poly->AddVertex(v);
      }

      if(found_plane1 && found_plane2 && found_plane3)
      {
        break;
      }
    }

    if(!found_plane1)
    {
      CMapPolygon new_poly(&plane1);
      new_poly.AddVertex(v);
      m_polygons.Push(new_poly);
    }
   
    if(!found_plane2)
    {
      CMapPolygon new_poly(&plane2);
      new_poly.AddVertex(v);
      m_polygons.Push(new_poly);
    }
   
    if(!found_plane3)
    {
      CMapPolygon new_poly(&plane3);
      new_poly.AddVertex(v);
      m_polygons.Push(new_poly);
    }
  }
}


void CMapBrush::CreatePolygons()
{
  size_t planes_num, i, j, k;
  
  planes_num = m_planes.GetSize();

  for(i = 0; i < planes_num; ++i)
  {
    for(j = 0; j < planes_num; ++j)
    {
      for(k = 0; k < planes_num; ++k)
      {
        if((i == j) || (i == k) || (j == k))
        {
          continue;
        }

        CheckPlanes(m_planes[i], m_planes[j], m_planes[k]);
      }
    }
  }
}


bool CMapBrush::IsInside(csVector3& v)
{
  for (size_t i=0; i < m_planes.GetSize(); ++i)
  {
    if (m_planes[i].GetPlane().Classify(v) < 0.0)
    {
      return false;
    }
  }

  return true;
}


const csArray<CMapPolygon>& CMapBrush::GetPolygons() const
{
  return m_polygons;
}

