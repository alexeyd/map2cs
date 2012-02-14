#ifndef __MC_MAPENTITY_H__
#define __MC_MAPENTITY_H__

#include "imapconv.h"

#include "mcbrush.h"
#include "mparser.h"

class mcMapEntity : public scfImplementation1<mcMapEntity, iMapEntity>
{
  protected:
    static int s_entity_count;

    csString m_unique_name;
    csHash<csString,csString> m_key_value_map;
    csArray<mcBrush> m_brushes;
 
  public:
    mcMapEntity();
    mcMapEntity(const mcMapEntity& other);
    virtual ~mcMapEntity();

    virtual const char* GetName() const;

    virtual const char* GetValue(const char *key) const;
    virtual const char* GetKey(size_t index) const;
    virtual size_t GetKeysSize() const;


    bool Read(mcMapParser* parser);
    void CreatePolygons(double max_edge_length);

    size_t GetNumBrushes() const;
    const mcBrush& GetBrush(size_t index) const;
};

#endif // __MC_MAPENTITY_H__

