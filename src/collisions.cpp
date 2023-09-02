#include "collisions.h"
#include <iostream>

bool checkAABBCollision(GameObject* obj1, GameObject* obj2){
    bool x_axis_collision, y_axis_collision, z_axis_collision;
    glm::vec3 obj1_bbox_max = obj1->getBBoxMaxWorld();
    glm::vec3 obj1_bbox_min = obj1->getBBoxMinWorld();
    glm::vec3 obj2_bbox_max = obj2->getBBoxMaxWorld();
    glm::vec3 obj2_bbox_min = obj2->getBBoxMinWorld();    
    
    x_axis_collision = obj1_bbox_min.x <= obj2_bbox_max.x && obj1_bbox_max.x >= obj2_bbox_min.x;
    y_axis_collision = obj1_bbox_min.y <= obj2_bbox_max.y && obj1_bbox_max.y >= obj2_bbox_min.y;
    z_axis_collision = obj1_bbox_min.z <= obj2_bbox_max.z && obj1_bbox_max.z >= obj2_bbox_min.z;

    return x_axis_collision && y_axis_collision && z_axis_collision;    
}
