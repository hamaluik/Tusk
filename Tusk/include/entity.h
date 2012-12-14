#ifndef _ENTITY_H_
#define _ENTITY_H_

class Entity {
private:
	GLfloat x, y;
	GLfloat width, height;

public:
	// update and draw functions to be
	// overwritten by inheritance
	virtual void update(GLfloat deltaTime) {};
	virtual void draw() {};
};

#endif