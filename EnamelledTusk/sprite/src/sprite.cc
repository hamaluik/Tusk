// includes that we will need
#include <iostream>
#include <cmath>
#include <sys/time.h>
#include "lodepng.h"

// our own includes
#include "eglManager.h"
#include "oglManager.h"

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
	GLuint numTilesWide = 60;
	GLuint numTilesHigh = 18;
	// map data generated using tIDE
	// (each number represents a tile index, starting at the top-left
	// and going left-to-right, top-to-bottom)
	GLubyte tileMap[] = {17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,5,17,17,17,17,7,17,17,17,6,17,17,17,17,17,17,17,17,17,17,7,17,17,6,17,17,7,17,17,17,17,17,17,17,17,3,34,37,0,17,17,17,17,17,17,17,5,17,17,17,17,17,17,17,17,17,17,5,17,17,17,17,17,17,17,17,17,17,17,17,17,17,7,17,17,6,17,17,17,17,17,17,17,17,17,17,17,17,17,6,17,17,7,17,3,34,39,0,20,17,17,17,6,17,17,5,17,17,17,17,17,17,17,17,17,17,3,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,33,34,37,0,20,17,17,5,17,134,135,135,135,135,136,17,6,17,7,17,17,17,3,34,37,37,37,37,37,38,37,37,37,41,37,37,37,37,38,37,37,37,37,37,37,37,41,37,37,39,37,37,37,37,38,37,37,38,37,37,37,37,0,20,17,17,17,17,17,150,151,151,151,151,137,135,136,5,17,17,17,17,18,37,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,20,17,8,17,17,17,17,150,151,151,151,118,151,151,152,5,17,17,17,17,18,40,16,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,6,17,17,17,17,17,17,17,17,5,17,17,6,17,17,17,5,17,17,6,17,17,17,17,17,17,166,122,151,151,151,153,151,152,5,17,17,17,17,18,37,16,17,17,17,17,17,96,97,97,97,97,97,97,97,97,97,97,98,17,17,17,17,17,134,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,17,17,17,5,166,167,122,151,151,121,168,17,17,17,17,17,18,37,16,17,17,17,17,8,112,113,17,17,17,17,5,17,17,17,5,114,17,7,17,5,17,150,153,151,151,151,118,151,151,151,151,151,151,151,151,151,151,118,151,151,17,17,17,6,17,17,166,167,167,168,17,17,17,17,17,3,34,37,16,17,17,17,17,6,128,129,129,129,129,129,129,68,17,17,17,114,17,17,17,17,17,150,151,169,151,151,151,151,151,151,151,151,151,118,151,151,151,151,151,151,17,17,17,17,17,17,17,17,5,17,17,17,17,17,3,34,40,0,20,17,17,17,17,17,160,147,161,161,161,161,162,112,17,17,17,114,17,17,134,135,135,138,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,17,17,17,17,17,17,5,17,17,17,17,17,17,3,34,37,0,20,17,17,17,17,5,17,17,17,17,17,17,17,17,112,17,5,17,114,17,5,150,151,151,151,151,151,151,151,151,151,151,169,151,151,151,151,151,151,151,169,151,151,17,17,17,7,17,17,17,17,17,17,17,7,3,34,39,0,20,17,17,17,6,17,17,5,17,17,17,17,17,17,17,112,17,17,17,114,17,17,150,118,151,151,151,151,151,118,151,151,151,151,151,151,151,151,151,151,151,151,151,151,17,17,17,17,17,3,33,33,33,33,33,33,34,37,0,20,17,17,7,17,48,49,49,49,50,17,17,17,17,17,17,112,17,113,17,114,17,17,150,151,151,151,151,151,151,151,151,151,151,151,151,151,121,122,151,151,151,151,151,151,17,17,17,17,3,34,37,41,37,37,37,38,37,0,20,17,17,17,17,17,64,65,22,65,66,17,17,8,17,17,17,128,129,129,129,130,5,17,150,151,151,151,151,151,151,151,151,151,151,151,151,151,137,138,151,151,151,151,151,151,17,17,17,3,34,37,0,1,1,1,1,1,1,20,17,17,17,17,17,17,64,22,65,35,82,7,17,17,17,17,17,160,161,147,161,162,17,134,138,151,151,169,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,118,151,151,17,17,17,18,37,0,20,17,17,17,17,17,17,17,17,17,17,17,17,17,80,81,81,82,17,17,17,17,17,17,17,17,17,17,17,6,17,150,151,151,151,151,151,151,169,151,151,151,151,151,169,151,151,151,151,151,151,151,151,151,17,17,5,18,37,16,17,17,17,17,17,6,17,8,17,17,17,17,7,17,17,17,17,17,17,17,17,17,17,17,6,17,17,17,17,17,134,138,151,151,169,151,151,151,151,151,118,151,151,151,151,151,151,151,151,151,151,151,151,151,17,17,5,18,40,16,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,7,17,17,17,7,17,17,17,134,138,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,169};
	cout << " done!" << endl;
	
	// load our vertex buffer
	cout << "Loading vertex buffer...";
	
	// get an id for our buffer
	GLuint vboId;
	glGenBuffers(1, &vboId);
	
	// bind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
		
	// construct a vertex array
	GLuint numTiles = numTilesWide * numTilesHigh;
	// drawing with GL_TRIANGLES (since we need separate vertices to work with
	// textures properly)
	GLuint numVertices = numTiles * 6;
	// initialize our vertices
	GLfloat *vertices = new GLfloat[numVertices * 4];
	for(GLuint yi = 0; yi < numTilesHigh; yi++) {
		for(GLuint xi = 0; xi < numTilesWide; xi++) {
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
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 0] = (GLfloat)xi;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 1] = (GLfloat)yi;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 2] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] % 16) / 16.0;//0.375f;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 3] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] / 16 + 1) / 16.0;//0.0625f;
			
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 4] = (GLfloat)xi + 1.0;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 5] = (GLfloat)yi;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 6] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] % 16 + 1) / 16.0;//0.4375f;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 7] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] / 16 + 1) / 16.0;//0.0625f;
			
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) +  8] = (GLfloat)xi;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) +  9] = (GLfloat)yi + 1.0;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 10] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] % 16) / 16.0;//0.375f;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 11] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] / 16) / 16.0;//0.0f;
			
			/// Our second triangle
			// (vertex numbers & locations)
			// 5----4
			//  \   |
			//   \  |
			//    \ |
			//     \|
			//      3
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 12] = (GLfloat)xi + 1.0;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 13] = (GLfloat)yi;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 14] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] % 16 + 1) / 16.0;//0.4375f;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 15] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] / 16 + 1) / 16.0;//0.0625f;
			
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 16] = (GLfloat)xi + 1.0;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 17] = (GLfloat)yi + 1.0;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 18] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] % 16 + 1) / 16.0;//0.4375f;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 19] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] / 16) / 16.0;//0.0f;
			
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 20] = (GLfloat)xi;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 21] = (GLfloat)yi + 1.0;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 22] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] % 16) / 16.0;//0.375f;
			vertices[(((numTilesWide * yi) + xi) * 4 * 6) + 23] = (GLfloat)(tileMap[(numTilesWide * (numTilesHigh - yi - 1)) + xi] / 16) / 16.0;//0.0f;
		}
	}
		
	// and fill in the buffer
	glBufferData(GL_ARRAY_BUFFER, numVertices * 4 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	
	// and clean up our vertices
	delete[] vertices;
	
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
	
	// load our sprite
	cout << "Loading sprite...";
	GLuint spriteTexture = oglManager.loadPNGTexture("resources/smallMario.png");
	cout << " done!" << endl;
	
	// data for calculating timing / FPS
	struct timeval t1, t2;
	struct timezone tz;
	float deltatime;
	float totaltime = 0.0f;
	unsigned int frames = 0;
	gettimeofday(&t1 , &tz);

	// our program loop
	cout << "Running main loop..." << endl;
	float t = 0.0f;
	while(1) {
		// get timing information
		gettimeofday(&t2, &tz);
		deltatime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6);
		t1 = t2;
		
		// exit after 10 seconds
		/*if(t >= 10.0f) {
			break;
		}*/
		
		// update the camera
		// (camera X is in viewMatrix[12] -- we SHOULD be doing some matrix math
		// here to keep things sane, but for this demo, just oscillate this value
		// between -1 and -3)
		t += deltatime;
		//viewMatrix[12] = sin(t/4.0f) - 2.0f;
		//viewMatrix[12] = sin(t) - 2.0f;
		//viewMatrix[13] = cos(t) - 2.0f;
			
		// set the viewport
		glViewport(0, 0, eglManager.getScreenWidth(), eglManager.getScreenHeight());
		
		// clear the color buffer
		glClear(GL_COLOR_BUFFER_BIT);
		
		// use the program object
		glUseProgram(oglManager.getProgram());
		
		// load the vertex data
		// first, our vertex positions
		glEnableVertexAttribArray(positionLoc);
		GLuint offset = 0;
		glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE,4 * sizeof(float), (const void*)offset);
		// now, the texture coordinates of each of those vertices
		glEnableVertexAttribArray(texCoordLoc);
		offset += 2 * sizeof(GLfloat);
		glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void*)offset);
		
		// set the view matrix (change camera location, etc)
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, (const GLfloat*)&viewMatrix);
		
		// bind the texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		// set the sampler texture unit to 0
		glUniform1i(samplerLoc, 0);
		
		// and draw with arrays!
		glDrawArrays(GL_TRIANGLES, 0, numVertices);
		
		// bind the texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, spriteTexture);

		// set the sampler texture unit to 0
		glUniform1i(samplerLoc, 1);
		
		// and draw with arrays!
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// swap our buffers
		eglManager.swapBuffers();

		// update our time calculations
		totaltime += deltatime;
		frames++;
		if (totaltime >  2.0f) {
			// print and calculate FPS
			//printf("\r%4d frames rendered in %1.4f seconds -> FPS=%3.4f", frames, totaltime, frames/totaltime);
			cout << "\r" << "FPS = " << (frames / totaltime) << " (frame render time = " << (deltatime*1000) << "ms)       " << flush;
			totaltime -= 2.0f;
			frames = 0;
		}
	}
	cout << endl;
	
	// clean up
	glDeleteBuffers(1, &vboId);
	
	return 0;
}
