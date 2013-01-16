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

std::vector<float> Hull::scaled_hull_points(Vector3 scale) const {
    std::vector<float> scaled_points(_hull_points);

    int i = 0;
    for (float &f : scaled_points) {
        f *= get(scale, i++ % 3);
    }

    return scaled_points;
}

void Hull::reload_hull_asset() {
    auto &contents = _hull_asset->str();
    msgpack::unpacked msg;
    msgpack::unpack(&msg, contents.c_str(), contents.size());

    auto data = msg.get();

    _hull_points = data.as<std::vector<float>>();
}
