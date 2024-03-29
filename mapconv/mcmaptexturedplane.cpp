#include "mcmaptexturedplane.h"


const mcTexDef& mcTexDef::operator= (const mcTexDef &other)
{
  if(this == &other)
  {
    return *this;
  }

  m_shift = other.m_shift;
  m_scale = other.m_scale;
  m_rotate = other.m_rotate;

  m_plane_normal = other.m_plane_normal;
  m_max_axis = other.m_max_axis;

  return *this;
}


mcTexDef::mcTexDef()
{
  m_shift = csVector2(0.0, 0.0);
  m_scale = csVector2(1.0, 1.0);
  m_rotate = 0.0;

  m_plane_normal = csVector3(0.0, 0.0, 1.0);
  m_max_axis = CS_AXIS_Z;
}


mcTexDef::mcTexDef(const mcTexDef &other)
{
  (*this) = other;
}


mcTexDef::mcTexDef(const csVector2 &shift, const csVector2 &scale, 
                   float rotate, const csVector3 &plane_normal)
{
  m_shift = shift;
  m_scale = scale;
  m_rotate = rotate;

  m_plane_normal = plane_normal;

  if(fabs(m_plane_normal.z) > fabs(m_plane_normal.y))
  {
    if(fabs(m_plane_normal.z) > fabs(m_plane_normal.x))
    {
      m_max_axis = CS_AXIS_Z;
    }
    else
    {
      m_max_axis = CS_AXIS_X;
    }
  }
  else
  {
    if(fabs(m_plane_normal.y) > fabs(m_plane_normal.x))
    {
      m_max_axis = CS_AXIS_Y;
    }
    else
    {
      m_max_axis = CS_AXIS_X;
    }
  }
}


csVector2 mcTexDef::TexCoords(const csVector3 vertex,
                              int tex_width, int tex_height) const
{
  csVector2 inv_scale;

  float width  = static_cast<float>( tex_width );
  float height = static_cast<float>( tex_height );

  float s = sinf(-m_rotate);
  float c = cosf(-m_rotate);

  inv_scale.x = 1.0 / (m_scale.x * width);
  inv_scale.y = 1.0 / (m_scale.x * -height);

  CS::Math::Matrix4 local2tex = 
    CS::Math::Matrix4(c*inv_scale.x, -s*inv_scale.x, 0.0, m_shift.x / width,
                      s*inv_scale.y, c*inv_scale.y,  0.0, m_shift.y / height,
                      0.0,           0.0,            1.0, 0.0,
                      0.0,           0.0,            0.0, 1.0);


  csVector3 texcoords;

  texcoords.z = 1.0;

  switch(m_max_axis)
  {
    case CS_AXIS_X:
      texcoords.x = vertex.y;
      texcoords.y = vertex.z;
      break;

    case CS_AXIS_Y:
      texcoords.x = vertex.x;
      texcoords.y = vertex.z;
      break;

    case CS_AXIS_Z:
      texcoords.x = vertex.x;
      texcoords.y = vertex.y;
      break;

    default:
      CS_ASSERT(0);
  }

  texcoords *= local2tex.GetTransform();

  return csVector2(texcoords.x, texcoords.y);
}



mcMapTexturedPlane::mcMapTexturedPlane(const csDVector3 &v0, 
                                     const csDVector3 &v1,
                                     const csDVector3 &v2,
                                     const csString &texture_name,
                                     double x_off, double y_off, 
                                     double rot_angle,
                                     double x_scale, double y_scale)
{
  m_texture_name = texture_name;

  csDMath3::CalcNormal(m_plane.norm, v0, v1, v2);
  m_plane.DD = -m_plane.norm * v0;

  m_plane.Normalize();

  m_texdef = mcTexDef(csVector2(x_off, y_off),
                      csVector2(x_scale, y_scale),
                      (M_PI * rot_angle) / 180.0,
                      m_plane.norm);
}


mcMapTexturedPlane::mcMapTexturedPlane(const mcMapTexturedPlane &other)
{
  (*this) = other;
}


mcMapTexturedPlane::~mcMapTexturedPlane()
{
}


const mcMapTexturedPlane& 
mcMapTexturedPlane::operator = (const mcMapTexturedPlane &other)
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


const csString& mcMapTexturedPlane::GetTextureName() const
{
  return m_texture_name;
}


const csDPlane& mcMapTexturedPlane::GetPlane() const
{
  return m_plane;
}


const mcTexDef& mcMapTexturedPlane::GetTexDef() const
{
  return m_texdef;
}

