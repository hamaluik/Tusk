#include "mapLayer.h"

// constructors and destructors
MapLayer::MapLayer(TextureManager *textureManager, unsigned int textureIndex) {
	this->textureManager = textureManager;
	this->tileSheet = tileSheet;
	
	// get our texture data
	textureManager->getTexture(textureIndex, &textureID, &textureUnit);
}

~MapLayer() {
}
	
// for loading!
bool MapLayer::loadMap(const std::string &fileName) {
	// todo: load maps from file
}

void MapLayer::update(GLfloat deltaTime) {
	// todo: implement animated tiles
}

void MapLayer::draw() {
	
}