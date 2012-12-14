#ifndef _TEXTURE_MANAGER_H_
#define _TEXTURE_MANAGER_H_

#include <map>
#include <string>
#include "GLES2/gl2.h"

struct TextureData {
	GLuint textureID;
	GLenum textureUnit;
};

class TextureManager {
private:
	unsigned int nextIndex;
	std::map<unsigned int, TextureData> textures; 

public:
	TextureManager();
	~TextureManager();
	
	// load / unload textures
	unsigned int loadTexture(const std::string &fileName);
	void unloadTexture(unsigned int index);
	
	// texture loading functions
	GLuint createSimple2DTexture();
	GLuint loadPNGTexture(const std::string &filename);
	
	// texture access
	bool getTexture(unsigned int index, GLuint *textureID, GLenum *textureUnit);
};

#endif