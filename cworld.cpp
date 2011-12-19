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
  m_vfs = csQueryRegistry<iVFS>(m_object_reg);
  m_image_io = csQueryRegistry<iImageIO>(m_object_reg);

  m_sector = m_engine->CreateSector("scene");
}

CCSWorld::~CCSWorld()
{
}

struct CMapSubBrush
{
  csString m_texture_name;
  csArray<const CMapPolygon*> m_polygons;
  csArray<int> m_indices;

  CMapSubBrush() {}

  CMapSubBrush(const csString &texture_name) 
  : m_texture_name(texture_name)
  {
  }
};

csVector2 TexCoordsFromTexDef(const CMapTexDef &texdef,
                              const csVector3 vertex,
                              const csVector3 &plane_normal,
                              const iImage *texture_image)
{
  csVector3 texcoords;

  texcoords.z = 1.0;

  if(fabs(plane_normal.z) > fabs(plane_normal.y))
  {
    if(fabs(plane_normal.z) > fabs(plane_normal.x))
    {
      // plane xy
      texcoords.x = vertex.x;
      texcoords.y = vertex.y;
    }
    else
    {
      // plane yz
      texcoords.x = vertex.y;
      texcoords.y = vertex.z;
    }
  }
  else
  {
    if(fabs(plane_normal.y) > fabs(plane_normal.x))
    {
      // plane xz
      texcoords.x = vertex.x;
      texcoords.y = vertex.z;
    }
    else
    {
      // plane yz
      texcoords.x = vertex.y;
      texcoords.y = vertex.z;
    }
  }

  float width  = static_cast<float>( texture_image->GetWidth()  );
  float height = static_cast<float>( texture_image->GetHeight() );

  CS::Math::Matrix4 shift_matrix = 
    CS::Math::Matrix4(1.0, 0.0, 0.0, texdef.m_shift.x / width,
                      0.0, 1.0, 0.0, texdef.m_shift.y / height,
                      0.0, 0.0, 1.0, 0.0,
                      0.0, 0.0, 0.0, 1.0);


  float c = cos( texdef.m_rotate );
  float s = sin( texdef.m_rotate );

  CS::Math::Matrix4 rotate_matrix = 
    CS::Math::Matrix4(c,   s,   0.0, 0.0,
                      -s,  c,   0.0, 0.0,
                      0.0, 0.0, 1.0, 0.0,
                      0.0, 0.0, 0.0, 1.0);

  CS::Math::Matrix4 scale_matrix = 
    CS::Math::Matrix4(texdef.m_scale.x, 0.0,              0.0,   0.0,
                      0.0,              texdef.m_scale.y, 0.0,   0.0,
                      0.0,              0.0,              1.0,   0.0,
                      0.0,              0.0,              0.0,   1.0);

  CS::Math::Matrix4 inverse_transform_matrix = 
    shift_matrix * rotate_matrix * scale_matrix;

  inverse_transform_matrix.Invert();

  texcoords *= inverse_transform_matrix.GetTransform();

  return csVector2(texcoords.x, texcoords.y);
}


