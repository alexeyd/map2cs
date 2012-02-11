#ifndef __MC_MAPCONV_H__
#define __MC_MAPCONV_H__

#include "imapconv.h"

#include "mcmapfile.h"

class mcMapConv : public scfImplementation2<mcMapConv, iMapConv, iComponent>
{
  private:
    iObjectRegistry *m_object_registry;

    csRef<iEngine> m_engine;
    csRef<iVFS> m_vfs;

    mcMapFile *m_map_file;
//    mcWorld *m_world;
    

  public:
    mcMapConv(iBase *p);
    virtual ~mcMapConv();

    virtual bool Initialize(iObjectRegistry *obj_reg);
 
    virtual bool LoadMap(const char *resource_dir,
                         const char *map_filename);
 
    virtual void CompileMap(bool rotate, float scale, 
                            float max_edge_len,
                            iProgressMeter *meter=0);
 
    virtual void FillCollection(iCollection *collection);
 
    virtual const iMapEntity* GetMapEnitity(size_t index) const;
    virtual size_t GetMapEntitiesSize() const;
};


#endif // __MC_MAPCONV_H__

