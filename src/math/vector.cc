#include "rosewood/math/vector.h"

#include <cmath>

#include "rosewood/math/quaternion.h"
#include "rosewood/math/trig.h"

using rosewood::math::Vector3;

void rosewood::math::construct_plane(const Vector3 n,
                                     Vector3 *out_p1, Vector3 *out_p2) {
    // Find a vector `w` that is not parallel with `n`. Also, it shouldn't
    // be "almost" parallel either, for the sake of numerical precision.
    auto w = quaternion_from_axis_angle(Vector3(1, 0, 0), deg2rad(90)) * n;
    auto d = dot(n, w);

    if (std::abs(d) >= 0.6f) {
        w = quaternion_from_axis_angle(Vector3(0, 1, 0), deg2rad(90)) * n;
    }

    w = normalized(w);

    // Construct orthonormal vectors from `n` and `w`.
    auto p1 = cross(n, w);
    auto p2 = cross(n, p1);

    if (out_p1) *out_p1 = p1;
    if (out_p2) *out_p2 = p2;
}
