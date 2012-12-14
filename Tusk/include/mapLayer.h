#ifndef _MAP_LAYER_H_
#define _MAP_LAYER_H_

#include <string>
#include "entity.h"
#include "textureManager.h"

class MapLayer: public Entity {
private:
	TextureManager *textureManager;

	GLuint mapWidth, mapHeight;
	
	unsigned int tileSheet;
	GLuint textureID;
	GLenum textureUnit;

public:
	// constructors and destructors
	MapLayer(TextureManager *textureManager, unsigned int textureIndex);
	~MapLayer();
	
	// for loading!
	bool loadMap(const std::string &fileName);
	
	// update and draw functions
	void update(GLfloat deltaTime);
	void draw();
};

#endif