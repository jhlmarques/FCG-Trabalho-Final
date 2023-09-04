#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include "gameobject.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Colisão entre planos utilizando AABBs
bool checkPlaneToPlaneCollision(GameObject* obj1, GameObject* obj2);
// Colisão entre cubos utilizando AABBs
bool checkCubeToCubeCollision(GameObject* obj1, GameObject* obj2);
// Colisão entre o mouse e um plano definido com coordenadas de tela
bool checkPointToPlaneCollision(glm::vec2 point, glm::vec2 plane_bbox_min, glm::vec2 plane_bbox_max);

#endif