#ifndef __I_MAPCONV_H__
#define __I_MAPCONV_H__

#include "crystalspace.h"

struct iMapEntity : public virtual iBase
{
  SCF_INTERFACE (iMapEntity, 0, 0, 1);
 
  virtual const char* GetName() const = 0;

  virtual const char* GetValue(const char *key) const = 0;
  virtual const char* GetKey(size_t index) const = 0;
  virtual size_t GetKeysSize() const = 0;
};


struct iMapConv : public virtual iBase
{
  SCF_INTERFACE (iMapConv, 0, 0, 1);
  
  virtual bool LoadMap(const char *resource_dir, 
                       const char *map_filename) = 0;

  virtual void CompileMap(bool rotate, float scale, 
                          float max_edge_len,
                          iProgressMeter *meter=0) = 0;

  virtual const iMapEntity* GetMapEntity(size_t index) const = 0;
  virtual size_t GetMapEntitiesSize() const = 0;
};

#endif // __I_MAPCONV_H__

