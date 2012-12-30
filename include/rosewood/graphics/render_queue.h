#ifndef __ROSEWOOD_GRAPHICS_RENDER_QUEUE_H__
#define __ROSEWOOD_GRAPHICS_RENDER_QUEUE_H__

#include <memory>
#include <vector>

#include "rosewood/graphics/camera.h"
#include "rosewood/graphics/scene_object.h"
#include "rosewood/graphics/material.h"

#include "rosewood/math/math_types.h"
#include "rosewood/math/matrix4.h"

namespace rosewood { namespace graphics {

    class Mesh;
    class Material;
    class RenderQueue;
    class Shader;

    class RenderCommand {
    public:
        RenderCommand(Mesh *mesh,
                      math::Matrix4 transform,
                      math::Matrix4 inverse_transform,
                      float max_axis_scale,
                      Material *material,
                      Camera *camera);

        void execute(const RenderCommand *previous) const;
        void flush() const;

    private:
        Camera *_camera;
        Mesh *_mesh;
        math::Matrix4 _transform;
        math::Matrix4 _inverse_transform;
        float _max_axis_scale;
        Material *_material;
        Shader *_shader;

        void activate_material() const;

        friend class RenderQueue;
        friend bool operator<(const RenderCommand &lhs, const RenderCommand &rhs);
    };

    class RenderQueue {
    public:
        void clear();
        void add_command(RenderCommand command);
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
                                        Camera *camera)
    : _camera(camera), _mesh(mesh)
    , _transform(camera->owner()->inverse_world_transform() * transform)
    , _inverse_transform(inverse_transform * camera->owner()->world_transform())
    , _max_axis_scale(max_axis_scale)
    , _material(material)
    , _shader(material->shader().get()) { }

    inline void RenderQueue::add_command(RenderCommand command) {
        _commands.push_back(command);
    }
} }

#endif
