// includes that we will need
#include <iostream>
#include <cmath>
#include <sys/time.h>
#include "lodepng.h"

// our own includes
#include "eglManager.h"
#include "oglManager.h"

GLuint bufferVBO(GLuint vboId, GLuint numTilesWide, GLuint numTilesHigh, GLshort *tileMap) {
	// bind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	
	// count how many non-null vertices we have
	GLuint numTileSlots = numTilesWide * numTilesHigh;
	GLuint numTiles = 0;
	for(GLuint i = 0; i < numTileSlots; i++) {
		if(*(tileMap + i) >= 0) numTiles++;
	}
	
	// construct a vertex array
	// drawing with GL_TRIANGLES (since we need separate vertices to work with
	// textures properly)
	GLuint numVertices = numTiles * 6;
	// initialize our vertices
	GLfloat *vertices = new GLfloat[numVertices * 4];
	GLuint vertexNumber = 0;
	for(GLuint yi = 0; yi < numTilesHigh; yi++) {
		for(GLuint xi = 0; xi < numTilesWide; xi++) {
			// make sure we have a valid tile here
			if(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] == -1) {
				continue;
			}
		
			// we're essentially drawing quads, but OpenGL ES doesn't have quads, so we fake
			// it using triangles
		
			/// Our first triangle
			// (vertex numbers & locations)
			// 2    
			// |\
			// | \
			// |  \
			// |   \
			// 0----1
			vertices[vertexNumber++] = (GLfloat)xi;
			vertices[vertexNumber++] = (GLfloat)yi;
			vertices[vertexNumber++] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] % 16) / 16.0;//0.375f;
			vertices[vertexNumber++] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] / 16 + 1) / 16.0;//0.0625f;
			
			vertices[vertexNumber++] = (GLfloat)xi + 1.0;
			vertices[vertexNumber++] = (GLfloat)yi;
			vertices[vertexNumber++] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] % 16 + 1) / 16.0;//0.4375f;
			vertices[vertexNumber++] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] / 16 + 1) / 16.0;//0.0625f;
			
			vertices[vertexNumber++] = (GLfloat)xi;
			vertices[vertexNumber++] = (GLfloat)yi + 1.0;
			vertices[vertexNumber++] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] % 16) / 16.0;//0.375f;
			vertices[vertexNumber++] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] / 16) / 16.0;//0.0f;
			
			/// Our second triangle
			// (vertex numbers & locations)
			// 5----4
			//  \   |
			//   \  |
			//    \ |
			//     \|
			//      3
			vertices[vertexNumber++] = (GLfloat)xi + 1.0;
			vertices[vertexNumber++] = (GLfloat)yi;
			vertices[vertexNumber++] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] % 16 + 1) / 16.0;//0.4375f;
			vertices[vertexNumber++] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] / 16 + 1) / 16.0;//0.0625f;
			
			vertices[vertexNumber++] = (GLfloat)xi + 1.0;
			vertices[vertexNumber++] = (GLfloat)yi + 1.0;
			vertices[vertexNumber++] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] % 16 + 1) / 16.0;//0.4375f;
			vertices[vertexNumber++] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] / 16) / 16.0;//0.0f;
			
			vertices[vertexNumber++] = (GLfloat)xi;
			vertices[vertexNumber++] = (GLfloat)yi + 1.0;
			vertices[vertexNumber++] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] % 16) / 16.0;//0.375f;
			vertices[vertexNumber++] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] / 16) / 16.0;//0.0f;
		}
	}
		
	// and fill in the buffer
	glBufferData(GL_ARRAY_BUFFER, numVertices * 4 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	
	delete[] vertices;
	return numVertices;
}

