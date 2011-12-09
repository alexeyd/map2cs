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

#include "cworld.h"

CCSWorld::CCSWorld(iObjectRegistry *object_reg)
{
  m_object_reg = object_reg;
  m_engine = csQueryRegistry<iEngine>(m_object_reg);
}

CCSWorld::~CCSWorld()
{
}

struct MapSubBrush
{
  csString m_texture_name;
  csArray<CMapPolygon*> m_polygons;

  MapSubBrush() {}

  MapSubBrush(const csString &texture_name) 
  : m_texture_name(texture_name)
  {
  }
};


void CCSWorld::CreateMeshFromBrush(CMapBrush *brush, csString name)
{
  csArray<MapSubBrush> subbrushes;

  MapSubBrush *subbrush;
  csString texture_name;
  CMapPolygon *polygon;

  size_t i, polygon_num;
  size_t j;

  polygon_num = brush->GetPolygonCount();

  for(i = 0; i < polygon_num; ++i)
  {
    polygon = brush->GetPolygon(i);
    texture_name = polygon->GetBaseplane()->GetTextureName();

    subbrush = NULL;

    for(j = 0; j < subbrushes.GetSize(); ++j)
    {
      if(subbrushes.Get(j).m_texture_name == texture_name)
      {
        subbrush = &( subbrushes.Get(j) );
        break;
      }
    }

    if(!subbrush)
    {
      subbrushes.Push(MapSubBrush(texture_name));
      subbrush = &( subbrushes.Get(subbrushes.GetSize() - 1) );
    }

    subbrush->m_polygons.Push(polygon);
  }

  csString factory_name = name + "_factory";
  csString mesh_name = name + "_mesh";

  csRef<iMeshFactoryWrapper> fact = 
    m_engine->CreateMeshFactory ("crystalspace.mesh.object.genmesh",
                                 factory_name.GetData());

  csRef<iGeneralFactoryState> factstate = 
    scfQueryInterface<iGeneralFactoryState> (fact->GetMeshObjectFactory ());

  factstate->SetVertexCount(brush->GetPolygonCount() * 3);
  factstate->SetTriangleCount(brush->GetPolygonCount());

  int pc = 0; // polygon count

  for(i = 0; i < subbrushes.GetSize(); ++i)
  {
    subbrush = &( subbrushes.Get(i) );

    for(j = 0; j < subbrush->m_polygons.GetSize(); ++j)
    {
      polygon = subbrush->m_polygons.Get(j);
      CdVector3 v1, v2, v3;

      v1 = polygon->GetVertex(0);
      v2 = polygon->GetVertex(1);
      v3 = polygon->GetVertex(2);

      factstate->GetVertices()[pc*3].Set (v1.x, v1.y, v1.z);
      factstate->GetTexels()  [pc*3].Set (0, 0);
     
      factstate->GetVertices()[pc*3 + 1].Set (v2.x, v2.y, v2.z);
      factstate->GetTexels()  [pc*3 + 1].Set (0, 0);
     
      factstate->GetVertices()[pc*3 + 2].Set (v3.x, v3.y, v3.z);
      factstate->GetTexels()  [pc*3 + 2].Set (0, 0);

      ++pc;
    }
  }

  for(i = 0; i < polygon_num; ++i)
  {
    factstate->GetTriangles()[i].Set(i*3, i*3+1, i*3+2);
  }

  csRef<iMeshWrapper> mesh =
    m_engine->CreateMeshWrapper (fact, mesh_name.GetData());

  csRef<iGeneralMeshState> meshstate = 
    scfQueryInterface<iGeneralMeshState> (mesh->GetMeshObject());

  factories.Push(fact);
  meshes.Push(mesh);
}


void CCSWorld::Create(CMapFile *map)
{
  CMapEntity *entity;
  csString mesh_name;
  size_t i, entities_num;
  size_t j, brushes_num;

  entities_num = map->GetNumEntities();

  for(i = 0; i < entities_num; ++i)
  {
    entity = map->GetEntity(i);

    brushes_num = entity->GetNumBrushes();

    for(j = 0; j < brushes_num; ++j)
    {
      mesh_name.Format("map_%s_%s_brush_%d",
                       entity->GetClassname(),
                       entity->GetName(),
                       static_cast<int>(j));

      CreateMeshFromBrush(entity->GetBrush(j), mesh_name);
    }
  }
}


void CCSWorld::Save(csRef <iDocumentNode> &root)
{
  csRef<iSaver> saver = csQueryRegistry<iSaver> (m_object_reg);
  if(!saver)
  {
    csPrintf("Failed to load iSaver\n");
    return;
  }

  saver->SaveMapFile(root);
}

