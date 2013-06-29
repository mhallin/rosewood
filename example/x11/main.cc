#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <iostream>
#include <thread>

#include "rosewood/core/memory.h"

#include "rosewood/graphics/platform_gl.h"

#include "rosewood/utils/animatable.h"
#include "rosewood/utils/folder_resource_loader.h"
#include "rosewood/utils/time.h"
#include "rosewood/utils/logging.h"

#define GLX_GLXEXT_PROTOTYPES

#include <GL/glx.h>
#include <GL/glxext.h>

#include "../sample_app.h"

typedef sample_app::RosewoodApp AppClass;

static GLint gl3_attributes[] = { GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
								  GLX_CONTEXT_MINOR_VERSION_ARB, 2,
								  GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
								  None };
static GLint visual_attributes[] = { GLX_X_RENDERABLE, True,
									 GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
									 GLX_RENDER_TYPE, GLX_RGBA_BIT,
									 GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
									 GLX_RED_SIZE, 8,
									 GLX_GREEN_SIZE, 8,
									 GLX_BLUE_SIZE, 8,
									 GLX_ALPHA_SIZE, 8,
									 GLX_DEPTH_SIZE, 24,
									 GLX_DOUBLEBUFFER, True,
									 None };

static Display *main_display = nullptr;
static Window root_window;
static XVisualInfo *visual_info = nullptr;
static Colormap colormap;
static XSetWindowAttributes window_attributes;
static Window dest_window;
static GLXContext gl_context;

static AppClass *app;

using rosewood::core::add_resource_loader;

using rosewood::utils::FolderResourceLoader;

static void render_loop(GLXFBConfig fbcfg, bool *is_rendering) {
	XLockDisplay(main_display);

	gl_context = glXCreateContextAttribsARB(main_display, fbcfg, NULL, GL_TRUE, gl3_attributes);
	glXMakeCurrent(main_display, dest_window, gl_context);

	LOG(INFO) << "OpenGL visual id: " << (void*)visual_info->visualid;
	LOG(INFO) << "OpenGL vendor:    " << glGetString(GL_VENDOR);
	LOG(INFO) << "OpenGL renderer:  " << glGetString(GL_RENDERER);
	LOG(INFO) << "OpenGL version:   " << glGetString(GL_VERSION);
	LOG(INFO) << "GLSL version:     " << glGetString(GL_SHADING_LANGUAGE_VERSION);

	app = AppClass::create();

	app->reshape_viewport(1024, 768);
	app->update();

	GLXDrawable drawable = glXGetCurrentDrawable();
	glXSwapIntervalEXT(main_display, drawable, 1);

	glXMakeCurrent(main_display, dest_window, gl_context);

	XUnlockDisplay(main_display);

	uint64_t last_print_time = rosewood::utils::current_usec_time();
	int frames_since_last_print = 0;

	LOG(INFO, "Starting render loop");

	while (*is_rendering) {
		rosewood::utils::mark_frame_beginning();
		rosewood::utils::tick_all_animations();

		app->update();
		app->draw();
		glFlush();
		glXSwapBuffers(main_display, dest_window);
		++frames_since_last_print;

		uint64_t now = rosewood::utils::current_usec_time();
		uint64_t usec_diff = now - last_print_time;

		if (usec_diff >= rosewood::utils::kUsecPerSec) {
			last_print_time = now;
			LOG(DEBUG) << "FPS: " << (frames_since_last_print * rosewood::utils::kUsecPerSec / (double)usec_diff);
			frames_since_last_print = 0;
		}
	}

	LOG(INFO, "Stopping render loop");

	AppClass::destroy(app);

	glXMakeCurrent(main_display, None, nullptr);
	glXDestroyContext(main_display, gl_context);
}

int main(int, const char **) {
	XInitThreads();

	main_display = XOpenDisplay(NULL);
	if (!main_display) {
		LOG(ERROR, "Could not connect to X server");
		return 1;
	}

	root_window = DefaultRootWindow(main_display);

	int fb_elem_count;
	GLXFBConfig *fbcfg = glXChooseFBConfig(main_display, 0, visual_attributes, &fb_elem_count);
	if (!fbcfg) {
		LOG(ERROR, "Could not get framebuffer configs");
		return 1;
	}

	visual_info = glXGetVisualFromFBConfig(main_display, fbcfg[0]);

	if (!visual_info) {
		LOG(ERROR, "Could not find GL visual");
		return 1;
	}

	colormap = XCreateColormap(main_display, root_window, visual_info->visual, AllocNone);

	window_attributes.colormap = colormap;
	window_attributes.event_mask = ExposureMask | KeyPressMask;

	dest_window = XCreateWindow(main_display, root_window,
								0, 0, 1024, 768,
								0, visual_info->depth,
								InputOutput,
								visual_info->visual,
								CWColormap | CWEventMask, &window_attributes);

	char cwd[512];
	if (getenv("ROSEWOOD_ROOT")) {
		strcpy(cwd, getenv("ROSEWOOD_ROOT"));
	}
	else {
		getcwd(cwd, sizeof cwd);
	}

	LOG(INFO) << "Rosewood Root: " << cwd;

	add_resource_loader(make_unique<FolderResourceLoader>(cwd));

	XMapWindow(main_display, dest_window);
	XStoreName(main_display, dest_window, "Rosewood Example");

	bool is_rendering = true;

	std::thread render_thread(render_loop, fbcfg[0], &is_rendering);

	LOG(INFO, "Starting X event processing");

	while (is_rendering) {
		XEvent event;
		XNextEvent(main_display, &event);

		switch (event.type) {
		case Expose:
			// glXMakeCurrent(main_display, dest_window, gl_context);
			// std::cerr << "Drawing\n";
			// app->update();
			// app->draw();
			// glFlush();
			// glXSwapBuffers(main_display, dest_window);
			break;
		case KeyPress:
			is_rendering = false;
			break;
		}
	}

	LOG(INFO, "Waiting for render thread to finish");

	render_thread.join();

	XDestroyWindow(main_display, dest_window);
	XCloseDisplay(main_display);

	return 0;
}
