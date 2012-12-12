#include "oglManager.h"
#include <stdio.h>

OGLManager::OGLManager(EGLManager *eglManager) {
	this->eglManager = eglManager;
}

OGLManager::~OGLManager() {
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT);
	eglManager->swapBuffers();
}

// create a shader object, load it's source, and compile the shader
GLuint OGLManager::compileShader(GLenum type, const char *shaderSrc) {
	// create the shader object
	GLuint shader = glCreateShader(type);
	
	// make sure it's valid
	if(shader == 0) {
		return 0;
	}
	
	// load the shader source
	glShaderSource(shader, 1, &shaderSrc, NULL);
	
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
		if(infoLength > 1) {
			// allocate memory
			char *infoLog = (char*)malloc(sizeof(char) * infoLength);
			
			// load up the log
			glGetShaderInfoLog(shader, infoLength, NULL, infoLog);
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

GLuint OGLManager::compileProgram(GLuint vertexShader, GLuint fragmentShader) {	
	// create a program object to link the two together with
	GLuint programObject = glCreateProgram();
	if(programObject == 0) {
		return 0;
	}
	
	// attach our two shaders to the program
	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);
	
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
		
		// TODO: implement proper logging
		
		// print out the info message
		if(infoLength > 1) {
			// allocate memory for it
			char *infoLog = (char*)malloc(sizeof(char) * infoLength);
			
			// actually get the log
			glGetProgramInfoLog(programObject, infoLength, NULL, infoLog);
			printf("Error linking program:\n%s\n", infoLog);
			
			// and free our log memory
			free(infoLog);
		}
		
		// clean up
		glDeleteProgram(programObject);
		return GL_FALSE;
	}
	
	// store our program object
	this->program = programObject;
	
	return GL_TRUE;
}

GLuint OGLManager::getProgram() {
	return program;
}