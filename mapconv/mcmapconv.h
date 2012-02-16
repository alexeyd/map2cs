#ifndef __MC_MAPCONV_H__
#define __MC_MAPCONV_H__

#include <crystalspace.h>

#include "imapconv.h"
#include "mcmapfile.h"
#include "mcnulltexproxy.h"


struct mcMapSubBrush
{
  csString m_texture_name;
  csArray<const mcPolygon*> m_polygons;
  csArray<unsigned> m_indices;

  mcMapSubBrush();
  mcMapSubBrush(const csString &texture_name);
};


class mcMapConv : public scfImplementation2<mcMapConv, iMapConv, iComponent>
{
  private:
    iObjectRegistry *m_object_registry;

    csRef<iEngine> m_engine;
    csRef<iGraphics3D> m_graphics_3d;

    csRef<iVFS> m_vfs;

    csRef<iSector> m_sector;
    csHash< csRef<iTextureWrapper>, csString > m_texture_map;

    mcMapFile *m_map_file;
    
    void CreateMeshFromBrush(const mcBrush &brush, csString name,
                             bool rotate, float scale);

  public:
    mcMapConv(iBase *p);
    virtual ~mcMapConv();

    virtual bool Initialize(iObjectRegistry *obj_reg);
 
    virtual bool LoadMap(const char *resource_dir,
                         const char *map_filename);
 
    virtual void CompileMap(bool rotate, float scale, 
                            float max_edge_len,
                            iProgressMeter *meter=0);
 
    virtual const iMapEntity* GetMapEnitity(size_t index) const;
    virtual size_t GetMapEntitiesSize() const;
};


#endif // __MC_MAPCONV_H__

