#ifndef _OGL_MANAGER_H_
#define _OGL_MANAGER_H_

// GLES and EGL
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

// our EGL manager
#include "eglManager.h"

class OGLManager {
private:
	// our EGL manager
	EGLManager *eglManager;
	
	// our program object
	GLuint program;

public:
	// constructors / destructors
	OGLManager(EGLManager *eglManager);
	~OGLManager();
	
	// shader functions
	GLuint compileShader(GLenum type, const char *shaderSrc);
	GLuint compileProgram(GLuint vertexShader, GLuint fragmentShader);
	
	// access functions
	GLuint getProgram();
};

#endif