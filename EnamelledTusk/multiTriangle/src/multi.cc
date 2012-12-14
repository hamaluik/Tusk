// includes that we will need
/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>*/
#include <iostream>

// our own includes
#include "eglManager.h"
#include "oglManager.h"

// our 1st draw function!
void draw1(EGLManager *eglManager, OGLManager *oglManager, GLint positionLoc, GLint colourLoc) {
	// load up our vertices
	GLfloat vertices[] = {
		0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f,   0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f,   0.0f, 0.0f, 1.0f, 1.0f};
	
	// use the program object
	glUseProgram(oglManager->getProgram());
	
	// load the vertex data
	glEnableVertexAttribArray(positionLoc);
	glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), vertices);
	glEnableVertexAttribArray(colourLoc);
	glVertexAttribPointer(colourLoc, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), vertices + 2);
	
	// and draw with arrays!
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

// our 2nd draw function!
void draw2(EGLManager *eglManager, OGLManager *oglManager, GLint positionLoc, GLint colourLoc) {
	// load up our vertices
	GLfloat vertices[] = {
		0.0f, -1.0f,   1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,   1.0f, 1.0f, 1.0f, 0.5f,
		1.0f, 1.0f,   1.0f, 1.0f, 1.0f, 0.5f};
	
	// use the program object
	glUseProgram(oglManager->getProgram());
	
	// load the vertex data
	glEnableVertexAttribArray(positionLoc);
	glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), vertices);
	glEnableVertexAttribArray(colourLoc);
	glVertexAttribPointer(colourLoc, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), vertices + 2);
	
	// and draw with arrays!
	glDrawArrays(GL_TRIANGLES, 0, 3);
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	cout << " done!" << endl;
	
	// now compile and load our shaders
	cout << "Compiling shaders...";
	// create our shader sources inline here
	char vertexShaderSrc[] =
		"precision lowp float;\n"
		"attribute vec2 aVertexPosition;\n"
		"attribute vec4 aVertexColour;\n"
		"invariant varying vec4 vVertexColour;\n"
		"void main() {\n"
		"	vVertexColour = aVertexColour;\n"
		"	gl_Position = vec4(aVertexPosition, 0.0, 1.0);\n"
		"}\n";
	char fragmentShaderSrc[] =
		"precision lowp float;\n"
		"invariant varying vec4 vVertexColour;\n"
		"void main() {\n"
		"	gl_FragColor = vVertexColour;\n"
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
	GLint colourLoc = glGetAttribLocation(oglManager.getProgram(), "aVertexColour");
	cout << " done!" << endl;

	// our program loop
	cout << "Running main loop..." << endl;
	while(1) {
		// set the viewport
		glViewport(0, 0, eglManager.getScreenWidth(), eglManager.getScreenHeight());
		
		// clear the color buffer
		glClear(GL_COLOR_BUFFER_BIT);
		
		// draw!
		draw1(&eglManager, &oglManager, positionLoc, colourLoc);
		draw2(&eglManager, &oglManager, positionLoc, colourLoc);

		// swap our buffers
		eglManager.swapBuffers();
	}
	
	return 0;
}
