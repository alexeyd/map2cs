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

CCSWorld::CCSWorld(iObjectRegistry *object_reg, bool rotate, float scale)
{
  m_rotate = rotate;
  m_scale = scale;

  m_object_reg = object_reg;

  m_engine = csQueryRegistry<iEngine>(m_object_reg);
  m_graphics_3d = csQueryRegistry<iGraphics3D>(m_object_reg);
  m_vfs = csQueryRegistry<iVFS>(m_object_reg);

  m_sector = m_engine->CreateSector("scene");
}

CCSWorld::~CCSWorld()
{
}

struct CMapSubBrush
{
  csString m_texture_name;
  csArray<const mcPolygon*> m_polygons;
  csArray<unsigned> m_indices;

  CMapSubBrush() {}

  CMapSubBrush(const csString &texture_name) 
  : m_texture_name(texture_name)
  {
  }
};


void CCSWorld::CreateMeshFromBrush(mcBrush *brush, csString name)
{
  csArray<CMapSubBrush> subbrushes;

  CMapSubBrush *subbrush;
  csString texture_name;

  size_t i, polygon_num;
  size_t j;


  polygon_num = brush->GetPolygons().GetSize();

  for(i = 0; i < polygon_num; ++i)
  {
    const mcPolygon &polygon = brush->GetPolygons()[i];
    texture_name = polygon.GetBaseplane()->GetTextureName();

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

  factstate->DisableAutoNormals();

  unsigned vc = 0; // vertex count
  int tex_width, tex_height;


  for(i = 0; i < subbrushes.GetSize(); ++i)
  {
    subbrush = &( subbrushes.Get(i) );

    csRef<iTextureWrapper> texture = 
      m_texture_map.Get(subbrush->m_texture_name,
                        csRef<iTextureWrapper>(NULL));

    if(!texture.IsValid())
    {
      csString texture_filename;
      texture_filename.Format("%s.png",
                              subbrush->m_texture_name.GetData());

      csPrintf("Will load texture %s\n", texture_filename.GetData());

      m_vfs->ChDir("/rc");
      csRef<iDataBuffer> image_file = m_vfs->ReadFile(texture_filename, false);

      csString engine_texture_name;
      engine_texture_name.Format("%s_texture", 
                                 subbrush->m_texture_name.GetData());
      engine_texture_name.FindReplace("/","_");


      if(image_file.IsValid())
      {
        m_vfs->ChDir("/world/textures");
        m_vfs->WriteFile(texture_filename, 
                         image_file->GetData(),
                         image_file->GetSize());


        texture = m_engine->CreateTexture(engine_texture_name, 
                                          texture_filename, NULL, 
                                          CS_TEXTURE_3D);

      }
      else
      {
        csPrintf("Failed to load texture file!\n");
        texture = m_engine->CreateBlackTexture(engine_texture_name, 
                                               256, 256, NULL, CS_TEXTURE_3D);
      }

      texture->Register(m_graphics_3d->GetTextureManager());

      csString new_texture_filename;
      new_texture_filename.Format("textures/%s", texture_filename.GetData());

      csNulltexProxy *nulltex_proxy  = 
        new csNulltexProxy(texture->GetTextureHandle(),
                           new_texture_filename);

      texture->SetTextureHandle(nulltex_proxy);

      m_texture_map.Put(subbrush->m_texture_name, texture);
    }

    texture->GetTextureHandle()->GetOriginalDimensions(tex_width,
                                                       tex_height);

    for(j = 0; j < subbrush->m_polygons.GetSize(); ++j)
    {
      csVector3 v1, v2, v3;
      csVector3 t1, t2, t3;
      csVector3 vu, vv;
      csVector3 origin, first, second;
      double first_len, second_len, l1, l2;
      csMatrix3 tex_mat;
      const CMapTexturedPlane *plane;

      const mcPolygon *polygon = subbrush->m_polygons.Get(j);
      plane = polygon->GetBaseplane();

      const csArray<csDVector3> &verts = polygon->GetVertices();

      for(size_t k = 0; k < verts.GetSize(); ++k)
      {
        csVector2 texcoords;
        csVector3 vertex, normal;

        texcoords = plane->GetTexDef().TexCoords(verts[k],
                                                 tex_width, tex_height);
        vertex = verts[k];
        normal = plane->GetPlane().Normal();

        if(m_rotate)
        {
          float swap;

          swap = vertex.y;
          vertex.y = vertex.z;
          vertex.z = swap;

          swap = normal.y;
          normal.y = normal.z;
          normal.z = swap;
        }

        vertex *= m_scale;
        normal *= -1.0;

        factstate->AddVertex(vertex, texcoords, normal,
                             csColor4(1.0,1.0,1.0));
      }

      const csArray<csTriangle> &tris = polygon->GetTriangles();

      for(size_t k = 0; k < tris.GetSize(); ++k)
      {
        if(m_rotate)
        {
          subbrush->m_indices.Push(tris[k][2] + vc);
          subbrush->m_indices.Push(tris[k][1] + vc);
          subbrush->m_indices.Push(tris[k][0] + vc);
        }
        else
        {
          subbrush->m_indices.Push(tris[k][0] + vc);
          subbrush->m_indices.Push(tris[k][1] + vc);
          subbrush->m_indices.Push(tris[k][2] + vc);
        }
      }

      vc += static_cast<int>(verts.GetSize());
    }
  }


  for(i = 0; i < subbrushes.GetSize(); ++i)
  {
    subbrush = &( subbrushes.Get(i) );

    int range_start = -1, range_end = -1;

    for(size_t j = 0; j < subbrush->m_indices.GetSize(); ++j)
    {
      if(range_end < 0 || subbrush->m_indices.Get(j) > range_end)
      {
        range_end = subbrush->m_indices.Get(j);
      }

      if(range_start < 0 || subbrush->m_indices.Get(j) < range_start)
      {
        range_start = subbrush->m_indices.Get(j);
      }
    }

    csRef<iRenderBuffer> indices_buffer = 
      csRenderBuffer::CreateIndexRenderBuffer(subbrush->m_indices.GetSize(),
                                              CS_BUF_DYNAMIC, 
                                              CS_BUFCOMP_UNSIGNED_INT,
                                              range_start, range_end);

    indices_buffer->CopyInto( &(subbrush->m_indices.Get(0)), 
                              subbrush->m_indices.GetSize() );

    iMaterialWrapper *material;
    csString material_name = subbrush->m_texture_name;
    material_name.FindReplace("/","_");

    material = m_engine->FindMaterial(material_name);

    if(!material)
    {
      csRef<iTextureWrapper> texture = 
        m_texture_map.Get(subbrush->m_texture_name,
                          csRef<iTextureWrapper>(NULL));

      material = m_engine->CreateMaterial(material_name, texture);
    }

    factstate->AddSubMesh(indices_buffer, material, "");
  }

  csRef<iMeshWrapper> mesh =
    m_engine->CreateMeshWrapper (fact, mesh_name.GetData());

  mesh->GetMovable()->SetPosition(m_sector, csVector3(0.0, 0.0, 0.0));
}



void CCSWorld::AddLight(CMapEntity *entity)
{
  csVector3 position;
  double x, y, z;
  double radius;

  bool is_directional = false;
  csVector3 target;


  radius = entity->GetNumValueOfKey("light");

  if(fabs(radius) < SMALL_EPSILON)
  {
    radius = MAX_WORLD_COORD;
  }

  if(entity->GetTripleNumValueOfKey("origin", x, y, z))
  {
    if(m_rotate)
    {
      position.Set(x, z, y);
    }
    else
    {
      position.Set(x, y, z);
    }
  }
  else
  {
    position.Set(0.0, 0.0, 0.0);
  }

  if(entity->GetBoolValueOfKey("_sun", false))
  {
    if(entity->GetTripleNumValueOfKey("target", x, y, z))
    {
      if(m_rotate)
      {
        target.Set(x, z, y);
      }
      else
      {
        target.Set(x, y, z);
      }
    }
    else
    {
      target.Set(0.0, 0.0, 0.0);
    }

    is_directional = true;
  }

  position *= m_scale;
  target *= m_scale;
  radius *= m_scale;

  csRef<iLight> light = 
    m_engine->CreateLight(NULL, position, radius, csColor(1.0, 1.0, 1.0));

  if(is_directional)
  {
    light->SetType(CS_LIGHT_DIRECTIONAL);

    target -= position;
    target.Normalize();


    light->GetMovable()->GetTransform().LookAtZUpY(target, 
                                                   csVector3(0.0, 1.0, 0.0));
    light->GetMovable()->UpdateMove();
  }

  light->GetMovable()->SetSector(m_sector);
  m_sector->GetLights()->Add(light);

  light->GetMovable()->UpdateMove();
}


void CCSWorld::AddStart(CMapEntity *entity)
{
  csVector3 position;
  double x, y, z;

  if(entity->GetTripleNumValueOfKey("origin", x, y, z))
  {
    if(m_rotate)
    {
      position.Set(x, z, y);
    }
    else
    {
      position.Set(x, y, z);
    }
  }
  else
  {
    position.Set(0.0, 0.0, 0.0);
  }

  position *= m_scale;

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

