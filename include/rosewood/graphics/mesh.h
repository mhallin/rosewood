#ifndef __ROSEWOOD_GRAPHICS_MESH_H__
#define __ROSEWOOD_GRAPHICS_MESH_H__

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include "rosewood/graphics/shader.h"

namespace rosewood { namespace math {
    class Matrix4;
    class Vector4;
    class Vector3;
    class Vector2;
} }

namespace rosewood { namespace core {
    class Asset;
    class AssetView;
} }

namespace rosewood { namespace graphics {

    class Mesh {
    public:
        typedef std::vector<math::Vector3> vertex_list;
        typedef std::vector<math::Vector3> normal_list;
        typedef std::vector<math::Vector2> texcoord_list;
        typedef std::vector<math::Vector4> color_list;

        typedef std::string data_map_key;

        typedef std::unordered_map<data_map_key, normal_list> normal_list_map;
        typedef std::unordered_map<data_map_key, texcoord_list> texcoord_list_map;

        static std::shared_ptr<Mesh> create(const std::shared_ptr<core::Asset> &mesh_asset);
        static std::shared_ptr<Mesh> create(const std::string &resource_path);

        Mesh(const std::shared_ptr<core::Asset> &mesh_asset);
        Mesh();

        void instantiate(math::Matrix4 transform,
                         math::Matrix4 inverse_transform,
                         std::vector<float>::iterator destination,
                         const std::vector<Shader::AttributeSpec> &attribute_specs) const;

        const vertex_list &vertex_data() const;
        const normal_list &normal_data() const;
        const texcoord_list &texcoord_data() const;

        const normal_list &normal_data(const data_map_key &key) const;
        const texcoord_list &texcoord_data(const data_map_key &key) const;
        
        void set_vertex_data(const vertex_list &vertex_data);
        void set_normal_data(const normal_list &normal_data);
        void set_texcoord_data(const texcoord_list &texcoord_data);

        void set_normal_data(const data_map_key &key, const normal_list &normal_data);
        void set_texcoord_data(const data_map_key &key, const texcoord_list &texcoord_data);

        void set_extra_data(const data_map_key &key, const std::vector<math::Vector4> &vec4_data);

        const data_map_key &default_normal_data_key() const;
        const data_map_key &default_texcoord_data_key() const;

        void set_default_normal_data_key(const data_map_key &key);
        void set_default_texcoord_data_key(const data_map_key &key);
        
        std::shared_ptr<Mesh> copy() const;

        float bounding_sphere_radius2() const;
        
    private:
        vertex_list _vertex_data;
        normal_list_map _normal_datas;
        texcoord_list_map _texcoord_datas;

        data_map_key _default_normal_data_key;
        data_map_key _default_texcoord_data_key;

        float _bounding_sphere_radius2;

        union AttributeData;
        std::unordered_map<std::string, std::vector<AttributeData>> _extra_data;

        std::shared_ptr<core::AssetView> _mesh_asset;
        
        void reload_mesh_asset();
        void recompute_bounds();
    };
    
    inline const Mesh::vertex_list &Mesh::vertex_data() const { return _vertex_data; }
    inline const Mesh::normal_list &Mesh::normal_data() const { return _normal_datas.at(_default_normal_data_key); }
    inline const Mesh::texcoord_list &Mesh::texcoord_data() const { return _texcoord_datas.at(_default_texcoord_data_key); }

    inline const Mesh::normal_list &Mesh::normal_data(const data_map_key &key) const { return _normal_datas.at(key); }
    inline const Mesh::texcoord_list &Mesh::texcoord_data(const data_map_key &key) const { return _texcoord_datas.at(key); }
    
    inline void Mesh::set_vertex_data(const Mesh::vertex_list &vertex_data) {
        _vertex_data = vertex_data;
        _mesh_asset = nullptr;
        recompute_bounds();
    }
    
    inline void Mesh::set_normal_data(const Mesh::normal_list &normal_data) {
        _normal_datas[_default_normal_data_key] = normal_data;
        _mesh_asset = nullptr;
    }
    
    inline void Mesh::set_texcoord_data(const Mesh::texcoord_list &texcoord_data) {
        _texcoord_datas[_default_texcoord_data_key] = texcoord_data;
        _mesh_asset = nullptr;
    }

    inline void Mesh::set_normal_data(const data_map_key &key, const normal_list &normal_data) {
        _normal_datas[key] = normal_data;
        _mesh_asset = nullptr;
    }

    inline void Mesh::set_texcoord_data(const data_map_key &key, const texcoord_list &texcoord_data) {
        _texcoord_datas[key] = texcoord_data;
        _mesh_asset = nullptr;
    }

    inline const Mesh::data_map_key &Mesh::default_normal_data_key() const {
        return _default_normal_data_key;
    }

    inline const Mesh::data_map_key &Mesh::default_texcoord_data_key() const {
        return _default_texcoord_data_key;
    }

    inline void Mesh::set_default_normal_data_key(const Mesh::data_map_key &key) {
        _default_normal_data_key = key;
    }

    inline void Mesh::set_default_texcoord_data_key(const Mesh::data_map_key &key) {
        _default_texcoord_data_key = key;
    }

    inline float Mesh::bounding_sphere_radius2() const { return _bounding_sphere_radius2; }
    
} }

#endif
