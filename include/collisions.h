#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include "gameobject.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Colisão entre AABBs
bool checkAABBCollision(GameObject* obj1, GameObject* obj2);

#endif