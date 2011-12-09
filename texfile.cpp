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

#include "texfile.h"

static const char* InvisibleTextures[] =
{
 //"common_areaportal",
 "common_caulk",
 "common_clip",
 //"common_clusterportal",
 "common_cushion",
 "common_donotenter",
 "common_fullclip",
 "common_hint",
 "common_invisible",
 "common_missileclip",
 "common_nodraw",
 "common_nodrawnonsolid",
 "common_nodrop",
 "common_noimpact",
 "common_nolightmap",
 "common_origin",
 "common_portal",
 "common_qer_portal",
 "common_skip",
 "common_slick",
 //"common_teleporter",
 "common_trigger",
 "common_weapclip",
 "clip",
 "aaatrigger",
 "origin"
};

static const char* UnstoredTextures[] =
{
  "sky"
};

CTextureFile::CTextureFile()
{
}

CTextureFile::~CTextureFile()
{
}


void CTextureFile::SetTexturename(const char* name)
{
  m_Texturename = name;
}

const char* CTextureFile::GetTexturename() const
{
  return m_Texturename.GetData();
}


void CTextureFile::SetFilename(const char* name) 
{
  m_Filename = name;
}


const char* CTextureFile::GetFilename() const
{
  return m_Filename.GetData();
}

int CTextureFile::GetOriginalWidth() const
{
  return 128;
}


int CTextureFile::GetOriginalHeight() const
{
  return 128;
}

