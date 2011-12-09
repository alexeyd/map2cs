#include "planeman.h"


CTexturedPlaneManager::CTexturedPlaneManager()
{
}


CTexturedPlaneManager::~CTexturedPlaneManager()
{
  DELETE_VECTOR_MEMBERS(m_Planes);
}


CMapTexturedPlane* 
CTexturedPlaneManager::AddPlane(CdVector3 v1, CdVector3 v2, CdVector3 v3,
                                const char* TextureName,
                                double x_off, double y_off,
                                double rot_angle,
                                double x_scale, double y_scale,
                                CdVector3 v_tx_right, CdVector3 v_tx_up,
                                bool QuarkModeTexture,
                                bool QuarkMirrored,
                                bool HLTexture)
{
  CTextureFile* texture = m_TextureManager.GetTexture(TextureName);
  if (!texture)
  {
    csPrintf("Fatal error: Texture '%s', "
             "not found! aborting!\n", TextureName);
    exit(1);
  }

  CMapTexturedPlane* pNewPlane = new CMapTexturedPlane(v1, v2, v3, 
                                                       texture, x_off, y_off, 
                                                       rot_angle, 
                                                       x_scale, y_scale);
  return AddPlane(pNewPlane);
}


CMapTexturedPlane* 
CTexturedPlaneManager::AddPlane(CdVector3 v1, CdVector3 v2, CdVector3 v3,
                                int r, int g, int b)
{
  CMapTexturedPlane* pNewPlane = new CMapTexturedPlane(v1, v2, v3, r, g, b);
  return AddPlane(pNewPlane);
}


CMapTexturedPlane* 
CTexturedPlaneManager::AddPlane(CMapTexturedPlane* pNewPlane)
{
  //first we look in m_Planes to check, if a similar plane is already stored.
  size_t i, NumPlanes = m_Planes.GetSize();

  for (i=0; i<NumPlanes; i++)
  {
    CMapTexturedPlane* pPlane = m_Planes[i];
    assert(pPlane);

    if (pPlane->IsEqual(pNewPlane))
    {
      //That plane is already stored. So we return a pointer to it.
      delete pNewPlane;
      return pPlane;
    }
  }

  //There is no such plane, so we will add the new plane.
  m_Planes.Push(pNewPlane);

  //Create a mirror plane for this plane.
  CMapTexturedPlane* pMirrorPlane = new CMapTexturedPlane(pNewPlane, true);

  //Add the mirror plane too.
  m_Planes.Push(pMirrorPlane);

  //Set Mirrorinfo to both planes.
  pNewPlane->SetMirror(pMirrorPlane);
  pMirrorPlane->SetMirror(pNewPlane);

  //return the unmirrored plane.
  return pNewPlane;
}


size_t CTexturedPlaneManager::GetPlaneCount() const
{
  return m_Planes.GetSize();
}


CMapTexturedPlane* 
CTexturedPlaneManager::GetPlane(size_t index)
{
  return m_Planes.Get(index);
}

