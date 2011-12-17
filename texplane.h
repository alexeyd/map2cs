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

#ifndef __TEXPLANE_H__
#define __TEXPLANE_H__

#include "crystalspace.h"

class CMapTexturedPlane
{
  public:

    CMapTexturedPlane(const csVector3 &v0, 
                      const csVector3 &v1,
                      const csVector3 &v2,
                      const csString &texture_name,
                      double x_off, double y_off, 
                      double rot_angle,
                      double x_scale, double y_scale);

    CMapTexturedPlane(const CMapTexturedPlane &other);
 
    ~CMapTexturedPlane();

    const CMapTexturedPlane& operator = (const CMapTexturedPlane &other);


    const csString& GetTextureName() const;
    const csVector3*  GetTextureCoords() const;
    const csPlane3& GetPlane() const;

  protected:

    csPlane3 m_plane;
    csVector3 m_texcoords[3];
    csString m_texture_name;
};

#endif // __TEXPLANE_H__

