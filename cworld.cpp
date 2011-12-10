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
  m_sector = m_engine->CreateSector("scene");
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
      CdVector3 v1, v2, v3;
      CdVector3 t1, t2, t3;
      CdVector3 vu, vv;
      CdVector3 origin, first, second;
      double first_len, second_len, l1, l2;
      CdMatrix3 tex_mat;
      CMapTexturedPlane *plane;

      polygon = subbrush->m_polygons.Get(j);
      plane = polygon->GetBaseplane();

      origin = plane->GetTextureCoordinates(0);
      first  = plane->GetTextureCoordinates(1);
      second = plane->GetTextureCoordinates(2);

      first_len = first.Norm();
      second_len = second.Norm();
      l1 = (first - origin).Norm();
      l2 = (second - origin).Norm();

      vu = (first - origin) * (first_len / l1);
      vv = (second - origin) * (second_len / l2);

      tex_mat.m11 = vu.x; tex_mat.m12 = vv.x; tex_mat.m13 = 1.0;
      tex_mat.m21 = vu.y; tex_mat.m22 = vv.y; tex_mat.m23 = 1.0;
      tex_mat.m31 = vu.z; tex_mat.m32 = vv.z; tex_mat.m33 = 1.0;

      v1 = polygon->GetVertex(0);
      v2 = polygon->GetVertex(1);
      v3 = polygon->GetVertex(2);

      t1 = tex_mat * (v1 - origin);
      t2 = tex_mat * (v2 - origin);
      t3 = tex_mat * (v3 - origin);

      factstate->GetVertices()[pc*3].Set (v1.x, v1.y, v1.z);
      factstate->GetTexels()  [pc*3].Set (t1.x, t1.y);
     
      factstate->GetVertices()[pc*3 + 1].Set (v2.x, v2.y, v2.z);
      factstate->GetTexels()  [pc*3 + 1].Set (t2.x, t2.y);
     
      factstate->GetVertices()[pc*3 + 2].Set (v3.x, v3.y, v3.z);
      factstate->GetTexels()  [pc*3 + 2].Set (t3.x, t3.y);

      ++pc;
    }
  }

  pc = 0; // polygon count

  for(i = 0; i < subbrushes.GetSize(); ++i)
  {
    subbrush = &( subbrushes.Get(i) );

    size_t range_start = pc * 3;
    size_t range_end = (pc + subbrush->m_polygons.GetSize())* 3 - 1;

    csRef<iRenderBuffer> indices_buffer = 
      csRenderBuffer::CreateIndexRenderBuffer(subbrush->m_polygons.GetSize()*3,
                                              CS_BUF_DYNAMIC, CS_BUFCOMP_INT,
                                              range_start, range_end, true);
    csArray<int> indices;

    for(j = 0; j < subbrush->m_polygons.GetSize(); ++j)
    {
      indices.Push((pc + j) * 3);
      indices.Push((pc + j) * 3 + 1);
      indices.Push((pc + j) * 3 + 2);

    }

    pc += j;

    indices_buffer->CopyInto( &(indices.Get(0)), indices.GetSize() );

    iMaterialWrapper *material;

    material = m_engine->FindMaterial(subbrush->m_texture_name);

    if(!material)
    {
      material = m_engine->CreateMaterial(subbrush->m_texture_name, NULL);
    }

    factstate->AddSubMesh(indices_buffer, material, "");
  }

  factstate->CalculateNormals();

  csRef<iMeshWrapper> mesh =
    m_engine->CreateMeshWrapper (fact, mesh_name.GetData());

  mesh->GetMovable()->SetPosition(m_sector, csVector3(0.0, 0.0, 0.0));
}



void CCSWorld::AddLight(CMapEntity *entity)
{
  csVector3 position;
  double x, y, z;
  double radius;


  radius = entity->GetNumValueOfKey("light");

  if(entity->GetTripleNumValueOfKey("origin", x, y, z))
  {
    position.Set(x, y, z);
  }
  else
  {
    position.Set(0.0, 0.0, 0.0);
  }

  csRef<iLight> light = 
    m_engine->CreateLight(NULL, position, radius, csColor(1.0, 1.0, 1.0));

  light->GetMovable()->SetSector(m_sector);

  m_sector->GetLights()->Add(light);
}


void CCSWorld::Create(CMapFile *map)
{
  CMapEntity *entity;
  csString mesh_name;
  size_t i, entities_num;
  size_t j, brushes_num;
  csString classname;

  entities_num = map->GetNumEntities();

  for(i = 0; i < entities_num; ++i)
  {
    entity = map->GetEntity(i);
    classname = entity->GetClassname();

    if(classname == "worldspawn")
    {
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
    else if(classname == "light")
    {
      AddLight(entity);
    }
    else
    {
      csPrintf("Unknown classname: %s\n", classname.GetData());
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

