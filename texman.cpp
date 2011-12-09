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

  CTextureFile* pTexture = 0;

  pTexture = new CTextureFile();
  pTexture->SetTexturename (InternalName);
  pTexture->SetFilename    (CleanedUpTextureName);
  m_StoredTextures.Push(pTexture);

  return pTexture;
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

