#include "eglManager.h"

EGLManager::EGLManager() {
}

EGLManager::~EGLManager() {
	// release our resources
	eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroySurface(display, surface);
	eglDestroyContext(display, context);
	eglTerminate(display);
}

// initialization function
void EGLManager::initialize() {
	// initialize BCM
	bcm_host_init();
	
	int32_t success = 0;
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

	static const EGLint context_attributes[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	EGLConfig config;

	// get an EGL display connection
	this->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	assert(this->display!=EGL_NO_DISPLAY);
	check();

	// initialize the EGL display connection
	result = eglInitialize(this->display, NULL, NULL);
	assert(EGL_FALSE != result);
	check();

	// get an appropriate EGL frame buffer configuration
	result = eglChooseConfig(this->display, attribute_list, &config, 1, &num_config);
	assert(EGL_FALSE != result);
	check();

	// get an appropriate EGL frame buffer configuration
	result = eglBindAPI(EGL_OPENGL_ES_API);
	assert(EGL_FALSE != result);
	check();

	// create an EGL rendering context
	this->context = eglCreateContext(this->display, config, EGL_NO_CONTEXT, context_attributes);
	assert(this->context != EGL_NO_CONTEXT);
	check();

	// create an EGL window surface
	success = graphics_get_display_size(0 /* LCD */, &this->screenWidth, &this->screenHeight);
	assert( success >= 0 );

	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.width = this->screenWidth;
	dst_rect.height = this->screenHeight;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = this->screenWidth << 16;
	src_rect.height = this->screenHeight << 16;		

	dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
	dispman_update = vc_dispmanx_update_start( 0 );
	 
	dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
	0/*layer*/, &dst_rect, 0/*src*/,
	&src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, DISPMANX_NO_ROTATE);

	nativewindow.element = dispman_element;
	nativewindow.width = this->screenWidth;
	nativewindow.height = this->screenHeight;
	vc_dispmanx_update_submit_sync( dispman_update );

	check();

	this->surface = eglCreateWindowSurface( this->display, config, &nativewindow, NULL );
	assert(this->surface != EGL_NO_SURFACE);
	check();

	// connect the context to the surface
	result = eglMakeCurrent(this->display, this->surface, this->surface, this->context);
	assert(EGL_FALSE != result);
	check();

	// Set background color and clear buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	check();
}

void EGLManager::swapBuffers() {
	eglSwapBuffers(display, surface);
}

uint32_t EGLManager::getScreenWidth() {
	return screenWidth;
}

uint32_t EGLManager::getScreenHeight() {
	return screenHeight;
}