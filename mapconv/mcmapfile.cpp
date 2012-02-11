#include "mcmapfile.h"

mcMapFile::mcMapFile(iObjectRegistry *object_reg)
{
  m_object_reg = object_reg;
}


mcMapFile::~mcMapFile()
{
}


bool mcMapFile::Read(const char* filename)
{
  mcMapParser parser(m_object_reg);
  csString buffer;

  if (!parser.Open(filename))
  {
    return false;
  }

  while (parser.GetTextToken(buffer))
  {
    if (buffer == "{")
    {
      mcMapEntity entity(0);

      if (!entity.Read(&parser))
      {
        return false;
      }

      m_entities.Push(entity);
    }
    else
    {
      parser.ReportError("Format error! Expected an entity, "
                         "that starts with \"{\" "
                         "but found \"%s\"", buffer.GetData());
      return false;
    }
  }

  csPrintf("Map contains %zu entites\n", m_entities.GetSize());

  return true;
}


void mcMapFile::CreatePolygons(double max_edge_length)
{
  for (size_t i=0; i<m_entities.GetSize(); i++)
  {
    m_entities[i].CreatePolygons(max_edge_length);
  }
}


size_t mcMapFile::GetNumEntities() const
{
  return m_entities.GetSize();
}


const mcMapEntity& mcMapFile::GetEntity(size_t index) const
{
  return m_entities[index];
}

