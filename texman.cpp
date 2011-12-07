/*
    Map2cs: a convertor to convert the frequently used MAP format, into
    something, that can be directly understood by Crystal Space.

    Copyright (C) 1999 Thomas Hieber (thieber@gmx.net)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "cssysdef.h"
#include "mapstd.h"
#include "texman.h"
#include "map.h"
#include "texfile.h"
#include "entity.h"

#include "iutil/vfs.h"

static const char* Q3Extensions[] =
{
 "_hell",
 "_trans",
 "_1k",
 "_750",
 "flat_400"
};

CTextureManager::CTextureManager()
{
}

CTextureManager::~CTextureManager()
{
  DELETE_VECTOR_MEMBERS(m_StoredTextures);
}


CTextureFile* CTextureManager::GetTexture(const char* TextureName)
{
  csString CleanedUpTextureName (TextureName);
  CleanupTexturename (CleanedUpTextureName);

  csString InternalName (CleanedUpTextureName);

  // clean out some chars like '/' etc.
  size_t p=0;
  for (p=0; p<InternalName.Length(); p++)
  {
    if (strchr ("/\\", InternalName[p]))
    {
      InternalName[p] = '_';
    }
  }
#if 0
  //First, we search in the array of already stored textures.
  size_t i;
  for (i=0; i<m_StoredTextures.GetSize(); i++)
  {
    CTextureFile* pTexture = m_StoredTextures[i];
    assert(pTexture);
    if (strcasecmp(pTexture->GetTexturename(), InternalName)==0)
    {
      return pTexture;
    }
  }

  //If we didn't find it there, we look in all known texture archives.
  for (i=0; i<m_TextureArchives.GetSize(); i++)
  {
    CTextureArchive* pTexArchive = m_TextureArchives[i];
    assert(pTexArchive);
    CTextureFile* pTexture = pTexArchive->CreateTexture(CleanedUpTextureName);
    if (pTexture)
    {
      pTexture->SetTexturename(InternalName);
      m_StoredTextures.Push(pTexture);
      return pTexture;
    }
  }

  //That texture is not available. Now we will use the default texture
  //instead, if we are not already looking for the defaulttexture
  char defaultname[200];
  strcpy(defaultname, "");

  //Now we create a default texture for the missing texture.
  for (i=0; i<m_TextureArchives.GetSize(); i++)
  {
    CTextureArchive* pTexArchive = m_TextureArchives[i];
    assert(pTexArchive);
    CTextureFile* pTexture = pTexArchive->CreateTexture(defaultname);
    if (pTexture)
    {
      pTexture->SetTexturename(InternalName);
      if (pTexture->IsVisible() && pTexture->IsStored())
      {
	csPrintf ("Warning: texture '%s'('%s') is missing.\n"
	  "         Using '%s' instead!\n",  TextureName, 
	  InternalName.GetData(), defaultname);
      }
      m_StoredTextures.Push(pTexture);
      return pTexture;
    }
  }
#endif
  CTextureFile* pTexture = 0;

  csPrintf("Warning: texture '%s'('%s') is missing.\n        Making a new null texture.\n",
    TextureName, CleanedUpTextureName.GetData());
  pTexture = new CTextureFile;
  pTexture->SetTexturename (InternalName);
  pTexture->SetFilename    (CleanedUpTextureName);
  pTexture->SetOriginalData(0,0);
  pTexture->SetOriginalSize(256, 256);
  m_StoredTextures.Push(pTexture);
  return pTexture;
}

bool CTextureManager::AddAllTexturesToVFS(csRef<iVFS> VFS, const char* path)
{
  bool ok = true;
  size_t i;
  for (i=0; i<m_StoredTextures.GetSize(); i++)
  {
    CTextureFile* pTexture = m_StoredTextures[i];
    assert(pTexture);

    //Only add visible textures to the ZIP. This avoids adding textures
    //that shouldn't bee seen in a map. (Like brushes that are used for
    //visblocking and that are using some reserved names.).
    if (pTexture->IsVisible() && pTexture->IsStored())
    {
      if (!pTexture->AddToVFS(VFS, path))
      {
        ok = false;
      }
    }
  }
  return ok;
}

void CTextureManager::CleanupTexturename (csString& Name)
{
  int i;

  for (i=0; i<int(sizeof(Q3Extensions)/sizeof(Q3Extensions[0])); i++)
  {
    size_t ExtensionLen = strlen(Q3Extensions[i]);
    if (Name.Length() > ExtensionLen)
    {
      if (strcmp (Name.GetData() + Name.Length() - ExtensionLen, 
        Q3Extensions[i])==0)
      {
        Name.Truncate (Name.Length() - ExtensionLen);
        return;
      }
    }
  }
}
