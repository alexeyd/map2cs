#ifndef __MC_MAPTEXTUREDPLANE_H__
#define __MC_MAPTEXTUREDPLANE_H__

#include <crystalspace.h>

class mcTexDef
{
  protected:
    csVector2 m_shift;
    csVector2 m_scale;
    float m_rotate;

    csVector3 m_plane_normal;
    int m_max_axis;

  public:
    const mcTexDef& operator= (const mcTexDef &other);

    mcTexDef();
    mcTexDef(const mcTexDef &other);

    mcTexDef(const csVector2 &shift, const csVector2 &scale, 
             float rotate, const csVector3 &plane_normal);

    csVector2 TexCoords(const csVector3 vertex,
                        int tex_width, int tex_height) const;
};


class mcMapTexturedPlane
{
  public:

    mcMapTexturedPlane(const csDVector3 &v0, 
                      const csDVector3 &v1,
                      const csDVector3 &v2,
                      const csString &texture_name,
                      double x_off, double y_off, 
                      double rot_angle,
                      double x_scale, double y_scale);

    mcMapTexturedPlane(const mcMapTexturedPlane &other);
 
    ~mcMapTexturedPlane();

    const mcMapTexturedPlane& operator = (const mcMapTexturedPlane &other);


    const csString& GetTextureName() const;
    const csDPlane& GetPlane() const;
    const mcTexDef& GetTexDef() const;

  protected:

    csDPlane m_plane;
    mcTexDef m_texdef;
    csString m_texture_name;
};

#endif // __MC_MAPTEXTUREDPLANE_H__

