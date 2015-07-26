#pragma once
#include <vector>
#include "renderer/Ray.h"
#include "renderer/HitInformation.h"
#include "renderer/SceneObject.h"
#include "renderer/Model.h"
#include "tools/Constant.h"
#include "renderer/IBL.h"
#include "renderer/LightBase.h"

namespace OmochiRenderer {

class SceneObject;
class BVH;
class QBVH;
class IBL;

class Scene {
public:
  struct IntersectionInformation {
    HitInformation hit;
    SceneObject *object;
    Color texturedHitpointColor;
  };

public:
  virtual ~Scene();

  void ConstructBVH();
  void ConstructQBVH();

  // �V�[�����̃I�u�W�F�N�g�ɑ΂��Č���������s��
  bool CheckIntersection(const Ray &ray, IntersectionInformation &info) const;

  // ���C�g���X�g�擾
  const std::vector<LightBase *> GetLights() const { return m_lights; }

  // �w�i�擾
  const IBL *GetIBL() const { return m_ibl.get(); }
  virtual Color Background() const { return Color(0,0,0);  }

  virtual bool IsValid() const { return true; }

protected:
  Scene() : m_objects(), m_models(), m_inBVHObjects(), m_notInBVHObjects(), m_lights(), m_bvh(NULL), m_qbvh(NULL), m_ibl(NULL) {}

  // �V�[���փI�u�W�F�N�g�ǉ�
  void AddObject(SceneObject *obj, bool doDelete = true, bool containedInBVH = true) {
    m_objects.push_back(SceneObjectInfo(obj, doDelete, containedInBVH));
    if (containedInBVH) {
      m_inBVHObjects.push_back(obj);
    } else {
      m_notInBVHObjects.push_back(obj);
    }

    if (dynamic_cast<LightBase *>(obj) != NULL) {
      m_lights.push_back(dynamic_cast<LightBase *>(obj));
    }
  }

  // �w���p�[
  // XZ���ʏ�ɁAyUP�ŏ���ǉ�
  std::vector<SceneObject *> AddFloorXZ_yUp(const double size_x, const double size_z, const Vector3 &position, const Material &material);
  // XY���ʏ�ɁAzUP�ŏ���ǉ�
  std::vector<SceneObject *> AddFloorXY_zUp(const double size_x, const double size_y, const Vector3 &position, const Material &material);
  // YZ���ʏ�ɁAxUP�ŏ���ǉ�
  std::vector<SceneObject *> AddFloorYZ_xUp(const double size_y, const double size_z, const Vector3 &position, const Material &material);
  // XZ���ʏ�ɁAyDown�ŏ���ǉ�
  std::vector<SceneObject *> AddFloorXZ_yDown(const double size_x, const double size_z, const Vector3 &position, const Material &material);
  // XY���ʏ�ɁAzDown�ŏ���ǉ�
  std::vector<SceneObject *> AddFloorXY_zDown(const double size_x, const double size_y, const Vector3 &position, const Material &material);
  // YZ���ʏ�ɁAxDown�ŏ���ǉ�
  std::vector<SceneObject *> AddFloorYZ_xDown(const double size_y, const double size_z, const Vector3 &position, const Material &material);

  // �ǂݍ��񂾃��f����ǉ�
  void AddModel(Model *obj, bool doDelete = true, bool containedInBVH = true) {
    m_models.push_back(ModelObjectInfo(obj, doDelete));

    for (size_t i=0; i<obj->GetMaterialCount(); i++) {
      const Material &mat = obj->GetMaterial(i);
      const Model::PolygonList &pl = obj->GetPolygonList(mat);
      for (size_t j=0; j<pl.size(); j++) {
        AddObject(pl[j], false, containedInBVH);
      }
    }
  }

  struct SceneObjectInfo {
    SceneObjectInfo(SceneObject *obj, bool doDelete_, bool inBVH_)
      : object(obj)
      , doDelete(doDelete_)
      , inBVH(inBVH_)
    {
    }
    SceneObject *object;
    bool doDelete;
    bool inBVH;
  };
  struct ModelObjectInfo {
    ModelObjectInfo(Model *model_, bool doDelete_)
      : model(model_)
      , doDelete(doDelete_)
    {
    }
    Model *model;
    bool doDelete;
  };
  std::vector<SceneObject *> m_inBVHObjects;
  std::vector<SceneObject *> m_notInBVHObjects;
  std::vector<LightBase *> m_lights;
  std::vector<SceneObjectInfo> m_objects;
  std::vector<ModelObjectInfo> m_models;

  BVH *m_bvh;
  QBVH *m_qbvh;
  std::auto_ptr<IBL> m_ibl;

private:
  Scene(const Scene &s) {}
  Scene &operator =(const Scene &s) {return *this;}

};

}
