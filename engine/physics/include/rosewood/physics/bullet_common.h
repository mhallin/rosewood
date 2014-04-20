#ifndef __ROSEWOOD_PHYSICS_BULLET_COMMON_H__
#define __ROSEWOOD_PHYSICS_BULLET_COMMON_H__

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Woverloaded-virtual"

#include "btBulletDynamicsCommon.h"

#pragma cclang diagnostic pop

#include "rosewood/math/math_types.h"
#include "rosewood/core/transform.h"

namespace rosewood { namespace math {

    Vector3 from_bt(const btVector3 &v3);
    Quaternion from_bt(const btQuaternion &q);

    btVector3 to_bt(const Vector3 &v3);
    btQuaternion to_bt(const Quaternion &q);
    btTransform to_bt(const Vector3 &origin, const Quaternion &rotation);



    inline Vector3 from_bt(const btVector3 &v3) {
        return Vector3(v3.x(), v3.y(), v3.z());
    }

    inline Quaternion from_bt(const btQuaternion &q) {
        return Quaternion(q.w(), q.x(), q.y(), q.z());
    }

    inline btVector3 to_bt(const Vector3 &v3) {
        return btVector3(v3.x(), v3.y(), v3.z());
    }

    inline btQuaternion to_bt(const Quaternion &q) {
        return btQuaternion(q._x, q._y, q._z, q._w);
    }

    inline btTransform to_bt(const Vector3 &origin, const Quaternion &rotation) {
        return btTransform(to_bt(rotation), to_bt(origin));
    }

    inline btTransform to_bt(const core::Transform *tf_comp) {
        return to_bt(tf_comp->world_position(), tf_comp->world_rotation());
    }

} }

#endif
