#ifndef _TUSK_H_
#define _TUSK_H_

class Tusk {
private:
	// our managers
	EGLManager eglManager;
	OGLManager oglManager;
	
	// keep track of our shaders
	GLuint vertexShader;
	GLuint fragmentShader;
	
	// keep track of our attribute and uniform locations
	GLint positionLoc;
	GLint texCoordLoc;
	GLint samplerLoc;
	GLint viewMatrixLoc;

public:
	// standard constructor / destructor
	Tusk();
	~Tusk();
	
	// our initialization function
	bool begin();
};

#endif