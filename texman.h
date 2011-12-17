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

#ifndef __TEXMAN_H__
#define __TEXMAN_H__

#include "crystalspace.h"

#include "texfile.h"

class CTextureManager
{
public:
  CTextureManager();
  ~CTextureManager();

  /// Find the required Texture, and return a pointer to it.
  CTextureFile* GetTexture(const char* TextureName);

  /// returns the Number of known textures
  size_t GetTextureCount() { return m_StoredTextures.GetSize(); }

  /// returns the n-th texture (from 0 to GetTextureCount()-1)
  CTextureFile* GetTexture (size_t index) {return m_StoredTextures[index];}

protected:
  void CleanupTexturename (csString& Name);

  /**
    * Here are all textures stored, that have already been extracted.
    */
  csArray<CTextureFile*>      m_StoredTextures;
};

#endif // __TEXMAN_H__

