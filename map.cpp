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
      if (!pEntity->Read(&parser))
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

