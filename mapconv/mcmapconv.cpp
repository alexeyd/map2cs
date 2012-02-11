#include "mcmapconv.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY(mcMapConv)


mcMapConv::mcMapConv(iBase *p) : scfImplementationType(this, p)
{
  m_object_registry = 0;
  m_map_file = 0;
//  m_world = 0;
}


mcMapConv::~mcMapConv()
{
}


bool mcMapConv::Initialize(iObjectRegistry *obj_reg)
{
  m_object_registry = obj_reg;

  m_vfs = csQueryRegistry<iVFS> (obj_reg);
  m_engine = csQueryRegistry<iEngine> (obj_reg);

  m_map_file = new mcMapFile(obj_reg);
//  m_world = new mcWorld(obj_reg);
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
}


void mcMapConv::CompileMap(bool rotate, float scale, 
                           float max_edge_len,
                           iProgressMeter *meter)
{
  max_edge_len /= scale;

  m_map_file->CreatePolygons(max_edge_len);
//  m_world->Create(m_map, rotate, scale);
}


void mcMapConv::FillCollection(iCollection *collection)
{
 // m_world->Dump(collection);
}


const iMapEntity* mcMapConv::GetMapEnitity(size_t index) const
{
  return &(m_map_file->GetEntity(index));
}


size_t mcMapConv::GetMapEntitiesSize() const
{
  return m_map_file->GetNumEntities();
}

