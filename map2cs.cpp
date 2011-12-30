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
#include <stdlib.h>

#include "cssysdef.h"
#include "crystalspace.h"

#include "map.h"
#include "cworld.h"

CS_IMPLEMENT_APPLICATION


void PrintSyntax()
{
  csPrintf("Syntax: map2cs -rc=<resource_dir> "
           "-map=<mapfile> -world=<world_dir> [-[no]rotate]\n");
}


int AppMain (iObjectRegistry* object_reg)
{
  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);

  if (!vfs)
  {
    csPrintf ("Couldn't load VFS!\n");
    return 1;
  }

  csRef<iCommandLineParser> args_parser = 
    csQueryRegistry<iCommandLineParser> (object_reg);

  const char *rc_dir = args_parser->GetOption("rc");
  const char *map_file = args_parser->GetOption("map");
  const char *world_dir = args_parser->GetOption("world");
  bool rotate = args_parser->GetBoolOption("rotate", true);

  if (!rc_dir || !map_file || !world_dir)
  {
    csPrintf("Incorrect syntax!\n");
    PrintSyntax();
    return 1;
  }

  if(!vfs->Mount("/rc", rc_dir))
  {
    csPrintf("Failed to mount resource dir (%s)!\n", rc_dir);
    return 1;
  }

  if(!vfs->Mount ("/world", world_dir))
  {
    csPrintf("Failed to mount world dir (%s)!\n", world_dir);
    return 1;
  }


  CMapFile map;
  csPrintf("Reading map '%s'...\n", map_file);

  if (!map.Read(map_file))
  {
    csPrintf("Failed to read map!\n", map_file);
    return 2;
  }

  csPrintf("Generating data for world '%s'...\n", world_dir);
  map.CreatePolygons();

  csRef<iDocumentSystem> xml(csPtr <iDocumentSystem>
    (new csTinyDocumentSystem()));
  csRef <iDocument> doc = xml->CreateDocument();
  csRef <iDocumentNode> root = doc->CreateRoot();

  CCSWorld world(object_reg, rotate);
  world.Create(&map);
  world.Save(root);

  csPrintf ("Writing world...\n");

  /* create a stub to ensure that directory will be created */
  vfs->ChDir("/world/textures");
  vfs->WriteFile("stub", "abc\n", 4);
  vfs->DeleteFile("stub");

  doc->Write(vfs, "/world/world");

  csPrintf("Done.\n");

  return 0;
}


int main (int argc, char *argv[])
{
  // this is required for the image loader
  iObjectRegistry* object_reg = csInitializer::CreateEnvironment (argc, argv);
  if (!object_reg) return false;
  if (!csInitializer::RequestPlugins (object_reg,
    CS_REQUEST_VFS,
    CS_REQUEST_NULL3D,
    CS_REQUEST_ENGINE,
    CS_REQUEST_FONTSERVER,
    CS_REQUEST_IMAGELOADER,
    CS_REQUEST_LEVELLOADER,
    CS_REQUEST_REPORTER,
    CS_REQUEST_REPORTERLISTENER,
    CS_REQUEST_PLUGIN("crystalspace.graphics3d.shadermanager", iShaderManager),
    CS_REQUEST_END))
  {
    return 1;
  }

  csWeakRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  engine->SetSaveableFlag(true);

  if (!csInitializer::RequestPlugins (object_reg,
                                      CS_REQUEST_LEVELSAVER,
                                      CS_REQUEST_END))
  {
    return 1;
  }


  int ret = AppMain (object_reg);
  
  csInitializer::DestroyApplication (object_reg);

  return ret;
}

