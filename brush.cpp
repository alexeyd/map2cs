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
    csDVector3 v1, v2, v3;
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


bool CMapBrush::ReadVector(CMapParser* parser, csDVector3& v)
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


void CMapBrush::CreatePolygons()
{
  size_t i, j;
  
  for(i = 0; i < m_planes.GetSize(); ++i)
  {
    m_polygons.Push( CMapPolygon(&(m_planes[i])) );
  }

  for(i = 0; i < m_polygons.GetSize(); ++i)
  {
    for(j = 0; j < m_planes.GetSize(); ++j)
    {
      m_polygons[i].GetChopped( m_planes[j].GetPlane() );
    }
  }
}


const csArray<CMapPolygon>& CMapBrush::GetPolygons() const
{
  return m_polygons;
}

