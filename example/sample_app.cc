#include "sample_app.h"

#include "rosewood/core/memory.h"
#include "rosewood/core/resource_manager.h"

#include "rosewood/math/math_types.h"
#include "rosewood/math/quaternion.h"

#include "rosewood/graphics/texture.h"
#include "rosewood/graphics/render_queue.h"
#include "rosewood/graphics/gl_func.h"
#include "rosewood/graphics/shader.h"
#include "rosewood/graphics/scene_object.h"
#include "rosewood/graphics/mesh.h"
#include "rosewood/graphics/material.h"
#include "rosewood/graphics/renderer.h"

#include "rosewood/utils/scene.h"
#include "rosewood/utils/time.h"

using rosewood::core::get_resource;

using rosewood::math::Vector3;
using rosewood::math::quaternion_from_axis_angle;

using rosewood::graphics::Texture;
using rosewood::graphics::Shader;
using rosewood::graphics::SceneObject;
using rosewood::graphics::Mesh;
using rosewood::graphics::Material;
using rosewood::graphics::Renderer;

using rosewood::utils::delta_time;

using sample_app::RosewoodApp;

RosewoodApp *RosewoodApp::create() {
    return new RosewoodApp();
}

void RosewoodApp::destroy(RosewoodApp *app) {
    delete app;
}

RosewoodApp::RosewoodApp() : _scene(make_unique<Scene>()), _rotation(0) {
    GL_FUNC(glEnable)(GL_DEPTH_TEST);
    GL_FUNC(glEnable)(GL_BLEND);
    GL_FUNC(glEnable)(GL_CULL_FACE);

    GL_FUNC(glBlendFunc)(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    GL_FUNC(glCullFace)(GL_BACK);

    auto checkered = std::make_shared<Texture>(get_resource("Space.png"));

    auto shader = std::make_shared<Shader>(get_resource("Shaders/Shader." SHADER_EXT ".rwshader-mp"));

    auto camera_object = std::make_shared<SceneObject>("Main Camera");
    auto camera = camera_object->add_component<Camera>();
    camera->set_fov(65.0f * ((float)M_PI/180.0f));
    camera->set_z_planes(0.1f, 100.0f);

    camera_object->set_local_position(0.0f, 0.0f, 4.0f);

    auto mesh = Mesh::create(get_resource("Cube 1x1x1.mesh-mp"));
    mesh->set_default_texcoord_data_key("Wrap");

    auto material = std::make_shared<Material>();
    material->set_shader(shader);
    material->set_texture(checkered);

    _cube1 = std::make_shared<SceneObject>("Cube 1");
    _cube1->set_local_position(0, 0, -1.5);
    auto renderer = _cube1->add_component<Renderer>();
    renderer->set_mesh(mesh);
    renderer->set_material(material);

    material = std::make_shared<Material>();
    material->set_shader(shader);
    material->set_texture(checkered);

    _cube2 = std::make_shared<SceneObject>("Cube 2");
    _cube2->set_local_position(0, 0, 1.5);
    renderer = _cube2->add_component<Renderer>();
    renderer->set_mesh(mesh);
    renderer->set_material(material);

    auto parent = std::make_shared<SceneObject>("Parent");
    parent->add_child(_cube1);
    parent->add_child(_cube2);

    auto root = std::make_shared<SceneObject>("Root");
    root->add_child(camera_object);
    root->add_child(parent);

    _scene->set_root_node(root);
    _scene->set_main_camera_node(camera_object);

    _main_camera = camera;

    GL_FUNC(glClearColor)(0.65f, 0.65f, 0.65f, 1.0f);
}

void RosewoodApp::update() {
    auto parent = _cube1->parent();

    _cube1->set_local_rotation(quaternion_from_axis_angle(Vector3(1, 1, 1),
                                                          _rotation));
    _cube2->set_local_rotation(quaternion_from_axis_angle(Vector3(-1, -1, -1),
                                                          _rotation));
    parent->set_local_rotation(quaternion_from_axis_angle(Vector3(0, 1, 0),
                                                          _rotation));


    _rotation += delta_time() * 0.5f;

    _scene->update();
}

void RosewoodApp::draw() const {
    GL_FUNC(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _scene->draw();
}

void RosewoodApp::reshape_viewport(float width, float height) {
    GL_FUNC(glViewport)(0, 0, (int)width, (int)height);
    _main_camera->set_aspect(fabsf(width/height));
}
