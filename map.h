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

#ifndef __MAP_H__
#define __MAP_H__

#include "crystalspace.h"
#include "entity.h"
#include "brush.h"


/**
  * This is the main class to manage all information found within
  * a map file.
  * That file can consist of any number of entities, which can all
  * contain any number of brushes.
  * The entities will define the function of the brushes (world
  * geometry, doors, water).
  * For simplification, we will also store all possible planes here.
  * This is an optimisation, that is being done while reading, because
  * only unique planes are being created. This will allow a direct
  * realtion to the plane statement in Crystal Space worldfiles, which
  * will define the texture orientation.
  */
class CMapFile
{
public:
  /**
    * Constructs the map
    */
  CMapFile();

  /**
    * Destructor, will do cleanup. It will take care of removing all
    * entities and planes.
    */
  ~CMapFile();

  /**
    * Reads the given mapfile. It will do detailed error handling and
    * error messages by itself. It will return, true, if it was able
    * to load the complete file, and false, if there was an error, and
    * loading did not successfully finish. It will _not_ clean up the
    * already loaded part.
    * This method can currently be called only once for every instance
    * of this class. Otherwise, you will merge both map files.
    */
  bool Read(const char* filename);


  /**
    * Make all entities create their polygons.
    */
  void CreatePolygons(double max_edge_length);


  /// Get the number of all contained entities
  size_t         GetNumEntities()     {return m_Entities.GetSize();}

  /// Get the specified entity
  CMapEntity* GetEntity(size_t index) {return m_Entities.Get(index);}


  /// Get the total number of brushes in this map
  size_t GetNumBrushes() {return m_NumBrushes;}

protected:
  /**
    * Look for the texture with the given "name" in all given paths.
    * returns true, if the file is found, else return false. If it
    * returns true, then the fully qualified name is copied to
    * fullname
    */
  bool FindTextureFile(const char* name, char* fullname);


protected:
  csArray<CMapEntity*>        m_Entities;

  /// For statistics: How many Brushes are in this map.
  size_t m_NumBrushes;
};

#endif // __MAP_H__

