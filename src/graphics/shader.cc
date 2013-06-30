#include "rosewood/graphics/shader.h"

#include <iostream>
#include <numeric>

#include "rosewood/core/resource_manager.h"
#include "rosewood/core/clang_msgpack.h"
#include "rosewood/core/logging.h"

#include "rosewood/math/math_types.h"
#include "rosewood/math/matrix4.h"
#include "rosewood/math/vector.h"

#include "rosewood/graphics/gl_state.h"
#include "rosewood/graphics/gl_func.h"

using rosewood::core::Asset;

using rosewood::math::Matrix4;
using rosewood::math::Matrix3;
using rosewood::math::Vector4;

using rosewood::graphics::Shader;

const int Shader::kDefaultQueueIndex = 100;

const char *Shader::kAttributeNames[(int)Shader::Attributes::kNumAttributes] = {
    "rw_vertex", "rw_normal", "rw_texcoord",
};

const char *Shader::kUniformNames[(int)Shader::Uniforms::kNumUniforms] = {
    "rw_projection_matrix", "rw_modelview_matrix", "rw_normal_matrix",
    "rw_texture",
    "rw_light_position", "rw_light_color"
};

static GLenum convert_blend_name(const std::string &name) {
    static std::unordered_map<std::string, GLenum> blend_modes{
        { "zero", GL_ZERO }, { "one", GL_ONE },
        { "src-color", GL_SRC_COLOR },
        { "src-alpha", GL_SRC_ALPHA },
        { "dst-color", GL_DST_COLOR },
        { "dst-alpha", GL_DST_ALPHA },
        { "one-minus-src-color", GL_ONE_MINUS_SRC_COLOR },
        { "one-minus-src-alpha", GL_ONE_MINUS_SRC_ALPHA },
        { "one-minus-dst-color", GL_ONE_MINUS_DST_COLOR },
        { "one-minus-dst-alpha", GL_ONE_MINUS_DST_ALPHA },
    };
    return blend_modes.at(name);
}

std::shared_ptr<Shader> Shader::create(const std::string &resource_path) {
    return std::make_shared<Shader>(core::get_resource(resource_path + ".rwshader-mp"));
}

Shader::Shader(std::shared_ptr<Asset> shader_spec_asset)
: _program(UINT_MAX)
, _shader_spec(core::create_view(shader_spec_asset, [&] { reload_shader(); }))
, _queue_index(kDefaultQueueIndex)
, _depth_test(true), _depth_write(true)
, _enable_blend(false)
, _polygon_offset_factor(0), _polygon_offset_units(0)
{
    reload_shader();
}

Shader::~Shader() {
    destroy_shader();
}

void Shader::use() const {
    gl_state::use_program(_program);

    gl_state::set_state(GL_DEPTH_TEST, _depth_test || _depth_write);
    gl_state::set_depth_mask(_depth_write);
    gl_state::set_depth_func(_depth_test ? GL_LESS : GL_ALWAYS);

    gl_state::set_state(GL_BLEND, _enable_blend);
    if (_enable_blend) {
        gl_state::set_blend_func(_blend_sfactor, _blend_dfactor);
    }

    auto enable_offset = _polygon_offset_factor != 0 ||
                         _polygon_offset_units != 0;
    gl_state::set_state(GL_POLYGON_OFFSET_FILL, enable_offset);
    if (enable_offset) {
        gl_state::set_polygon_offset(_polygon_offset_factor,
                                     _polygon_offset_units);
    }
}

void Shader::set_projection_uniform(Matrix4 projection_matrix) const {
    gl_state::set_uniform(_program,
                          _uniforms.at(kUniformNames[(int)Uniforms::kProjectionMatrixUniform]),
                          projection_matrix);
}

void Shader::set_modelview_uniform(Matrix4 modelview_matrix) const {
    gl_state::set_uniform(_program,
                          _uniforms.at(kUniformNames[(int)Uniforms::kModelViewMatrixUniform]),
                          modelview_matrix);
}

void Shader::set_normal_uniform(Matrix3 normal_matrix) const {
    gl_state::set_uniform(_program,
                          _uniforms.at(kUniformNames[(int)Uniforms::kNormalMatrixUniform]),
                          normal_matrix);
}

