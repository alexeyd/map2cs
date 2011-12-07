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

#include "cssysdef.h"
#include "mapstd.h"
#include "map.h"
#include "entity.h"
#include "mparser.h"
#include "texplane.h"
#include "mpoly.h"

CMapFile::CMapFile()
{
  m_NumBrushes  = 0;
}


CMapFile::~CMapFile()
{
  DELETE_VECTOR_MEMBERS(m_Entities);
}


bool CMapFile::Read(const char* filename)
{
  CMapParser parser;
  csString buffer;

  m_NumBrushes  = 0;

  if (!parser.Open(filename))
  {
    return false;
  }


  while (parser.GetNextToken(buffer))
  {
    if (strcmp(buffer, "{") == 0)
    {
      //a new entity follows.
      CMapEntity* pEntity = new CMapEntity;
      if (!pEntity->Read(&parser, &m_tex_plane_manager))
      {
        return false;
      }
      m_Entities.Push(pEntity);
      m_NumBrushes += pEntity->GetNumBrushes();
    }
    else
    {
      parser.ReportError("Format error! Expected an entity, "
                         "that starts with \"{\" "
                         "but found \"%s\"", buffer.GetData());
      return false;
    }
  }

  csPrintf("Map contains:\n");
  csPrintf("%zu Entites\n", m_Entities.GetSize());
  csPrintf("%zu Brushes\n", m_NumBrushes);

  return true;
}


void CMapFile::CreatePolygons()
{
  size_t i;

  for (i=0; i<m_Entities.GetSize(); i++)
  {
    m_Entities[i]->CreatePolygons();
  }
}


void CMapFile::GetMapSize(CdVector3& Min, CdVector3& Max)
{
  bool inited = false;
  Min = CdVector3(0,0,0);
  Max = CdVector3(0,0,0);

  //iterate all entities, brushes, polygons and vertices:
  size_t i;
  for (i=0; i<GetNumEntities(); i++)
  {
    CMapEntity* pEntity = GetEntity((int)i);

    // First take care of the "origin" of entities, because otherwise
    // in very open maps, we might miss some lights.
    CdVector3 v(0,0,0);
    if (pEntity->GetOrigin(v))
    {
      if (!inited)
      {
        //we will intialize Min and Max to the value of the first
        //vertex we find. (We can't just intialise Min and Max to
        //(0,0,0), because we can'T guarantee, that nobody will
        //create a map, that does not even enclude that point
        //inside the maps objects.
        inited = true;
        Min = v;
        Max = v;
      }
      else
      {
        //Extend Min and Max if needed.
        if (v.x > Max.x) Max.x = v.x;
        if (v.y > Max.y) Max.y = v.y;
        if (v.z > Max.z) Max.z = v.z;

        if (v.x < Min.x) Min.x = v.x;
        if (v.y < Min.y) Min.y = v.y;
        if (v.z < Min.z) Min.z = v.z;
      }
    }

    size_t j, k, l;
    for (j=0; j<pEntity->GetNumBrushes(); j++)
    {
      CMapBrush* pBrush = pEntity->GetBrush(j);
      for (k=0; k<pBrush->GetPolygonCount(); k++)
      {
        CMapPolygon* pPolygon = pBrush->GetPolygon(k);
        for (l=0; l<pPolygon->GetVertexCount(); l++)
        {
          CdVector3 v = pPolygon->GetVertex(l);
          if (!inited)
          {
            //we will intialize Min and Max to the value of the first
            //vertex we find. (We can't just intialise Min and Max to
            //(0,0,0), because we can'T guarantee, that nobody will
            //create a map, that does not even enclude that point
            //inside the maps objects.
            inited = true;
            Min = v;
            Max = v;
          }
          else
          {
            //Extend Min and Max if needed.
            if (v.x > Max.x) Max.x = v.x;
            if (v.y > Max.y) Max.y = v.y;
            if (v.z > Max.z) Max.z = v.z;

            if (v.x < Min.x) Min.x = v.x;
            if (v.y < Min.y) Min.y = v.y;
            if (v.z < Min.z) Min.z = v.z;
          }
        } //for vertex
      } //for poly
    } //for brush
  } //for entity
}

