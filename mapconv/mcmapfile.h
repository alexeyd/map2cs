#ifndef __MC_MAPFILE_H__
#define __MC_MAPFILE_H__

#include "crystalspace.h"
#include "mcmapentity.h"
#include "mcbrush.h"


class mcMapFile
{
  public:
    mcMapFile(iObjectRegistry *object_reg);
    ~mcMapFile();

    bool Read(const char* filename);
    void CreatePolygons(double max_edge_length);

    size_t GetNumEntities() const;
    const mcMapEntity& GetEntity(size_t index) const;

  protected:
    csArray<mcMapEntity> m_entities;
    iObjectRegistry *m_object_reg;
};

#endif // __MC_MAPFILE_H__

