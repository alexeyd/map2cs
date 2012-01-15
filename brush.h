/*
    Map2cs a convertor to convert the frequently used MAP format, into
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

#ifndef __BRUSH_H__
#define __BRUSH_H__

#include "crystalspace.h"
#include "mparser.h"
#include "texplane.h"
#include "mpoly.h"


class mcBrush
{
  public:
    mcBrush();
    ~mcBrush();
 
    bool Read(mcMapParser* parser);
    void CreatePolygons(double max_edge_length);
 
    const csArray<mcPolygon>& GetPolygons() const;

  protected:

    void CheckPlanes(const CMapTexturedPlane &plane1,
                     const CMapTexturedPlane &plane2,
                     const CMapTexturedPlane &plane3);

    /**
      * Reads three integer values from the parser and stores them inside a
      * vector3. Will also do error handling and error messages.
      * returns false, if there was an error. Parsing of the file should then
      * stop.
      */
    bool ReadVector(mcMapParser* parser, csDVector3& v);


    csArray<CMapTexturedPlane> m_planes;
    csArray<mcPolygon> m_polygons;
};

#endif // __BRUSH_H__

