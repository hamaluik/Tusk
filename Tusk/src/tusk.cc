#include "tusk.h"
#include "eglManager.h"
#include "oglManager.h"

// standard constructor / destructor
Tusk::Tusk() {

}

Tusk::~Tusk() {

}

// our initialization function
bool Tusk::begin() {
	// start our EGL manager
	eglManager.initialize();
	
	// and now our OGL manager
	oglManager.setEGLManager(&eglManager);
	
	// set up our OpenGL states
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	
	// create our shader sources inline here
	char vertexShaderSrc[] =
		"precision mediump float;\n"
		"uniform mat4 uViewMatrix;\n"
		"attribute vec2 aVertexPosition;\n"
		"attribute vec2 aTexturePosition;\n"
		"invariant varying vec2 vTexturePosition;\n"
		"void main() {\n"
		"	vTexturePosition = aTexturePosition;\n"
		"	gl_Position = uViewMatrix * vec4(aVertexPosition, 0.0, 1.0);\n"
		"}\n";
	char fragmentShaderSrc[] =
		"precision mediump float;\n"
		"invariant varying vec2 vTexturePosition;\n"
		"uniform sampler2D sTexture;\n"
		"void main() {\n"
		"	gl_FragColor = texture2D(sTexture, vTexturePosition);\n"
		"}\n";
		
	// compile our vertex and fragment shaders
	vertexShader = oglManager.compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
	fragmentShader = oglManager.compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);

	// compile our program
	GLuint result = oglManager.compileProgram(vertexShader, fragmentShader);
	if(result == GL_FALSE) {
		return false;
	}

	// get the locations of our attributes
	positionLoc = glGetAttribLocation(oglManager.getProgram(), "aVertexPosition");
	texCoordLoc = glGetAttribLocation(oglManager.getProgram(), "aTexturePosition");
	samplerLoc = glGetUniformLocation(oglManager.getProgram(), "uTexture");
	viewMatrixLoc = glGetUniformLocation(oglManager.getProgram(), "uViewMatrix");
}