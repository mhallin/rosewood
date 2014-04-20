#include "rosewood/physics/hull.h"

#include "rosewood/core/clang_msgpack.h"

#include "rosewood/math/math_types.h"
#include "rosewood/math/vector.h"

#include "rosewood/core/resource_manager.h"

using rosewood::math::Vector3;
using rosewood::math::get;

using rosewood::core::Asset;
using rosewood::core::AssetView;

using rosewood::physics::Hull;

std::shared_ptr<Hull> Hull::create(const std::string &resource_path) {
    return std::make_shared<Hull>(core::get_resource(resource_path + ".hull-mp"));
}

Hull::Hull(const std::shared_ptr<Asset> &hull_asset)
: _hull_asset(core::create_view(hull_asset, [&] { reload_hull_asset(); })) {
    reload_hull_asset();
}

std::vector<Vector3> Hull::scaled_hull_points(Vector3 scale) const {
    std::vector<Vector3> scaled_points(_hull_points);

    for (auto &f : scaled_points) {
        f = emult(f, scale);
    }

    return scaled_points;
}

void Hull::reload_hull_asset() {
    auto &contents = _hull_asset->str();
    msgpack::unpacked msg;
    msgpack::unpack(&msg, contents.c_str(), contents.size());

    auto data = msg.get();

    auto points = data.as<std::vector<float>>();

    _hull_points.clear();
    for (int i = 0; i < points.size(); i += 3) {
        _hull_points.emplace_back(points[i+0],
                                  points[i+1],
                                  points[i+2]);
    }
}
