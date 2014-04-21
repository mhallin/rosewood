#ifndef __ROSEWOOD_GRAPHICS_RENDER_QUEUE_H__
#define __ROSEWOOD_GRAPHICS_RENDER_QUEUE_H__

#include <memory>
#include <vector>

#include "rosewood/core/assert.h"
#include "rosewood/core/entity.h"
#include "rosewood/core/transform.h"

#include "rosewood/graphics/camera.h"
#include "rosewood/graphics/material.h"

#include "rosewood/math/math_types.h"
#include "rosewood/math/matrix4.h"

namespace rosewood { namespace graphics {

    class Mesh;
    class Material;
    class RenderQueue;
    class Shader;
    class Light;

    class RenderCommand {
    public:
        RenderCommand(Mesh *mesh,
                      math::Matrix4 transform,
                      math::Matrix4 inverse_transform,
                      float max_axis_scale,
                      Material *material,
                      Camera *camera,
                      Light *light);

        void execute(const RenderCommand *previous) const;
        void flush() const;
        bool is_visible() const;

    private:
        Camera *_camera;
        Mesh *_mesh;
        math::Matrix4 _transform;
        math::Matrix4 _inverse_transform;
        float _max_axis_scale;
        Material *_material;
        Shader *_shader;
        Light *_light;

        void activate_material() const;

        friend class RenderQueue;
        friend bool operator<(const RenderCommand &lhs, const RenderCommand &rhs);
    };

    class RenderQueue {
    public:
        void clear();
        void add_command(const RenderCommand &command);
        void sort();
        void run();

    private:
        std::vector<RenderCommand> _commands;
    };

    inline RenderCommand::RenderCommand(Mesh *mesh,
                                        math::Matrix4 transform,
                                        math::Matrix4 inverse_transform,
                                        float max_axis_scale,
                                        Material *material,
                                        Camera *camera,
                                        Light *light)
    : _camera(camera), _mesh(mesh)
    , _max_axis_scale(max_axis_scale)
    , _material(material)
    , _shader(material->shader().get())
    , _light(light) {
        RW_ASSERT(_material, "Expecting material for rendering");
        RW_ASSERT(_camera, "Expecting camera for rendering");
        RW_ASSERT(_mesh, "Expecting mesh for rendering");
        auto camera_transform = core::transform(camera->entity());
        _transform = math::make_hand_shift4() * camera_transform->inverse_world_transform() * transform;
        _inverse_transform = inverse_transform * camera_transform->world_transform() * math::make_hand_shift4();
    }

    inline void RenderQueue::add_command(const RenderCommand &command) {
        if (command.is_visible()) {
            _commands.emplace_back(command);
        }
    }
} }

#endif
