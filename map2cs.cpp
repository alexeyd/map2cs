#include "crystalspace.h"
#include "imapconv.h"

CS_IMPLEMENT_APPLICATION

void PrintSyntax()
{
  csPrintf("Syntax: map2cs -rc=<resource_dir> "
           "-map=<mapfile> -world=<world_dir> "
           "[-[no]rotate] [-ambient=\"<float_r>, <float_g>, <float_b>\"] "
           "[-scale=<float>] [-max_edge_len=<float>]\n\n");
}


int AppMain (iObjectRegistry* object_reg)
{
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
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
  const char *ambient_color = args_parser->GetOption("ambient");
  const char *scale_text = args_parser->GetOption("scale");
  const char *max_edge_len_text = args_parser->GetOption("max_edge_len");
  bool rotate = args_parser->GetBoolOption("rotate", true);

  float scale = 1.0;
  double max_edge_len = 8.0;

  if (!rc_dir || !map_file || !world_dir)
  {
    csPrintf("Incorrect syntax!\n");
    PrintSyntax();
    return 1;
  }

  if(ambient_color)
  {
    float r, g, b;
    if(sscanf(ambient_color, "%f, %f, %f\n", &r, &g, &b) == 3)
    {
      engine->SetAmbientLight(csColor(r, g, b));
    }
    else
    {
      csPrintf("Failed to parse ambient string!\n");
    }
  }

  if(scale_text)
  {
    float t;

    if(sscanf(scale_text, "%f", &t) == 1)
    {
      scale = t;
    }
    else
    {
      csPrintf("Incorrect scale value (%s)!\n", scale_text);
    }
  }

  if(max_edge_len_text)
  {
    float t;

    if(sscanf(max_edge_len_text, "%f", &t) == 1)
    {
      max_edge_len = t;
    }
    else
    {
      csPrintf("Incorrect max_edge_len value (%s)!\n", max_edge_len_text);
    }
  }

  if(!vfs->Mount ("/world", world_dir))
  {
    csPrintf("Failed to mount world dir (%s)!\n", world_dir);
    return 1;
  }

  csRef<iMapConv> map_conv = csQueryRegistry<iMapConv> (object_reg);
  csPrintf("Reading map '%s'...\n", map_file);
  if( !(map_conv->LoadMap(rc_dir, map_file)) )
  {
    csPrintf("Failed to load map\n");
    return 1;
  }
  csPrintf("Done\n");

  csPrintf("Compiling map '%s'...\n", map_file);
  map_conv->CompileMap(rotate, scale, max_edge_len);
  csPrintf("Done\n");

/*
  csRef<iDocumentSystem> xml(csPtr <iDocumentSystem>
    (new csTinyDocumentSystem()));
  csRef <iDocument> doc = xml->CreateDocument();
  csRef <iDocumentNode> root = doc->CreateRoot();

  CCSWorld world(object_reg, rotate, scale);
  world.Create(&map);
  world.Save(root);

  csPrintf ("Writing world...\n"); */

  /* create a stub to ensure that directory will be created */
/*  vfs->ChDir("/world/textures");
  vfs->WriteFile("stub", "abc\n", 4);
  vfs->DeleteFile("stub");

  doc->Write(vfs, "/world/world");

  csPrintf("Done.\n"); */

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
                   CS_REQUEST_PLUGIN("crystalspace.level.saver2", iSaver),
                   CS_REQUEST_PLUGIN("crystalspace.level.mapconv", iMapConv),
                                      CS_REQUEST_END))
  {
    return 1;
  }


  int ret = AppMain (object_reg);
  
  csInitializer::DestroyApplication (object_reg);

  return ret;
}

