#ifndef __PLANEMAN_H__
#define __PLANEMAN_H__

#include "crystalspace.h"

#include "contain.h"
#include "texplane.h"
#include "texman.h"

class CTexturedPlaneManager
{
  public:
    CTexturedPlaneManager();
    ~CTexturedPlaneManager();

    /**
      * This method will look for the given Plane in the Plane List.
      * If a similar Plane (Same position Texture and Texture alignment)
      * is found, it will return a pointer the the already available
      * plane otherwise it will create a new plane and return a pointer
      * to that plane.
      */
    CMapTexturedPlane* AddPlane(CdVector3 v1, CdVector3 v2, CdVector3 v3,
                                const char* TextureName,
                                double x_off, double y_off,
                                double rot_angle,
                                double x_scale, double y_scale,
                                CdVector3 v_tx_right, CdVector3 v_tx_up,
                                bool QuarkModeTexture,
                                bool QuarkMirrored,
                                bool HLTexture);

    /**
      * Adds a flatshaded plane. the given values are the color components
      */
    CMapTexturedPlane* AddPlane(CdVector3 v1, CdVector3 v2, CdVector3 v3,
                                int r, int g, int b);

    size_t GetPlaneCount() const;
 
    /// Get the specified plane
    CMapTexturedPlane* GetPlane(size_t index);

  protected:

    /**
      * Add a CMapTexturedPlane object to the Map. 
      * (If a similar plane already exists,
      * the given plane is deleted!)
      */
    CMapTexturedPlane* AddPlane(CMapTexturedPlane* pNewPlane);

    CTextureManager         m_TextureManager;
    CMapTexturedPlaneVector m_Planes;
};

#endif

