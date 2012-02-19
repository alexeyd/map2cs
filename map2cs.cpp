#include <crystalspace.h>

#include "imapconv.h"

const double MAX_WORLD_COORD = 65536.0;
const double MIN_WORLD_COORD = -65536.0;

class Map2CSApp : public csApplicationFramework
{
  private:
    csRef<iEngine> m_engine;
    csRef<iVFS> m_vfs;
    csRef<iCommandLineParser> m_args_parser;


    void AddLight(const iMapEntity *entity, bool rotate, float scale);
    void AddStart(const iMapEntity *entity, bool rotate, float scale);

  public:
    Map2CSApp();

    virtual void OnCommandLineHelp();
    virtual bool OnInitialize(int argc, char* argv[]);
    virtual bool Application();
};


Map2CSApp::Map2CSApp()
{
}


void Map2CSApp::OnCommandLineHelp()
{
  csPrintf("Syntax: map2cs -rc=<resource_dir> \n"
           "               -map=<mapfile> -world=<world_dir> [-[no]rotate]\n"
           "               [-ambient=\"<float_r>, <float_g>, <float_b>\"]\n"
           "               [-scale=<float>] [-max_edge_len=<float>]\n");
}


bool Map2CSApp::OnInitialize(int argc, char* argv[])
{
  if (!RequestPlugins (object_reg,
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
    return false;
  }

  m_engine = csQueryRegistry<iEngine> (object_reg);
  m_vfs = csQueryRegistry<iVFS> (object_reg);
  m_args_parser = csQueryRegistry<iCommandLineParser> (object_reg);

  m_engine->SetSaveableFlag(true);

  if (!RequestPlugins (object_reg,
      CS_REQUEST_PLUGIN("crystalspace.documentsystem.tinyxml", iDocumentSystem),
      CS_REQUEST_PLUGIN("crystalspace.level.saver2", iSaver),
      CS_REQUEST_PLUGIN("crystalspace.level.mapconv", iMapConv),
      CS_REQUEST_END))
  {
    return false;
  }

  return true;
}


void Map2CSApp::AddLight(const iMapEntity *entity, bool rotate, float scale)
{
  csVector3 position;
  double x, y, z;
  double radius;

  bool is_directional = false;
  csVector3 target;

  if(sscanf(entity->GetValue("light"), "%lf", &radius) != 1)
  {
    radius = 0.0;
  }

  if(fabs(radius) < SMALL_EPSILON)
  {
    radius = MAX_WORLD_COORD;
  }

  if(sscanf(entity->GetValue("origin"), "%lf %lf %lf", &x, &y, &z) == 3)
  {
    if(rotate)
    {
      position.Set(x, z, y);
    }
    else
    {
      position.Set(x, y, z);
    }
  }
  else
  {
    position.Set(0.0, 0.0, 0.0);
  }

  csString sun_string = entity->GetValue("_sun");

  if(sun_string == "yes" || sun_string == "true" || sun_string =="1")
  {
    if(sscanf(entity->GetValue("target"), "%lf %lf %lf", &x, &y, &z) == 3)
    {
      if(rotate)
      {
        target.Set(x, z, y);
      }
      else
      {
        target.Set(x, y, z);
      }
    }
    else
    {
      target.Set(0.0, 0.0, 0.0);
    }

    is_directional = true;
  }

  position *= scale;
  target *= scale;
  radius *= scale;

  csRef<iLight> light = 
    m_engine->CreateLight(NULL, position, radius, csColor(1.0, 1.0, 1.0));

  if(is_directional)
  {
    light->SetType(CS_LIGHT_DIRECTIONAL);

    target -= position;
    target.Normalize();


    light->GetMovable()->GetTransform().LookAtZUpY(target, 
                                                   csVector3(0.0, 1.0, 0.0));
    light->GetMovable()->UpdateMove();
  }

  iSector *sector = m_engine->GetSectors()->FindByName("scene");

  light->GetMovable()->SetSector(sector);
  sector->GetLights()->Add(light);

  light->GetMovable()->UpdateMove();
}


void Map2CSApp::AddStart(const iMapEntity *entity, bool rotate, float scale)
{
  csVector3 position;
  double x, y, z;

  if(sscanf(entity->GetValue("origin"), "%lf %lf %lf", &x, &y, &z) == 3)
  {
    if(rotate)
    {
      position.Set(x, z, y);
    }
    else
    {
      position.Set(x, y, z);
    }
  }
  else
  {
    position.Set(0.0, 0.0, 0.0);
  }

  position *= scale;

  iCameraPosition *camera_position = 
    m_engine->GetCameraPositions()->NewCameraPosition(NULL);

  camera_position->Set("scene", position, 
                       csVector3(0.0, 0.0, 1.0),
                       csVector3(0.0, 1.0, 0.0));
}


bool Map2CSApp::Application()
{
  const char *rc_dir = m_args_parser->GetOption("rc");
  const char *map_file = m_args_parser->GetOption("map");
  const char *world_dir = m_args_parser->GetOption("world");
  const char *ambient_color = m_args_parser->GetOption("ambient");
  const char *scale_text = m_args_parser->GetOption("scale");
  const char *max_edge_len_text = m_args_parser->GetOption("max_edge_len");
  bool rotate = m_args_parser->GetBoolOption("rotate", true);

  float scale = 1.0;
  double max_edge_len = 8.0;


  if (!OpenApplication (GetObjectRegistry ()))
  {
    ReportError ("Error opening system!");
    return false;
  }

  if(!rc_dir)
  {
    ReportError("You should specify resource dir!\n");
    return false;
  }

  if(!map_file)
  {
    ReportError("You should specify map file!\n");
    return false;
  }

  if(!world_dir)
  {
    ReportError("You should specify world dir!\n");
    return false;
  }

  if(ambient_color)
  {
    float r, g, b;
    if(sscanf(ambient_color, "%f, %f, %f\n", &r, &g, &b) == 3)
    {
      m_engine->SetAmbientLight(csColor(r, g, b));
    }
    else
    {
      ReportError("Failed to parse ambient string (%s)!\n", ambient_color);
      return false;
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
      ReportError("Incorrect scale value (%s)!\n", scale_text);
      return false;
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
      ReportError("Incorrect max_edge_len value (%s)!\n", max_edge_len_text);
      return false;
    }
  }

  if(!m_vfs->Mount ("/world", world_dir))
  {
    ReportError("Failed to mount world dir (%s)!\n", world_dir);
    return false;
  }

  csRef<iMapConv> map_conv = csQueryRegistry<iMapConv> (object_reg);
  ReportInfo("Reading map '%s'...\n", map_file);
  if( !(map_conv->LoadMap(rc_dir, map_file)) )
  {
    ReportError("Failed to load map!\n");
    return 1;
  }
  ReportInfo("Done\n");

  ReportInfo("Compiling map '%s'...\n", map_file);
  map_conv->CompileMap(rotate, scale, max_edge_len);
  ReportInfo("Done\n");

  ReportInfo("Getting entities...\n");
  for(size_t i = 0; i < map_conv->GetMapEntitiesSize(); ++i)
  {
    const iMapEntity *entity = map_conv->GetMapEntity(i);

    csString classname = entity->GetValue("classname");

    if(classname == "light")
    {
      AddLight(entity, rotate, scale);
    }
    else if(classname == "info_player_start")
    {
      AddStart(entity, rotate, scale);
    }
  }
  ReportInfo("Done\n");


  csRef<iSaver> saver = csQueryRegistry<iSaver> (object_reg);
  csRef<iDocumentSystem> xml = csQueryRegistry<iDocumentSystem>(object_reg);
  csRef <iDocument> doc = xml->CreateDocument();
  csRef <iDocumentNode> root = doc->CreateRoot();

  saver->SaveMapFile(root);

  ReportInfo("Writing world...\n");

  /* create a stub to ensure that directory will be created */
  m_vfs->ChDir("/world/textures");
  m_vfs->WriteFile("map2cs_stub", "abc\n", 4);

  doc->Write(m_vfs, "/world/world");

  ReportInfo("Done.\n");

  return true;
}


CS_IMPLEMENT_APPLICATION


int main (int argc, char *argv[])
{
  return csApplicationRunner<Map2CSApp>::Run(argc, argv);
}

