// includes that we will need
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

// BCM?
#include "bcm_host.h"

// GLES and EGL
#include "GLES/gl.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

// define our state container
typedef struct {
	// Screen objects
	uint32_t screenWidth;
	uint32_t screenHeight;
	
	// OpenGL|ES objects
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
} STATE_STRUCT;

// and create our state and a pointer to it
static STATE_STRUCT _state, *state = &_state;

// our initialization function
static void initializeOpenGL(STATE_STRUCT *state) {
	EGLBoolean result;
	EGLint num_config;

	static EGL_DISPMANX_WINDOW_T nativewindow;

	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;

	static const EGLint attribute_list[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};

	EGLConfig config;

	// get an EGL display connection
	state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	assert(state->display!=EGL_NO_DISPLAY);

	// initialize the EGL display connection
	result = eglInitialize(state->display, NULL, NULL);
	assert(EGL_FALSE != result);

	// get an appropriate EGL frame buffer configuration
	result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
	assert(EGL_FALSE != result);

	// create an EGL rendering context
	state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, NULL);
	assert(state->context!=EGL_NO_CONTEXT);

	// create an EGL window surface
	int32_t success = graphics_get_display_size(0 /* LCD */, &state->screenWidth, &state->screenHeight);
	assert(success >= 0);

	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.width = state->screenWidth;
	dst_rect.height = state->screenHeight;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = state->screenWidth << 16;
	src_rect.height = state->screenHeight << 16;        

	dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
	dispman_update = vc_dispmanx_update_start( 0 );

	dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
	0/*layer*/, &dst_rect, 0/*src*/,
	&src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, 0/*transform*/);

	nativewindow.element = dispman_element;
	nativewindow.width = state->screenWidth;
	nativewindow.height = state->screenHeight;
	vc_dispmanx_update_submit_sync(dispman_update);

	state->surface = eglCreateWindowSurface(state->display, config, &nativewindow, NULL);
	assert(state->surface != EGL_NO_SURFACE);

	// connect the context to the surface
	result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
	assert(EGL_FALSE != result);

	// Set background color and clear buffers
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear( GL_COLOR_BUFFER_BIT );
	glClear( GL_DEPTH_BUFFER_BIT );
	glShadeModel(GL_FLAT);

	// Enable back face culling.
	glEnable(GL_CULL_FACE);
}

// our function for when we're done
static void onExit() {
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT);
	eglSwapBuffers(state->display, state->surface);
	
	// release opengl resources
	eglMakeCurrent( state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
	eglDestroySurface( state->display, state->surface );
	eglDestroyContext( state->display, state->context );
	eglTerminate( state->display );
	
	printf("Exiting...\n");
}

int main() {
	// initialize bcm
	bcm_host_init();
	
	// clear the application state
	memset(state, 0, sizeof(*state));
	
	// start OpenGL ES (OGLES)
	initializeOpenGL(state);
	
	// our program loop
	while(1) {
	}
	
	// handle exiting
	onExit();
	return 0;
}