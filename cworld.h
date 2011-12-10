/*
    Map2cs: a convertor to convert the frequently used MAP format, into
    something, that can be directly understood by Crystal Space.

    Copyright (C) 1999 Thomas Hieber (thieber@gmx.net)
    modified by Petr Kocmid (pkocmid@atlas.cz)

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

#ifndef __CWORLD_H__
#define __CWORLD_H__

#include "crystalspace.h"
#include "dmath3d.h"
#include "map.h"
#include "brush.h"
#include "entity.h"
#include "mpoly.h"
#include "texplane.h"

class CCSWorld
{
  public:
    CCSWorld(iObjectRegistry *object_reg);
    ~CCSWorld();
 
    void Create(CMapFile *map);
    void Save(csRef <iDocumentNode> &root);

  private:
    void CreateMeshFromBrush(CMapBrush *brush, csString name);

    iObjectRegistry* m_object_reg;
    csRef<iEngine> m_engine;
    csRef<iSector> m_sector;
};

#endif // __CWORLD_H__

