#include "rosewood/graphics/mesh.h"

#include <assert.h>

#include <numeric>

#include "rosewood/core/resource_manager.h"
#include "rosewood/core/clang_msgpack.h"

#include "rosewood/math/math_types.h"
#include "rosewood/math/matrix4.h"
#include "rosewood/math/vector.h"
#include "rosewood/math/matrix3.h"

#include "rosewood/graphics/shader.h"

using rosewood::core::Asset;
using rosewood::core::AssetView;

using rosewood::math::Matrix4;
using rosewood::math::Vector3;
using rosewood::math::Vector4;
using rosewood::math::length2;

using rosewood::graphics::Mesh;

std::shared_ptr<Mesh> Mesh::create(const std::shared_ptr<Asset> &mesh_asset) {
    return std::make_shared<Mesh>(mesh_asset);
}

std::shared_ptr<Mesh> Mesh::create(const std::string &resource_path) {
    return create(core::get_resource(resource_path + ".mesh-mp"));
}

Mesh::Mesh(const std::shared_ptr<Asset> &mesh_asset)
: _mesh_asset(core::create_view(mesh_asset, [&] { reload_mesh_asset(); })) {
    reload_mesh_asset();
}

Mesh::Mesh() { }

void Mesh::instantiate(Matrix4 transform, Matrix4 inverse_transform,
                       std::vector<float>::iterator destination,
                       const std::vector<Shader::AttributeSpec> &attribute_specs) const {
    auto n_matrix = transposed(mat3(inverse_transform));

	assert(this);

    auto &v_data = vertex_data();
    auto &n_data = normal_data();
    auto &tc_data = texcoord_data();

    typedef std::tuple<const Shader::AttributeSpec*, const AttributeData*> attribute_tuple;

    attribute_tuple *extra_attributes = static_cast<attribute_tuple*>(alloca(sizeof(attribute_tuple) * attribute_specs.size()));
    new(extra_attributes) attribute_tuple();

    for (int i = 0; i < attribute_specs.size(); ++i) {
        std::get<0>(extra_attributes[i]) = &attribute_specs[i];
        std::get<1>(extra_attributes[i]) = &_extra_data.at(attribute_specs[i].name);
    }

    for (size_t i = 0; i < _vertex_data.size(); ++i) {
        auto v = transform * v_data[i];
        auto n = n_matrix * n_data[i];
        auto t = tc_data[i];

        *destination++ = v.x();
        *destination++ = v.y();
        *destination++ = v.z();
        *destination++ = n.x();
        *destination++ = n.y();
        *destination++ = n.z();
        *destination++ = t.x;
        *destination++ = t.y;

        for (int i = 0; i < attribute_specs.size(); ++i) {
            auto &spec = *std::get<0>(extra_attributes[i]);
            auto &data = *std::get<1>(extra_attributes[i]);

            switch (spec.type) {
                case kTypeFloat:
                    if (spec.n_comps == 4) {
                        auto v4 = data.get<std::vector<Vector4>>()[i];
                        *destination++ = v4.x;
                        *destination++ = v4.y;
                        *destination++ = v4.z;
                        *destination++ = v4.w;
                    }
                    else if (spec.n_comps == 1) {
                        auto f = data.get<std::vector<float>>()[i];
                        *destination++ = f;
                    }
                    break;
            }
        }
    }
}

void Mesh::set_extra_data(const data_map_key &key, const std::vector<math::Vector4> &vec4_data) {
    _extra_data[key].get<std::vector<math::Vector4>>() = vec4_data;
    _mesh_asset = nullptr;
}

void Mesh::set_extra_data(const data_map_key &key, const std::vector<float> &float_data) {
    _extra_data[key].get<std::vector<float>>() = float_data;
    _mesh_asset = nullptr;
}


std::shared_ptr<Mesh> Mesh::copy() const {
    return std::make_shared<Mesh>(*this);
}

void Mesh::reload_mesh_asset() {
    auto &contents = _mesh_asset->str();
    msgpack::unpacked msg;
    msgpack::unpack(&msg, contents.c_str(), contents.size());

    auto arrays = msg.get();

    auto vertex_float_array = arrays.via.array.ptr[0].as<std::vector<float>>();
    auto normal_float_arrays = arrays.via.array.ptr[1].as<std::map<std::string, std::vector<float>>>();
    auto texcoord_float_arrays = arrays.via.array.ptr[2].as<std::map<std::string, std::vector<float>>>();

    _vertex_data.clear();
    _normal_datas.clear();
    _texcoord_datas.clear();

    _vertex_data.reserve(vertex_float_array.size()/3);

    for (size_t i = 0; i < vertex_float_array.size()/3; ++i) {
        _vertex_data.emplace_back(vertex_float_array[3*i+0],
                                  vertex_float_array[3*i+1],
                                  vertex_float_array[3*i+2]);
    }

    for (const auto &pair : normal_float_arrays) {
        const auto &normal_array = pair.second;
        normal_list normal_data;
        normal_data.reserve(normal_array.size()/3);

        for (size_t i = 0; i < normal_array.size()/3; ++i) {
            normal_data.emplace_back(normal_array[3*i+0],
                                     normal_array[3*i+1],
                                     normal_array[3*i+2]);
        }

        _normal_datas.emplace(pair.first, normal_data);
    }

    for (const auto &pair : texcoord_float_arrays) {
        const auto &texcoord_array = pair.second;
        texcoord_list texcoord_data;
        texcoord_data.reserve(texcoord_array.size()/2);

        for (size_t i = 0; i < texcoord_array.size()/2; ++i) {
            texcoord_data.emplace_back(texcoord_array[2*i+0],
                                       texcoord_array[2*i+1]);
        }

        _texcoord_datas.emplace(pair.first, texcoord_data);
    }

    recompute_bounds();
}

void Mesh::recompute_bounds() {
    _bounding_sphere_radius2 = std::accumulate(begin(_vertex_data), end(_vertex_data), 0.0f,
                                               [](float acc, Vector3 v){
                                                   return std::max(acc, length2(v));
                                               });
}