void drawLayer(GLint positionLoc, GLint texCoordLoc, GLint samplerLoc, GLuint texture, GLint viewMatrixLoc, GLfloat *viewMatrix, GLuint vboId, GLuint numVertices) {
	// bind our buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	
	// load the vertex data
	// first, our vertex positions
	glEnableVertexAttribArray(positionLoc);
	glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE,4 * sizeof(float), (const void*)0);
	// now, the texture coordinates of each of those vertices
	glEnableVertexAttribArray(texCoordLoc);
	glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void*)(2 * sizeof(GLfloat)));
	
	// bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// set the sampler texture unit to 0
	glUniform1i(samplerLoc, 0);
	
	// set the view matrix (change camera location, etc)
	glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, (const GLfloat*)viewMatrix);
	
	// and draw with arrays!
	
	// draw our background
	glDrawArrays(GL_TRIANGLES, 0, numVertices);
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
	// very basic shader - take vertex position and texture coordinate
	// and transform the vertex position according to our view matrix
	// then pass the texture coordinate on to the fragment shader
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
	// very basic texture lookup shader
	// just get what texture location we're looking at for this fragment (pixel)
	// and get the colour from the texture at that location
	char fragmentShaderSrc[] =
		"precision mediump float;\n"
		"invariant varying vec2 vTexturePosition;\n"
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
	GLint viewMatrixLoc = glGetUniformLocation(oglManager.getProgram(), "uViewMatrix");
	cout << " done!" << endl;
	
	// load our texture
	cout << "Loading textures...";
	GLuint texture = oglManager.loadPNGTexture("resources/tiles.png");
	cout << " done!" << endl;
	
	// load our map
	cout << "Loading map...";
	// TODO: load from file
	GLuint numTilesWide = 30;
	GLuint numTilesHigh = 17;
	// map data generated using tIDE
	// (each number represents a tile index, starting at the top-left
	// and going left-to-right, top-to-bottom)
	GLshort tileMapBack[] = {37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,38,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,39,37,37,37,37,37,37,37,37,37,39,37,37,37,37,37,37,37,37,37,37,38,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,39,37,37,37,37,37,37,37,37,37,38,37,37,37,37,37,37,37,37,37,37,37,37,37,39,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,40,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,37,40,37,0,1,1,1,2,37,37,40,37,37,37,37,37,37,37,37,37,40,37,38,37,37,37,37,37,37,37,37,37,0,20,17,17,17,19,1,1,2,37,37,37,37,37,40,0,1,1,1,37,37,37,37,37,37,37,40,0,1,1,20,17,17,17,17,17,17,17,19,2,37,40,37,0,1,20,17,17,17,37,40,37,37,0,1,1,1,20,17,17,17,17,17,17,17,17,17,17,17,19,1,1,1,20,17,17,17,17,5,1,1,1,1,20,17,17,17,17,17,17,17,5,17,5,17,17,17,5,17,17,17,17,17,17,17,17,8,17,17,17,5,17,17,17,17,17,17,17,17,17,17,17,5,17,17,17,17,17,17,17,17,17,17,8,17,17,17,17,17,17,17,17,17,17,17,17,7,17,17,5,17,17,17,17,17,17,17,17,17,17,17,17,17,134,135,135,135,135,135,17,17,7,17,6,17,17,17,17,17,17,17,17,17,17,5,17,5,17,17,17,8,17,17,150,151,151,151,151,151,17,17,17,17,17,17,17,17,6,17,17,17,17,17,17,17,5,17,17,17,134,135,135,135,138,151,151,151,151,151,5,17,17,17,17,7,17,17,17,17,17,7,17,17,17,17,17,17,134,135,138,151,151,151,151,151,151,169,151,151,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,134,135,135,138,151,151,151,151,151,151,151,169,151,169,151,17,17,17,17,17,5,17,17,17,17,17,17,17,17,134,138,151,151,151,151,151,151,151,151,151,151,151,169,151,151};
	GLshort tileMapGround[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,25,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,25,-1,-1,-1,-1,-1,-1,25,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,25,-1,-1,-1,-1,-1,-1,-1,-1,-1,25,-1,-1,-1,-1,-1,25,-1,-1,-1,-1,-1,-1,-1,25,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,25,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,25,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,25,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,42,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,42,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,170,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	GLshort tileMapFore[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,9,-1,-1,-1,-1,-1,-1,9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,9,-1,-1,-1,-1,-1,-1,-1,-1,-1,9,-1,-1,-1,-1,-1,9,-1,-1,-1,-1,-1,-1,-1,9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,9,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,26,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,26,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,154,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	cout << " done!" << endl;
	
	// load our vertex buffer
	cout << "Loading vertex buffer..." << flush;
	
	// get an id for our buffer
	GLuint vboIds[3];
	glGenBuffers(3, (GLuint*)&vboIds);
	
	// load up all of our vertex buffer objects
	GLuint numVerticesBack = bufferVBO(vboIds[0], numTilesWide, numTilesHigh, (GLshort*)&tileMapBack);
	GLuint numVerticesGround = bufferVBO(vboIds[1], numTilesWide, numTilesHigh, (GLshort*)&tileMapGround);
	GLuint numVerticesFore = bufferVBO(vboIds[2], numTilesWide, numTilesHigh, (GLshort*)&tileMapFore);
	
	cout << " done!" << endl;
	
	// load our view matrix
	cout << "Loading camera...";
	// we're using square tiles at 16px / tile
	// scale that up 4x so each tile draws at 64x64 px on the screen
	const GLfloat tileSize = 16, tileScale = 4;
	// pre-calculate a view matrix for our projection
	GLfloat viewMatrix[] = {
		(2*tileSize*tileScale)/eglManager.getScreenWidth(), 0, 0, 0,
		0, (2*tileSize*tileScale)/eglManager.getScreenHeight(), 0, 0,
		0, 0, 1, 0,
		-1, -1, 0, 1};
	cout << " done!" << endl;
	
	// data for calculating timing / FPS
	struct timeval t1, t2, tr1, tr2;
	struct timezone tz;
	float deltaTime, renderTime;
	float totaltime = 0.0f;
	unsigned int frames = 0;
	gettimeofday(&t1 , &tz);

	// our program loop
	cout << "Running main loop..." << endl;
	float t = 0.0f;
	while(1) {
		// get timing information
		gettimeofday(&t2, &tz);
		deltaTime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6);
		t1 = t2;
		
		// exit after 10 seconds
		t += deltaTime;
		/*if(t >= 10.0f) {
			break;
		}*/
			
		// count how long we spend actually rendering
		gettimeofday(&tr1 , &tz);
			
		// set the viewport
		glViewport(0, 0, eglManager.getScreenWidth(), eglManager.getScreenHeight());
		
		// clear the color buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// use the program object
		glUseProgram(oglManager.getProgram());
		
		// draw the background
		drawLayer(positionLoc, texCoordLoc, samplerLoc, texture, viewMatrixLoc, &viewMatrix[0], vboIds[0], numVerticesBack);
		drawLayer(positionLoc, texCoordLoc, samplerLoc, texture, viewMatrixLoc, &viewMatrix[0], vboIds[1], numVerticesGround);
		drawLayer(positionLoc, texCoordLoc, samplerLoc, texture, viewMatrixLoc, &viewMatrix[0], vboIds[2], numVerticesFore);
		
		gettimeofday(&tr2, &tz);
		renderTime = (float)(tr2.tv_sec - tr1.tv_sec + (tr2.tv_usec - tr1.tv_usec) * 1e-6);	

		// swap our buffers
		eglManager.swapBuffers();

		// update our time calculations
		totaltime += deltaTime;
		frames++;
		if (totaltime >  2.0f) {
			// print and calculate FPS
			//printf("\r%4d frames rendered in %1.4f seconds -> FPS=%3.4f", frames, totaltime, frames/totaltime);
			cout << "\r" << "FPS = " << (frames / totaltime) << " (frame render time = " << (renderTime*1000) << "ms)       " << flush;
			totaltime -= 2.0f;
			frames = 0;
		}
	}
	cout << endl;
	
	// clean up
	glDeleteBuffers(3, (GLuint*)&vboIds);
	
	return 0;
}
