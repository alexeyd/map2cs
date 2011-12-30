#include "nulltex_proxy.h"


csNulltexProxy::csNulltexProxy(csRef<iTextureHandle> real_handle,
                               const csString& image_name)
: scfImplementationType(this)
{
  this->real_handle = real_handle;
  this->image_name = image_name;
}

csNulltexProxy::~csNulltexProxy()
{
}


int csNulltexProxy::GetFlags () const
{
  return real_handle->GetFlags();
}


void csNulltexProxy::SetKeyColor (bool Enable)
{
  real_handle->SetKeyColor(Enable);
}


void csNulltexProxy::SetKeyColor (uint8 red, uint8 green, uint8 blue)
{
  real_handle->SetKeyColor(red, green, blue);
}


bool csNulltexProxy::GetKeyColor () const
{
  return real_handle->GetKeyColor();
}


void csNulltexProxy::GetKeyColor (uint8 &red, 
                                   uint8 &green, 
                                   uint8 &blue) const
{
  real_handle->GetKeyColor(red, green, blue);
}


bool csNulltexProxy::GetRendererDimensions (int &mw, int &mh)
{
  return real_handle->GetRendererDimensions(mw, mh);
}


void csNulltexProxy::GetOriginalDimensions (int& mw, int& mh)
{
  real_handle->GetOriginalDimensions(mw, mh);
}


bool csNulltexProxy::GetRendererDimensions (int &mw, int &mh, int &md)
{
  return real_handle->GetRendererDimensions(mw, mh, md);
}


void csNulltexProxy::GetOriginalDimensions (int& mw, int& mh, int &md)
{
  real_handle->GetOriginalDimensions(mw, mh, md);
}


void csNulltexProxy::Blit (int x, int y, int width, int height,
                            unsigned char const* data, 
                            TextureBlitDataFormat format)
{
  real_handle->Blit(x, y, width, height, data, format);
}

const char* csNulltexProxy::GetImageName () const
{
  return image_name.GetData();
}


csAlphaMode::AlphaType csNulltexProxy::GetAlphaType () const
{
  return real_handle->GetAlphaType();
}


void csNulltexProxy::Precache ()
{
  real_handle->Precache();
}


bool csNulltexProxy::IsPrecached ()
{
  return real_handle->IsPrecached();
}


void csNulltexProxy::SetTextureClass (const char* className)
{
  real_handle->SetTextureClass(className);
}


const char* csNulltexProxy::GetTextureClass ()
{
  return real_handle->GetTextureClass();
}


void csNulltexProxy::SetAlphaType (csAlphaMode::AlphaType alphaType)
{
  real_handle->SetAlphaType(alphaType);
}


iTextureHandle::TextureType csNulltexProxy::GetTextureType() const
{
  return real_handle->GetTextureType();
}


uint8* csNulltexProxy::QueryBlitBuffer (int x, int y, int width, int height,
                                         size_t& pitch, 
                                         TextureBlitDataFormat format,
                                         uint bufFlags)
{
  return real_handle->QueryBlitBuffer (x, y, width, height,
                                       pitch, format, bufFlags);
}


void csNulltexProxy::ApplyBlitBuffer (uint8* buf)
{
  real_handle->ApplyBlitBuffer(buf);
}


iTextureHandle::BlitBufferNature csNulltexProxy::GetBufferNature (uint8* buf)
{
  return real_handle->GetBufferNature(buf);
}


void csNulltexProxy::SetMipmapLimits (int maxMip, int minMip)
{
  real_handle->SetMipmapLimits(maxMip, minMip);
}


void csNulltexProxy::GetMipmapLimits (int& maxMip, int& minMip)
{
  real_handle->GetMipmapLimits(maxMip, minMip);
}


csPtr<iDataBuffer> 
csNulltexProxy::Readback (const CS::StructuredTextureFormat& format, 
                           int mip)
{
  return real_handle->Readback(format, mip);
}


