#ifndef NULLTEX_PROXY_H
#define NULLTEX_PROXY_H

#include <crystalspace.h>

/* That's a hack to be able to save texture filename while
 * using NULL3D render plugin */

class csNulltexProxy 
: public scfImplementation1<csNulltexProxy, iTextureHandle>
{
  public:
    csNulltexProxy(csRef<iTextureHandle> real_handle,
                   const csString& image_name);
    virtual ~csNulltexProxy();


  virtual int GetFlags () const;
  virtual void SetKeyColor (bool Enable);
  virtual void SetKeyColor (uint8 red, uint8 green, uint8 blue);
  virtual bool GetKeyColor () const;
  virtual void GetKeyColor (uint8 &red, uint8 &green, uint8 &blue) const;
  virtual bool GetRendererDimensions (int &mw, int &mh);
  virtual void GetOriginalDimensions (int& mw, int& mh);
  virtual bool GetRendererDimensions (int &mw, int &mh, int &md);
  virtual void GetOriginalDimensions (int& mw, int& mh, int &md);
  virtual void Blit (int x, int y, int width, int height,
                     unsigned char const* data, 
                     TextureBlitDataFormat format = RGBA8888);
  virtual const char* GetImageName () const;
  virtual csAlphaMode::AlphaType GetAlphaType () const;
  virtual void Precache ();
  virtual bool IsPrecached ();
  virtual void SetTextureClass (const char* className);
  virtual const char* GetTextureClass ();
  virtual void SetAlphaType (csAlphaMode::AlphaType alphaType);
  virtual TextureType GetTextureType() const;
  virtual uint8* QueryBlitBuffer (int x, int y, int width, int height,
                                  size_t& pitch, 
                                  TextureBlitDataFormat format = RGBA8888,
                                  uint bufFlags = 0);
  virtual void ApplyBlitBuffer (uint8* buf);
  virtual BlitBufferNature GetBufferNature (uint8* buf);
  virtual void SetMipmapLimits (int maxMip, int minMip = 0);
  virtual void GetMipmapLimits (int& maxMip, int& minMip);
  virtual csPtr<iDataBuffer> 
    Readback (const CS::StructuredTextureFormat& format, int mip = 0);

  private:
    csRef<iTextureHandle> real_handle;
    csString image_name;
};

#endif

