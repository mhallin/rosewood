#ifndef __ROSEWOOD_PHYSICS_HULL_H__
#define __ROSEWOOD_PHYSICS_HULL_H__

#include <vector>

#include "rosewood/core/memory.h"

namespace rosewood { namespace math {
    class Vector3;
} }

namespace rosewood { namespace core {
    class Asset;
    class AssetView;
} }

namespace rosewood { namespace physics {

    class Hull {
    public:
        static std::shared_ptr<Hull> create(const std::string &resource_path);

        Hull(const std::shared_ptr<core::Asset> &hull_asset);

        const std::vector<float> &hull_points() const;
        std::vector<float> scaled_hull_points(math::Vector3 scale) const;

    private:
        std::shared_ptr<core::AssetView> _hull_asset;

        std::vector<float> _hull_points;

        void reload_hull_asset();
    };

    inline const std::vector<float> &Hull::hull_points() const {
        return _hull_points;
    }

} }

#endif
