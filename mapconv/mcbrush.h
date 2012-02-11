#ifndef __MC_BRUSH_H__
#define __MC_BRUSH_H__

#include "crystalspace.h"
#include "mparser.h"
#include "texplane.h"
#include "mpoly.h"


class mcBrush
{
  public:
    mcBrush();
    ~mcBrush();
 
    bool Read(mcMapParser* parser);
    void CreatePolygons(double max_edge_length);
 
    const csArray<mcPolygon>& GetPolygons() const;

  protected:

    void CheckPlanes(const CMapTexturedPlane &plane1,
                     const CMapTexturedPlane &plane2,
                     const CMapTexturedPlane &plane3);

    /**
      * Reads three integer values from the parser and stores them inside a
      * vector3. Will also do error handling and error messages.
      * returns false, if there was an error. Parsing of the file should then
      * stop.
      */
    bool ReadVector(mcMapParser* parser, csDVector3& v);


    csArray<CMapTexturedPlane> m_planes;
    csArray<mcPolygon> m_polygons;
};

#endif // __MC_BRUSH_H__

