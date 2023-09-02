#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include "gameobject.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Colisão entre AABBs. É passado os objetos e as posições que serão atualizadas deles
bool checkAABBCollision(GameObject* obj1, glm::vec3 new_pos_obj1, GameObject* obj2, glm::vec3 new_pos_obj2);

#endif