void Shader::set_texture_sampler_uniform(int sampler) const {
    gl_state::set_uniform(_program,
                          _uniforms.at(kUniformNames[(int)Uniforms::kTextureSamplerUniform]),
                          sampler);
}

void Shader::set_light_position_uniform(math::Vector3 light_position) const {
    gl_state::set_uniform(_program,
                          _uniforms.at(kUniformNames[(int)Uniforms::kLightPositionUniform]),
                          Vector4(light_position.x(), light_position.y(), light_position.z(), 0));
}

void Shader::set_light_color_uniform(math::Vector4 light_color) const {
    gl_state::set_uniform(_program,
                          _uniforms.at(kUniformNames[(int)Uniforms::kLightColorUniform]),
                          light_color);
}

void Shader::initialize_attribute_arrays() const {
    int stride = attribute_stride();

    int offset = 0;
    gl_state::enable_vertex_attrib_array((int)Attributes::kPositionAttribute);
    gl_state::set_vertex_attrib_pointer((int)Attributes::kPositionAttribute,
                                        VertexAttribPointer(3, kTypeFloat, false,
                                                            stride, offset));
    offset += 3 * sizeof(float);

    gl_state::enable_vertex_attrib_array((int)Attributes::kNormalAttribute);
    gl_state::set_vertex_attrib_pointer((int)Attributes::kNormalAttribute,
                                        VertexAttribPointer(3, kTypeFloat, true,
                                                            stride, offset));
    offset += 3 * sizeof(float);

    gl_state::enable_vertex_attrib_array((int)Attributes::kTexCoordAttribute);
    gl_state::set_vertex_attrib_pointer((int)Attributes::kTexCoordAttribute,
                                        VertexAttribPointer(2, kTypeFloat, false,
                                                            stride, offset));
    offset += 2 * sizeof(float);

    int attr = (int)Attributes::kNumAttributes;
    for (auto spec : _extra_attributes) {
        gl_state::enable_vertex_attrib_array(attr);
        gl_state::set_vertex_attrib_pointer(attr,
                                            VertexAttribPointer(spec.n_comps, spec.type,
                                                                spec.normalized, stride, offset));

        ++attr;
        offset += spec.width;
    }
}

int Shader::attribute_stride() const {
    return (int)std::accumulate(begin(_extra_attributes), end(_extra_attributes),
                                (3 + 3 + 2) * sizeof(float),
                                [](int sum, AttributeSpec spec) { return sum + spec.width; });
}

const std::vector<Shader::AttributeSpec> &Shader::extra_attributes() const {
    return _extra_attributes;
}

void Shader::destroy_shader() {
    gl_state::delete_program(_program);
    _program = UINT_MAX;
}

