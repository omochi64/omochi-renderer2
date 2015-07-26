#pragma once

#include "LightBase.h"
#include "Sphere.h"
#include "tools/Random.h"

namespace OmochiRenderer {
  class SphereLight : public Sphere, public LightBase {
  public:
    SphereLight(double r, const Vector3 &pos, const Material &material_)
      : Sphere(r, pos, material_)
      , LightBase()
    {
    }
    virtual ~SphereLight() {}

    virtual void SampleOnePoint(Vector3 &point, Vector3 &normal, double &pdf, const Random &rnd) const {
      
      // 
      
      
      
      // �S������ uniform �ɃT���v�����O
      // pdf = 1.0 / ���ar�̋��̕\�ʐ�
      pdf = 1.0 / (4 * PI *m_radius * m_radius);
       
      // F(��, ��) = ��/2��*(1-cos��)/2
      double phi = 2 * PI*rnd.nextDouble();
      double cos_shita = 1 - 2 * rnd.nextDouble();
      double sin_shita = sqrt(1-cos_shita*cos_shita);

      Vector3 dir(sin_shita*cos(phi), cos_shita, sin_shita*sin(phi));

      point = this->position_ + dir * (m_radius + EPS);
      normal = dir;
    }

    // targetPoint ������ł���\���������ʒu�ŃT���v�����O����
    virtual bool SampleOnePointWithTargetPoint(Vector3 &sampledPoint, Vector3 &sampledPointNormal, double &pdf, const Vector3 &targetPoint, const Random &rnd) const {

      Vector3 diff = targetPoint - this->position_;

      // ���ł��肤��Ƃ͈̔� (target, ���̒��S, target��ʂ鋅�̐ڐ� �������āAcos�Ƃ̒l���v�Z����Ƃ����Ȃ�)
      double diff_size = diff.length();
      double max_cos_shita = m_radius / diff_size;

      if (max_cos_shita > 1) return false; // ���̓����ɓ��荞��ł���

      Vector3 &normal = diff;
      normal /= diff_size;

      Vector3 axis1, axis2;
      Utils::GetCrossAxes(normal, axis1, axis2);

      // max_cos_shita ���ォ�� uniform �ɃT���v�����O
      pdf = 1.0 / (2 * PI * m_radius * m_radius * ( 1 - max_cos_shita ) );

      // F(��, ��) = ��/2��*(1-cos��)/(1-max_cos)
      double phi = 2 * PI*rnd.nextDouble();
      double cos_shita = 1 - (1 - max_cos_shita) * rnd.nextDouble();
      double sin_shita = sqrt(1 - cos_shita*cos_shita);

      Vector3 dir = normal * cos_shita + axis1 * sin_shita * sin(phi) + axis2 * sin_shita * cos(phi);

      sampledPoint = this->position_ + dir * (m_radius + EPS);
      sampledPointNormal = dir;

      return true;
    }

    virtual double TotalPower() const {
      double emission_total = 0.f;
      for (auto mat : materials_)
      {
        emission_total += mat.material_.emission.length() * mat.rate_;
      }
      return 4 * PI * m_radius * m_radius * emission_total;
    }
  };
}
