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
	
	// Shader programs
	GLuint programObject;
} STATE_STRUCT;

// and create our state and a pointer to it
static STATE_STRUCT _state, *state = &_state;

// our initialization function
static void initializeEGL(STATE_STRUCT *state) {
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

// create a shader object, load it's source, and compile the shader
GLuint loadShader(GLenum type, const char *shaderSrc) {
	// create the shader object
	GLuint shader = glCreateShader(type);
	
	// make sure it's valid
	if(shader == 0) {
		return 0;
	}
	
	// load the shader source
	glShaderSource(shader, 1, &sgaderSrc, NULL);
	
	// compile the shader
	glCompileShader(shader);
	
	// check the compile status
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	
	// check for compilation errors
	if(!compiled) {
		// store how long our info string is
		GLint infoLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);
		
		// print out the info log
		if(infoLen > 1) {
			// allocate memory
			char *infoLog = malloc(sizeof(char) * infoLength);
			
			// load up the log
			glGetshaderInfoLog(shader, infoLength, NULL, infoLog);
			printf("Error compiling shader:\n%s\n", infoLog);
			
			// free up our memory
			free(infoLog);
		}
		
		// clean up
		glDeleteShader(shader);
		return 0;
	}
	
	// return our compiled shader!
	return shader;
}

int initializeShaders(STATE_STRUCT *state) {
	// create our shader sources inline here
	GLbyte vertexShaderSrc[] =
		"attribute vec4 vertexPosition;\n"
		"void main() {\n"
		"	gl_Position = vertexPosition;\n"
		"}\n";
	GLbyte fragmentShaderSrc[] =
		"precision mediump float;\n"
		"void main() {\n"
		"	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
		"}\n";
		
	// load our vertex and fragment shaders
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSrc);
	GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
	
	// create a program object to link the two together with
	GLuint programObject = glCreateProgram();
	if(programObject == 0) {
		return 0;
	}
	
	// attach our two shaders to the program
	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);
	
	// bind our vertex shader attributes
	// (we only have the vertex position here)
	// (bind it to location 0)
	glBindAttribLocation(programObject, 0, "vertexPosition");
	
	// link the program up
	glLinkProgram(programObject);
	
	// and check the link status to make sure it linked ok
	GLint linked;
	glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
	
	// check for linker errors
	if(!linked) {
		// get the size of our info buffer
		GLint infoLength = 0;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLength);
		
		// print out the info message
		if(infoLength > 1) {
			// allocate memory for it
			char *infoLog = malloc(sizeof(char) * infoLength);
			
			// actually get the log
			glGetProgramInfoLog(programObject, infoLength, NULL, infoLog);
			printf("Error linking program:\n%s\n", infoLog);
			
			// and free our log memory
			free(infolog);
		}
		
		// clean up
		glDeleteProgram(programObject);
		return GL_FALSE;
	}
	
	// now store our program object
	state->programObject = programObject;
	
	// and set our clear colour
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	return GL_TRUE;
}

int main() {
	// initialize bcm
	bcm_host_init();
	
	// clear the application state
	memset(state, 0, sizeof(*state));
	
	// initialize things
	initializeEGL(state);
	initializeShaders(state);
	
	// our program loop
	while(1) {
	}
	
	// handle exiting
	onExit();
	return 0;
}