#ifndef __ROSEWOOD_GRAPHICS_SHADER_H__
#define __ROSEWOOD_GRAPHICS_SHADER_H__

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "rosewood/graphics/gl_state.h"

namespace rosewood { namespace math {
    class Matrix4;
    class Matrix3;
} }

namespace rosewood { namespace core {
    class AssetView;
    class Asset;
} }

namespace rosewood { namespace graphics {

    class Shader {
    public:
        struct AttributeSpec {
            std::string name;
            int width;
            int n_comps;
            VertexAttribPointerType type;
            bool normalized;
        };
        
        struct UniformSpec {
            
        };

        static std::shared_ptr<Shader> create(const std::string &resource_path);
        
        Shader(std::shared_ptr<core::Asset> shader_spec_asset);
        ~Shader();
        
        void use() const;
        void set_projection_uniform(math::Matrix4 projection_matrix) const;
        void set_modelview_uniform(math::Matrix4 modelview_matrix) const;
        void set_normal_uniform(math::Matrix3 normal_matrix) const;
        void set_texture_sampler_uniform(int sampler) const;

        void initialize_attribute_arrays() const;
        int attribute_stride() const;

        int queue_index() const;

        const std::vector<AttributeSpec> &extra_attributes() const;
        
        enum class Uniforms {
            kProjectionMatrixUniform,
            kModelViewMatrixUniform,
            kNormalMatrixUniform,
            kTextureSamplerUniform,
            kNumUniforms
        };
        
        enum class Attributes {
            kPositionAttribute,
            kNormalAttribute,
            kTexCoordAttribute,
            kNumAttributes
        };
        
        Shader(const Shader&) = delete;
        Shader &operator=(const Shader&) = delete;
        
        void reload_shader();

    private:
        GLuint _program;
        std::unordered_map<std::string, GLint> _uniforms;

        std::shared_ptr<core::AssetView> _shader_spec;

        std::vector<AttributeSpec> _extra_attributes;
        std::vector<UniformSpec> _extra_uniforms;

        int _queue_index;
        
        void destroy_shader();

        static const int kDefaultQueueIndex;
        
        static const char *kUniformNames[(int)Uniforms::kNumUniforms];
        static const char *kAttributeNames[(int)Attributes::kNumAttributes];
        
        static bool compile_shader(GLuint *out_shader,
                                   GLenum shader_type,
                                   std::string shader_source);
        static bool link_program(GLuint program);
    };

    inline int Shader::queue_index() const { return _queue_index; }
    
} }

#endif