void CCSWorld::CreateMeshFromBrush(CMapBrush *brush, csString name)
{
  csArray<CMapSubBrush> subbrushes;

  CMapSubBrush *subbrush;
  csString texture_name;

  size_t i, polygon_num;
  size_t j;

  polygon_num = brush->GetPolygons().GetSize();

  for(i = 0; i < polygon_num; ++i)
  {
    const CMapPolygon &polygon = brush->GetPolygons()[i];
    texture_name = polygon.GetPlane()->GetTextureName();

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
      subbrushes.Push(CMapSubBrush(texture_name));
      subbrush = &( subbrushes.Get(subbrushes.GetSize() - 1) );
    }

    subbrush->m_polygons.Push(&polygon);
  }

  csString factory_name = name + "_factory";
  csString mesh_name = name + "_mesh";

  csRef<iMeshFactoryWrapper> fact = 
    m_engine->CreateMeshFactory ("crystalspace.mesh.object.genmesh",
                                 factory_name.GetData());

  csRef<iGeneralFactoryState> factstate = 
    scfQueryInterface<iGeneralFactoryState> (fact->GetMeshObjectFactory ());

  int vc = 0; // vertex count

  for(i = 0; i < subbrushes.GetSize(); ++i)
  {
    csString texture_filename;
    subbrush = &( subbrushes.Get(i) );


    texture_filename.Format("/this/%s.png", subbrush->m_texture_name.GetData());

    csRef<iImage> texture_image = m_texture_map.Get(subbrush->m_texture_name,
                                                    csRef<iImage>(NULL));

    if(!texture_image.IsValid())
    {
      csPrintf("Will load texture %s\n",
               texture_filename.GetData());
      csRef<iDataBuffer> image_file = m_vfs->ReadFile(texture_filename, false);

      if(image_file.IsValid())
      {
        texture_image = m_image_io->Load(image_file, CS_IMGFMT_TRUECOLOR);
      }
      else
      {
        csPrintf("Failed to load texture file, creating dummy texture\n");
        texture_image.AttachNew(new csImageMemory(256,256));
      }
    }

    m_texture_map.Put(subbrush->m_texture_name,
                      texture_image);

    for(j = 0; j < subbrush->m_polygons.GetSize(); ++j)
    {
      csVector3 v1, v2, v3;
      csVector3 t1, t2, t3;
      csVector3 vu, vv;
      csVector3 origin, first, second;
      double first_len, second_len, l1, l2;
      csMatrix3 tex_mat;
      const CMapTexturedPlane *plane;

      const CMapPolygon *polygon = subbrush->m_polygons.Get(j);
      plane = polygon->GetPlane();

      for(size_t k = 0; k < polygon->GetVertices().GetSize(); ++k)
      {
        csVector2 texcoords = 
          TexCoordsFromTexDef(plane->GetTexDef(),
                              polygon->GetVertices()[k],
                              plane->GetPlane().GetNormal(),
                              texture_image);

        factstate->AddVertex(polygon->GetVertices()[k],
                             texcoords,
                             plane->GetPlane().GetNormal(),
                             csColor4(1.0,1.0,1.0));
      }

      if(polygon->GetVertices().GetSize() >= 3)
      {
        for(size_t k = 2; k < polygon->GetVertices().GetSize(); ++k)
        {
          subbrush->m_indices.Push(vc);
          subbrush->m_indices.Push(vc + k - 1);
          subbrush->m_indices.Push(vc + k);
        }
      }

      vc += static_cast<int>(polygon->GetVertices().GetSize());
    }
  }


  for(i = 0; i < subbrushes.GetSize(); ++i)
  {
    subbrush = &( subbrushes.Get(i) );

    size_t range_start = static_cast<size_t>(subbrush->m_indices[0]);
    size_t range_end = 
    static_cast<size_t>(subbrush->m_indices[subbrush->m_indices.GetSize()-1]);

    csRef<iRenderBuffer> indices_buffer = 
      csRenderBuffer::CreateIndexRenderBuffer(subbrush->m_indices.GetSize(),
                                              CS_BUF_DYNAMIC, CS_BUFCOMP_INT,
                                              range_start, range_end);

    indices_buffer->CopyInto( &(subbrush->m_indices.Get(0)), 
                              subbrush->m_indices.GetSize() );

    iMaterialWrapper *material;
    csString material_name = subbrush->m_texture_name;
    material_name.FindReplace("/","_");

    material = m_engine->FindMaterial(material_name);

    if(!material)
    {
      material = m_engine->CreateMaterial(material_name, NULL);
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


void CCSWorld::AddStart(CMapEntity *entity)
{
  csVector3 position;
  double x, y, z;

  if(entity->GetTripleNumValueOfKey("origin", x, y, z))
  {
    position.Set(x, y, z);
  }
  else
  {
    position.Set(0.0, 0.0, 0.0);
  }

  iCameraPosition *camera_position = 
    m_engine->GetCameraPositions()->NewCameraPosition(NULL);

  camera_position->Set("scene", position, 
                       csVector3(0.0, 0.0, 1.0),
                       csVector3(0.0, 1.0, 0.0));
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
    else if(classname == "info_player_start")
    {
      AddStart(entity);
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

