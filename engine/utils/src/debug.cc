#include "rosewood/utils/debug.h"

#include <map>
#include <vector>

#include "rosewood/graphics/gl_func.h"

namespace rosewood { namespace utils {

    struct UniformInfo {
        std::string type_name;
        size_t byte_size;
        GLenum element_type;
    };

    static void dump_current_shader_info();
    static void dump_shader_info(GLint program);
    static UniformInfo uniform_type_info(GLenum e);

    void dump_gl_state() {
        std::cerr << "GL State\n";
        std::cerr << "========\n";

        dump_current_shader_info();
    }

    static void dump_current_shader_info() {
        GLint program;
        GL_FUNC(glGetIntegerv)(GL_CURRENT_PROGRAM, &program);

        std::cerr << "Current Program: " << program << "\n";
        if (program) {
            dump_shader_info(program);
        }
        else {
            std::cerr << "(no current shader program)\n";
        }
    }

    static void dump_shader_info(GLint program) {
        std::cerr << "Shader Info\n";
        std::cerr << "-----------\n";

        GLint active_uniforms, uniform_max_length;
        GL_FUNC(glGetProgramiv)(program, GL_ACTIVE_UNIFORMS, &active_uniforms);
        GL_FUNC(glGetProgramiv)(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniform_max_length);

        for (GLint uniform = 0; uniform < active_uniforms; ++uniform) {
            std::string name(uniform_max_length, '\0');
            GLsizei length;
            GLint size;
            GLenum type;

            glGetActiveUniform(program, uniform, uniform_max_length,
                               &length, &size, &type, &name[0]);
            name.resize(length);

            GLint location = glGetUniformLocation(program, name.c_str());

            auto uti = uniform_type_info(type);

            std::cerr << "* Uniform " << uniform << " \"" << name << "\"\n";
            std::cerr << "  Location: " << location << "\n";
            std::cerr << "  Type: " << uti.type_name << " (elements " << size << ", bytes " << uti.byte_size << ")\n";

            switch (uti.element_type) {
                case GL_FLOAT: {
                    std::vector<float> felems(uti.byte_size / sizeof(float));
                    GL_FUNC(glGetUniformfv)(program, location, &felems.front());
                    std::cerr << "  Elements: [ ";
                    std::copy(begin(felems), end(felems), std::ostream_iterator<float>(std::cerr, ", "));
                    std::cerr << " ]\n";
                    break;
                }
                case GL_INT: {
                    std::vector<int> ielems(uti.byte_size / sizeof(int));
                    GL_FUNC(glGetUniformiv)(program, location, &ielems.front());
                    std::cerr << "  Elements: [ ";
                    std::copy(begin(ielems), end(ielems), std::ostream_iterator<int>(std::cerr, ", "));
                    std::cerr << " ]\n";
                }
            }
        }
    }

    static UniformInfo uniform_type_info(GLenum e) {
        static std::map<GLenum, UniformInfo> mapping{
            { GL_FLOAT, { "float", sizeof(float), GL_FLOAT } },
            { GL_FLOAT_MAT3, { "mat3", sizeof(float) * 3 * 3, GL_FLOAT } },
            { GL_FLOAT_MAT4, { "mat4", sizeof(float) * 4 * 4, GL_FLOAT } },
            { GL_SAMPLER_2D, { "sampler2D", sizeof(int), GL_INT } },
        };

        return mapping.at(e);
    }

} }
