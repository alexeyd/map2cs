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

#include "texplane.h"

CMapTexturedPlane::CMapTexturedPlane(const csVector3 &v0, 
                                     const csVector3 &v1,
                                     const csVector3 &v2,
                                     const csString &texture_name,
                                     double x_off, double y_off, 
                                     double rot_angle,
                                     double x_scale, double y_scale)
: m_plane(v0, v1, v2)
{
  m_texture_name = texture_name;

  m_texdef.m_shift.x = static_cast<float>(x_off);
  m_texdef.m_shift.y = static_cast<float>(y_off);

  m_texdef.m_scale.x = static_cast<float>(x_scale);
  m_texdef.m_scale.y = static_cast<float>(y_scale);

  // to radians
  m_texdef.m_rotate = static_cast<float>((M_PI * rot_angle) / 180.0);

  m_plane.Normalize();
}


CMapTexturedPlane::CMapTexturedPlane(const CMapTexturedPlane &other)
{
  (*this) = other;
}


CMapTexturedPlane::~CMapTexturedPlane()
{
}


const CMapTexturedPlane& 
CMapTexturedPlane::operator = (const CMapTexturedPlane &other)
{
  if(&other == this)
  {
    return *this;
  }

  m_plane = other.m_plane;
  m_texture_name = other.m_texture_name;
  m_texdef = other.m_texdef;

  return *this;
}


const csString& CMapTexturedPlane::GetTextureName() const
{
  return m_texture_name;
}


const csPlane3& CMapTexturedPlane::GetPlane() const
{
  return m_plane;
}


const CMapTexDef& CMapTexturedPlane::GetTexDef() const
{
  return m_texdef;
}