void Shader::reload_shader() {
    gl_state::uniform_map known_state;

    if (_program != UINT_MAX) {
        known_state = gl_state::known_uniform_state(_program);
        destroy_shader();
    }

    auto &contents = _shader_spec->str();
    msgpack::unpacked spec_pack;
    msgpack::unpack(&spec_pack, contents.c_str(), contents.size());

    auto spec = spec_pack.get();

    std::string vertex_shader_source, fragment_shader_source;

    for (int i = 0; i < spec.via.map.size; ++i) {
        auto &kv = spec.via.map.ptr[i];

        auto key = kv.key.as<std::string>();

        if (key == "vertex-shader") {
            vertex_shader_source = kv.val.as<std::string>();
        }
        else if (key == "fragment-shader") {
            fragment_shader_source = kv.val.as<std::string>();
        }
        else if (key == "extra-attributes") {
            auto specs = kv.val.as<std::vector<std::vector<std::string>>>();
            _extra_attributes.clear();

            for (auto s : specs) {
                auto name = s[0];
                auto spec_str = s[1];

                AttributeSpec spec;
                spec.name = name;

                if (spec_str == "vec4") {
                    spec.n_comps = 4;
                    spec.width = 4 * sizeof(float);
                    spec.normalized = false;
                    spec.type = kTypeFloat;
                }

                _extra_attributes.push_back(spec);
            }
        }
        else if (key == "queue-index") {
            _queue_index = kv.val.as<int>();
        }
        else if (key == "depth-test") {
            _depth_test = kv.val.as<bool>();
        }
        else if (key == "depth-write") {
            _depth_write = kv.val.as<bool>();
        }
        else if (key == "blend-mode") {
            if (kv.val.is_nil()) continue;

            auto sfactor_name = kv.val.via.array.ptr[0].as<std::string>();
            auto dfactor_name = kv.val.via.array.ptr[1].as<std::string>();

            _blend_dfactor = convert_blend_name(dfactor_name);
            _blend_sfactor = convert_blend_name(sfactor_name);
            _enable_blend = true;
        }
        else if (key == "polygon-offset") {
            if (kv.val.is_nil()) continue;

            _polygon_offset_factor = kv.val.via.array.ptr[0].as<float>();
            _polygon_offset_units = kv.val.via.array.ptr[1].as<float>();
        }
    }

    _program = GL_FUNC(glCreateProgram)();

    GLuint vertex_shader;
    if (!compile_shader(&vertex_shader, GL_VERTEX_SHADER, vertex_shader_source)) {
        LOG(ERROR, "Could not compile vertex shader");
        return;
    }

    GLuint fragment_shader;
    if (!compile_shader(&fragment_shader, GL_FRAGMENT_SHADER, fragment_shader_source)) {
        LOG(ERROR, "Could not compile fragment shader");
        return;
    }

    GL_FUNC(glAttachShader)(_program, vertex_shader);
    GL_FUNC(glAttachShader)(_program, fragment_shader);

    auto n_attrs = (int)Attributes::kNumAttributes;

    for (GLuint index = 0; index < n_attrs; ++index) {
        GL_FUNC(glBindAttribLocation)(_program, index, kAttributeNames[index]);
    }

    for (GLuint index = 0; index < _extra_attributes.size(); ++index) {
        GL_FUNC(glBindAttribLocation)(_program, n_attrs + index,
                                      _extra_attributes[index].name.c_str());
    }

    if (!link_program(_program)) {
        LOG(ERROR, "Could not link shader program");
    }

    for (GLuint index = 0; index < (int)Uniforms::kNumUniforms; ++index) {
        auto name = kUniformNames[index];
        _uniforms[name] = GL_FUNC(glGetUniformLocation)(_program, name);
        if (_uniforms[name] == -1) {
            LOG(WARNING) << "Could not find uniform " << kUniformNames[index];
        }
    }

    if (!known_state.empty()) {
        gl_state::set_uniforms(_program, known_state);
    }
}

bool Shader::compile_shader(GLuint *out_shader, GLenum shader_type, std::string shader_source) {
    GLuint shader = GL_FUNC(glCreateShader)(shader_type);
    const char *source = shader_source.c_str();

    GL_FUNC(glShaderSource)(shader, 1, &source, nullptr);
    GL_FUNC(glCompileShader)(shader);

    GLint shader_log_length = 0;
    GL_FUNC(glGetShaderiv)(shader, GL_INFO_LOG_LENGTH, &shader_log_length);
    if (shader_log_length) {
        std::string log((size_t)shader_log_length - 1, '\0');
        GL_FUNC(glGetShaderInfoLog)(shader, shader_log_length, &shader_log_length, &log.front());

		if (shader_log_length) LOG(WARNING) << "Shader info log: " << log;
    }

    GLint status = 0;
    GL_FUNC(glGetShaderiv)(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        GL_FUNC(glDeleteShader)(shader);
        return false;
    }

    *out_shader = shader;
    return true;
}

bool Shader::link_program(GLuint program) {
    GL_FUNC(glLinkProgram)(program);

    GLint program_log_length = 0;
    GL_FUNC(glGetProgramiv)(program, GL_INFO_LOG_LENGTH, &program_log_length);
    if (program_log_length) {
        std::string log((size_t)program_log_length - 1, '\0');
        GL_FUNC(glGetProgramInfoLog)(program, program_log_length, &program_log_length, &log.front());

        if (program_log_length) LOG(WARNING) << "Program info log: " << log;
    }

    GLint status = 0;
    GL_FUNC(glGetProgramiv)(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        return false;
    }

    return true;
}
