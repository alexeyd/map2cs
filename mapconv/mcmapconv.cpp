#include "mcmapconv.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY(mcMapConv)

mcMapSubBrush::mcMapSubBrush()
{
}


mcMapSubBrush::mcMapSubBrush(const csString &texture_name) 
: m_texture_name(texture_name)
{
}


void mcMapConv::CreateMeshFromBrush(const mcBrush &brush, csString name,
                                    bool rotate, float scale)
{
  csArray<mcMapSubBrush> subbrushes;

  mcMapSubBrush *subbrush;
  csString texture_name;

  size_t i, j, polygon_num;


  polygon_num = brush.GetPolygons().GetSize();

  for(i = 0; i < polygon_num; ++i)
  {
    const mcPolygon &polygon = brush.GetPolygons()[i];
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
      subbrushes.Push(texture_name);
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

      mcNulltexProxy *nulltex_proxy  = 
        new mcNulltexProxy(texture->GetTextureHandle(),
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
      const mcMapTexturedPlane *plane;

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

        if(rotate)
        {
          float swap;

          swap = vertex.y;
          vertex.y = vertex.z;
          vertex.z = swap;

          swap = normal.y;
          normal.y = normal.z;
          normal.z = swap;
        }

        vertex *= scale;
        normal *= -1.0;

        factstate->AddVertex(vertex, texcoords, normal,
                             csColor4(1.0,1.0,1.0));
      }

      const csArray<csTriangle> &tris = polygon->GetTriangles();

      for(size_t k = 0; k < tris.GetSize(); ++k)
      {
        if(rotate)
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


mcMapConv::mcMapConv(iBase *p) : scfImplementationType(this, p)
{
  m_object_registry = 0;
  m_map_file = 0;
}


mcMapConv::~mcMapConv()
{
  delete m_map_file;
}


bool mcMapConv::Initialize(iObjectRegistry *obj_reg)
{
  m_object_registry = obj_reg;

  m_engine = csQueryRegistry<iEngine> (obj_reg);
  m_graphics_3d = csQueryRegistry<iGraphics3D>(obj_reg);

  m_vfs = csQueryRegistry<iVFS> (obj_reg);

  m_sector = m_engine->CreateSector("scene");

  m_map_file = new mcMapFile(obj_reg);
}


bool mcMapConv::LoadMap(const char *resource_dir,
                        const char *map_filename)
{
  if(!m_vfs->Mount("/rc", resource_dir))
  {
    csPrintf("Failed to mount resource dir (%s)!\n", resource_dir);
    return false;
  }

  if(!m_map_file->Read(map_filename))
  {
    csPrintf("Failed to read map!\n", map_filename);
    return false;
  }

  return true;
}


void mcMapConv::CompileMap(bool rotate, float scale, 
                           float max_edge_len,
                           iProgressMeter *meter)
{
  max_edge_len /= scale;

  m_map_file->CreatePolygons(max_edge_len);

  for(size_t i = 0; i < m_map_file->GetNumEntities(); ++i)
  {
    const mcMapEntity &entity = m_map_file->GetEntity(i);

    for(size_t j = 0; j < entity.GetNumBrushes(); ++j)
    {
      const mcBrush &brush = entity.GetBrush(j);
      csString mesh_name;

      mesh_name.Format("map_%s_%s_brush_%d",
                       entity.GetValue("classname"),
                       entity.GetName(),
                       static_cast<int>(j));

      CreateMeshFromBrush(brush, mesh_name, rotate, scale);
    }
  }
}


const iMapEntity* mcMapConv::GetMapEnitity(size_t index) const
{
  return &(m_map_file->GetEntity(index));
}


size_t mcMapConv::GetMapEntitiesSize() const
{
  return m_map_file->GetNumEntities();
}

