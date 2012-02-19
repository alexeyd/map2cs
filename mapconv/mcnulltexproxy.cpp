#include "mcnulltexproxy.h"


mcNulltexProxy::mcNulltexProxy(csRef<iTextureHandle> real_handle,
                               const csString& image_name)
: scfImplementationType(this)
{
  m_real_handle = real_handle;
  m_image_name = image_name;
}

mcNulltexProxy::~mcNulltexProxy()
{
}


int mcNulltexProxy::GetFlags () const
{
  return m_real_handle->GetFlags();
}


void mcNulltexProxy::SetKeyColor (bool Enable)
{
  m_real_handle->SetKeyColor(Enable);
}


void mcNulltexProxy::SetKeyColor (uint8 red, uint8 green, uint8 blue)
{
  m_real_handle->SetKeyColor(red, green, blue);
}


bool mcNulltexProxy::GetKeyColor () const
{
  return m_real_handle->GetKeyColor();
}


void mcNulltexProxy::GetKeyColor (uint8 &red, 
                                   uint8 &green, 
                                   uint8 &blue) const
{
  m_real_handle->GetKeyColor(red, green, blue);
}


bool mcNulltexProxy::GetRendererDimensions (int &mw, int &mh)
{
  return m_real_handle->GetRendererDimensions(mw, mh);
}


void mcNulltexProxy::GetOriginalDimensions (int& mw, int& mh)
{
  m_real_handle->GetOriginalDimensions(mw, mh);
}


bool mcNulltexProxy::GetRendererDimensions (int &mw, int &mh, int &md)
{
  return m_real_handle->GetRendererDimensions(mw, mh, md);
}


void mcNulltexProxy::GetOriginalDimensions (int& mw, int& mh, int &md)
{
  m_real_handle->GetOriginalDimensions(mw, mh, md);
}


void mcNulltexProxy::Blit (int x, int y, int width, int height,
                            unsigned char const* data, 
                            TextureBlitDataFormat format)
{
  m_real_handle->Blit(x, y, width, height, data, format);
}

const char* mcNulltexProxy::GetImageName () const
{
  return m_image_name.GetData();
}


csAlphaMode::AlphaType mcNulltexProxy::GetAlphaType () const
{
  return m_real_handle->GetAlphaType();
}


void mcNulltexProxy::Precache ()
{
  m_real_handle->Precache();
}


bool mcNulltexProxy::IsPrecached ()
{
  return m_real_handle->IsPrecached();
}


void mcNulltexProxy::SetTextureClass (const char* className)
{
  m_real_handle->SetTextureClass(className);
}


const char* mcNulltexProxy::GetTextureClass ()
{
  return m_real_handle->GetTextureClass();
}


void mcNulltexProxy::SetAlphaType (csAlphaMode::AlphaType alphaType)
{
  m_real_handle->SetAlphaType(alphaType);
}


iTextureHandle::TextureType mcNulltexProxy::GetTextureType() const
{
  return m_real_handle->GetTextureType();
}


uint8* mcNulltexProxy::QueryBlitBuffer (int x, int y, int width, int height,
                                         size_t& pitch, 
                                         TextureBlitDataFormat format,
                                         uint bufFlags)
{
  return m_real_handle->QueryBlitBuffer (x, y, width, height,
                                       pitch, format, bufFlags);
}


void mcNulltexProxy::ApplyBlitBuffer (uint8* buf)
{
  m_real_handle->ApplyBlitBuffer(buf);
}


iTextureHandle::BlitBufferNature mcNulltexProxy::GetBufferNature (uint8* buf)
{
  return m_real_handle->GetBufferNature(buf);
}


void mcNulltexProxy::SetMipmapLimits (int maxMip, int minMip)
{
  m_real_handle->SetMipmapLimits(maxMip, minMip);
}


void mcNulltexProxy::GetMipmapLimits (int& maxMip, int& minMip)
{
  m_real_handle->GetMipmapLimits(maxMip, minMip);
}


csPtr<iDataBuffer> 
mcNulltexProxy::Readback (const CS::StructuredTextureFormat& format, 
                           int mip)
{
  return m_real_handle->Readback(format, mip);
}


