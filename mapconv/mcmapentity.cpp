#include "mcmapentity.h"

int mcMapEntity::s_entity_count = 0;


mcMapEntity::mcMapEntity(iBase *p) : scfImplementationType(this, p)
{
  ++s_entity_count;
  m_unique_name.Format ("e%d", s_entity_count);
}


mcMapEntity::~mcMapEntity()
{
}


const char* mcMapEntity::GetName() const
{
  const csString &name = m_key_value_map.Get("cs_name", csString(""));

  if(name == "")
  {
    return m_unique_name.GetData();
  }

  return name.GetData();
}


const char* mcMapEntity::GetValue(const char *key) const
{
  const csString &value = m_key_value_map.Get("cs_name", csString(""));
  if(value == "")
  {
    return "";
  }

  return value.GetData();
}


const char* mcMapEntity::GetKey(size_t index) const
{
  csHash<csString,csString>::ConstGlobalIterator iter;

  iter = m_key_value_map.GetIterator();
  iter.Reset();

  for(size_t i = 0; i < index; ++i)
  {
    iter.Advance();
  }

  csString key;

  iter.NextNoAdvance(key);

  return key.GetData();
}


size_t mcMapEntity::GetKeysSize() const
{
  return m_key_value_map.GetSize();
}


bool mcMapEntity::Read(mcMapParser* parser)
{
  csString buffer;
  csString key;
  bool finished = false;

  while (!finished)
  {
    if (!parser->GetTextToken(buffer))
    {
      return false;
    }

    if (buffer == "{")
    {
      mcBrush brush;

      if (!brush.Read(parser)) 
      {
        return false;
      }

      m_brushes.Push(brush);
    }
    else if (buffer == "}")
    {
      finished = true;
    }
    else /* key-value pair */
    {
      key = buffer;

      if (!parser->GetTextToken(buffer))
      {
        parser->ReportError("Format error. Keys and values for entities must"
                             "always come in pairs. "
                             "Found no match for key \"%s\"", key.GetData());
        return false;
      }

      m_key_value_map.PutUnique(key, buffer);
    }
  }

  return true;
}


void mcMapEntity::CreatePolygons(double max_edge_length)
{
  for (size_t i=0; i < m_brushes.GetSize(); ++i)
  {
    m_brushes[i].CreatePolygons(max_edge_length);
  }
}


size_t mcMapEntity::GetNumBrushes() const
{
  return m_brushes.GetSize();
}


const mcBrush& mcMapEntity::GetBrush(size_t index) const
{
  return m_brushes[index];
}

