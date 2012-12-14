#include "textureManager.h"
#include "lodepng.h"

	unsigned int nextIndex;
	std::map<unsigned int, TextureData> textures; 

TextureManager::TextureManager() {
	nextIndex = 0;
}

TextureManager::~TextureManager() {
	// loop through all active textures and free them
	for(std::map<unsigned int, TextureData>::iterator it = textures.begin(); it != textures.end(); it++) {
		glDeleteTextures(1, &((*it).second.textureID));
	}
	
	// and delete our map
	textures.clear();
}

// load / unload textures
unsigned int TextureManager::loadTexture(const std::string &fileName) {
	// provide storage for our texture data
	TextureData textureData;
	
	// load our texture
	textureData.textureID = loadPNGTexture(fileName);
	
	// set our texture unit
	textureDatatextureUnit = GL_TEXTURE0 + nextIndex;
	
	// and store it!
	unsigned int index = nextIndex;
	textures[index] = textureData;
	nextIndex++;
	return index;
}

void TextureManager::unloadTexture(unsigned int index) {
	// make sure it exists first
	if(textures.find(index) == textures.end()) {
		// does not exist!
		return;
	}

	// free the texture memory
	glDeleteTextures(1, &(textures[index].textureID)));
	
	// and erase it from the map
	textures.erase(index);
}

// create a basic 2D texture for testing
GLuint TextureManager::createSimple2DTexture() {
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
GLuint TextureManager::loadPNGTexture(const std::string &filename) {
   // Texture object handle
   GLuint textureId;
   
   // load the png file
   unsigned error;
   unsigned char *image;
   unsigned width, height;
   
	error = lodepng_decode32_file(&image, &width, &height, filename.c_str());
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

// texture access
bool TextureManager::getTexture(unsigned int index, GLuint *textureID, GLenum *textureUnit) {
	// make sure it exists first
	if(textures.find(index) == textures.end()) {
		// does not exist!
		return false;
	}
	
	// if it exists, get it!
	*textureID = textures[index].textureID;
	*textureUnit = textures[index].textureUnit;
}