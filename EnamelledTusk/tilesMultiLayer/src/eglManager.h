#ifndef _EGL_MANAGER_H_
#define _EGL_MANAGER_H_

// some things that we will need
#include <assert.h>

// BCM?
#include "bcm_host.h"

// GLES and EGL
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#define check() assert(glGetError() == 0)

class EGLManager {
private:
	// information about our screen
	uint32_t screenWidth;
	uint32_t screenHeight;

	// our EGL objects
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	
public:
	// constructors and destructors
	EGLManager();
	~EGLManager();

	// call this to start EGL
	void initialize();
	
	// 
	void swapBuffers();
	
	// access functions
	uint32_t getScreenWidth();
	uint32_t getScreenHeight();
};

#endif