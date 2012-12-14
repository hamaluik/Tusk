#include "oglManager.h"
#include "lodepng.h"
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

// create a basic 2D texture for testing
GLuint OGLManager::createSimple2DTexture() {
   // Texture object handle
   GLuint textureId;
   
   // 2x2 Image, 3 bytes per pixel (R, G, B)
   GLubyte pixels[4 * 3] = {  
      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0  // Yellow
   };

   // Use tightly packed data
   glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

   // Generate a texture object
   glGenTextures (1, &textureId);

   // Bind the texture object
   glBindTexture (GL_TEXTURE_2D, textureId);

   // Load the texture
   glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

   // Set the filtering mode
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// set the wrap mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   return textureId;
}

// load a PNG texture
GLuint OGLManager::loadPNGTexture(const char *filename) {
   // Texture object handle
   GLuint textureId;
   
   // load the png file
   unsigned error;
   unsigned char *image;
   unsigned width, height;
   
	error = lodepng_decode32_file(&image, &width, &height, filename);
	if(error) {
		printf("lodepng error %u: %s\n", error, lodepng_error_text(error));
		return 0;
	}

   // Use tightly packed data
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   // Generate a texture object
   glGenTextures(1, &textureId);

   // Bind the texture object
   glBindTexture(GL_TEXTURE_2D, textureId);

   // Load the texture
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
   
   // and free the image
   free(image);

   // Set the filtering mode
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// set the wrap mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

   return textureId;
}

GLuint OGLManager::getProgram() {
	return program;
}