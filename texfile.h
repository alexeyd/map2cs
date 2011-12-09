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

#ifndef __TEXFILE_H__
#define __TEXFILE_H__

#include "crystalspace.h"
#include "csutil/ref.h"

/**
  *
  */
class CTextureFile
{
public:
  CTextureFile();
  ~CTextureFile();

  void SetTexturename(const char* name);
  const char* GetTexturename() const;

  void SetFilename(const char* name);
  const char* GetFilename() const;

  int GetOriginalWidth() const;
  int GetOriginalHeight() const;

protected:
  /**
    * The name of the texture, without filetype extension or references to
    * a path on the local HD. It _can_ contain relative paths like in Q3A,
    * where a texture name could be "gothic_base/wall_012_fragment"
    */
  csString m_Texturename;

  /**
    * The name, that will be used in the map file. It contains the filetype
    * as extension, but it will not contain paths, because we are going to use
    * a flat structure in CS map files. This name can be related to the original
    * Texture name, but it could well be a name like "tex00001.jpg"
    */
  csString m_Filename;
};

#endif // __TEXFILE_H__

