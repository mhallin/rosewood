#include <emscripten/emscripten.h>

#include "rosewood/core/memory.h"
#include "rosewood/core/logging.h"

#include "rosewood/graphics/platform_gl.h"

#include "rosewood/utils/folder_resource_loader.h"
#include "rosewood/utils/time.h"

#include "../sample_app.h"

using namespace rosewood::core;
using namespace rosewood::utils;

using namespace sample_app;

static RosewoodApp *gRosewoodApp = nullptr;

static bool init_gl() {
    if (glfwInit() != GL_TRUE) {
        LOG(ERROR, "glfwInit failed");
        return false;
    }

    if (glfwOpenWindow(640, 480, 8, 8, 8, 8, 16, 0, GLFW_WINDOW) != GL_TRUE) {
        LOG(ERROR, "glfwOpenWindow failed");
        return false;
    }

    LOG(INFO, "GL inited successfully");

    return true;
}

static void tick_frame() {
    mark_frame_beginning();
    gRosewoodApp->update();
    gRosewoodApp->draw();
}

int main(int argc, char **argv) {
    add_resource_loader(make_unique<FolderResourceLoader>("/"));

    if (!init_gl()) return 1;

    gRosewoodApp = RosewoodApp::create();
    mark_frame_beginning();

    if (!gRosewoodApp) {
        LOG(ERROR, "Could not create Rosewood App");
        return 1;
    }

    gRosewoodApp->reshape_viewport(640, 480);

    LOG(DBG, "Entering main loop...");

    emscripten_set_main_loop(tick_frame, 0, 1);

    LOG(DBG, "Destroying Rosewood App");

    RosewoodApp::destroy(gRosewoodApp);
    gRosewoodApp = nullptr;

    return 0;
}