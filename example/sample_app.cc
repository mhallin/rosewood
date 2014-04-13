#include "sample_app.h"

#include "rosewood/core/memory.h"
#include "rosewood/core/resource_manager.h"
#include "rosewood/core/transform.h"

#include "rosewood/math/math_types.h"
#include "rosewood/math/quaternion.h"
#include "rosewood/math/trig.h"

#include "rosewood/graphics/texture.h"
#include "rosewood/graphics/render_queue.h"
#include "rosewood/graphics/gl_func.h"
#include "rosewood/graphics/shader.h"
#include "rosewood/graphics/mesh.h"
#include "rosewood/graphics/material.h"
#include "rosewood/graphics/renderable.h"
#include "rosewood/graphics/camera.h"
#include "rosewood/graphics/light.h"

#include "rosewood/utils/time.h"

using namespace rosewood::core;
using namespace rosewood::math;
using namespace rosewood::graphics;
using namespace rosewood::utils;

using sample_app::RosewoodApp;

RosewoodApp *RosewoodApp::create() {
    return new RosewoodApp();
}

void RosewoodApp::destroy(RosewoodApp *app) {
    delete app;
}

RosewoodApp::RosewoodApp() : _render_system(&_entity_manager, &_scene_mutex), _rotation(0) {
    gl_state::enable(GL_DEPTH_TEST);
    gl_state::enable(GL_BLEND);
    gl_state::enable(GL_CULL_FACE);

    gl_state::set_blend_func(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    GL_FUNC(glFrontFace)(GL_CW);
    GL_FUNC(glCullFace)(GL_BACK);

    auto checkered = Texture::create("Space.png");
    auto shader = Shader::create("Shaders/main_shader." SHADER_EXT);

    auto camera_object = _entity_manager.create_entity();
    _main_camera = camera_object.add_component<Camera>();
    _main_camera->set_fov(deg2rad(45.0f));
    _main_camera->set_z_planes(0.1f, 100.0f);

    camera_object.add_component<Transform>()->set_local_position(0.0f, 0.0f, -4.0f);

    auto mesh = Mesh::create(get_resource("Cube 1x1x1.mesh-mp"));
    mesh->set_default_texcoord_data_key("Wrap");

    auto material = std::make_shared<Material>();
    material->set_shader(shader);
    material->set_texture(checkered);

    _cube1 = _entity_manager.create_entity<Transform, Renderable>();
    _cube1.component<Transform>()->set_local_position(0, 0, -1.5);
    auto renderable = _cube1.component<Renderable>();
    renderable->set_mesh(mesh);
    renderable->set_material(material);

    material = std::make_shared<Material>();
    material->set_shader(shader);
    material->set_texture(checkered);

    _cube2 = _entity_manager.create_entity<Transform, Renderable>();
    _cube2.component<Transform>()->set_local_position(0, 0, 1.5);
    renderable = _cube2.component<Renderable>();
    renderable->set_mesh(mesh);
    renderable->set_material(material);

    auto parent = _entity_manager.create_entity().add_component<Transform>();
    parent->add_child(_cube1.component<Transform>());
    parent->add_child(_cube2.component<Transform>());

    _root = _entity_manager.create_entity();
    auto root_tform = _root.add_component<Transform>();
    root_tform->add_child(camera_object.component<Transform>());
    root_tform->add_child(parent);

    auto light_obj = _entity_manager.create_entity<Transform, Light>();
    root_tform->add_child(light_obj.component<Transform>());
    light_obj.component<Transform>()->set_local_position(0, 0, 3);
    auto light = light_obj.component<Light>();
    light->set_color(Vector4(0.2f, 0.6f, 0.2f, 1.0f));

    GL_FUNC(glClearColor)(0.662f, 0.231f, 0.071f, 1.0f);
}

void RosewoodApp::update() {
    auto c1tform = _cube1.component<Transform>();
    auto c2tform = _cube2.component<Transform>();
    auto parent = c1tform->parent();

    c1tform->set_local_rotation(quaternion_from_axis_angle(Vector3(1, 1, 1),
                                                           _rotation));
    c2tform->set_local_rotation(quaternion_from_axis_angle(Vector3(-1, -1, -1),
                                                           _rotation));
    parent->set_local_rotation(quaternion_from_axis_angle(Vector3(0, 1, 0),
                                                          _rotation));


    _rotation += delta_time() * 0.5f;
}

void RosewoodApp::draw() {
    gl_state::set_depth_mask(true);
    GL_FUNC(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _render_system.draw();
}

void RosewoodApp::reshape_viewport(float width, float height) {
    GL_FUNC(glViewport)(0, 0, (int)width, (int)height);
    _main_camera->set_aspect(fabsf(width/height));
}
