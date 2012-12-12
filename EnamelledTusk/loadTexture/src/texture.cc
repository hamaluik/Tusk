// includes that we will need
#include <iostream>
#include "lodepng.h"

// our own includes
#include "eglManager.h"
#include "oglManager.h"

// our draw function!
void draw(EGLManager *eglManager, OGLManager *oglManager, GLint positionLoc, GLint texCoordLoc, GLuint texture, GLint samplerLoc) {
	// load up our vertices
	GLfloat vertices[] = {
	//	  x      y       s     t
		-1.0f, -1.0f,   0.0f, 16.875f,
		 1.0f, -1.0f,   30.0f, 16.875f,
		-1.0f,  1.0f,   0.0f, 0.0f,
		 1.0f,  1.0f,   30.0f, 0.0f};
		
	// set the viewport
	glViewport(0, 0, eglManager->getScreenWidth(), eglManager->getScreenHeight());
	
	// clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	
	// use the program object
	glUseProgram(oglManager->getProgram());
	
	// load the vertex data
	glEnableVertexAttribArray(positionLoc);
	glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE,4 * sizeof(float), vertices);
	glEnableVertexAttribArray(texCoordLoc);
	glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), vertices + 2);
	
	// bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// set the sampler texture unit to 0
	glUniform1i(samplerLoc, 0);
	
	// and draw with arrays!
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

int main() {
	using namespace std;

	// initialize things
	cout << "Initializing EGL...";
	EGLManager eglManager;
	eglManager.initialize();
	cout << " done!" << endl;
	cout << "Initializing OpenGL...";
	OGLManager oglManager(&eglManager);
	cout << " done!" << endl;
	
	// now compile and load our shaders
	cout << "Compiling shaders...";
	// create our shader sources inline here
	char vertexShaderSrc[] =
		"precision lowp float;\n"
		"attribute vec2 aVertexPosition;\n"
		"attribute vec2 aTexturePosition;\n"
		"varying vec2 vTexturePosition;\n"
		"void main() {\n"
		"	vTexturePosition = aTexturePosition;\n"
		"	gl_Position = vec4(aVertexPosition, 0.0, 1.0);\n"
		"}\n";
	char fragmentShaderSrc[] =
		"precision lowp float;\n"
		"varying vec2 vTexturePosition;\n"
		"uniform sampler2D sTexture;\n"
		"void main() {\n"
		"	gl_FragColor = texture2D(sTexture, vTexturePosition);\n"
		"}\n";
		
	// compile our vertex and fragment shaders
	GLuint vertexShader = oglManager.compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
	GLuint fragmentShader = oglManager.compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
	
	// compile our program
	GLuint result = oglManager.compileProgram(vertexShader, fragmentShader);
	if(result == GL_FALSE) {
		cout << " failed!" << endl;
		return 1;
	}
	
	// get the locations of our attributes
	GLint positionLoc = glGetAttribLocation(oglManager.getProgram(), "aVertexPosition");
	GLint texCoordLoc = glGetAttribLocation(oglManager.getProgram(), "aTexturePosition");
	GLint samplerLoc = glGetUniformLocation(oglManager.getProgram(), "uTexture");
	cout << " done!" << endl;
	
	// load our texture
	cout << "Loading textures...";
	GLuint texture = oglManager.loadPNGTexture("resources/brick.png");
	cout << " done!" << endl;

	// our program loop
	cout << "Running main loop..." << endl;
	while(1) {
		// TODO: implement update function
		
		// draw!
		draw(&eglManager, &oglManager, positionLoc, texCoordLoc, texture, samplerLoc);

		// swap our buffers
		eglManager.swapBuffers();
	}
	
	return 0;
}
