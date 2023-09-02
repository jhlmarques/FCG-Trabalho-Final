#include "collisions.h"
#include <iostream>

bool checkAABBCollision(GameObject* obj1, glm::vec3 new_pos_obj1, GameObject* obj2, glm::vec3 new_pos_obj2){
    bool x_axis_collision, y_axis_collision, z_axis_collision;
    glm::vec3 obj1_bbox_max = obj1->getBBoxMax() + new_pos_obj1;
    glm::vec3 obj1_bbox_min = obj1->getBBoxMin() + new_pos_obj1;
    glm::vec3 obj2_bbox_max = obj2->getBBoxMax() + new_pos_obj2;
    glm::vec3 obj2_bbox_min = obj2->getBBoxMin() + new_pos_obj2; 
    
    std::cout << "obj1_bbox_max: " << obj1_bbox_max.x << " " << obj1_bbox_max.y << " " << obj1_bbox_max.z << std::endl;
    std::cout << "obj2_bbox_min: " << obj2_bbox_min.x << " " << obj2_bbox_min.y << " " << obj2_bbox_min.z << std::endl;
    std::cout << "obj2_bbox_max: " << obj2_bbox_max.x << " " << obj2_bbox_max.y << " " << obj2_bbox_max.z << std::endl;
    std::cout << "obj1_bbox_min: " << obj1_bbox_min.x << " " << obj1_bbox_min.y << " " << obj1_bbox_min.z << std::endl;
    
    x_axis_collision = obj1_bbox_min.x <= obj2_bbox_max.x && obj1_bbox_max.x >= obj2_bbox_min.x;
    y_axis_collision = obj1_bbox_min.y <= obj2_bbox_max.y && obj1_bbox_max.y >= obj2_bbox_min.y;
    z_axis_collision = obj1_bbox_min.z <= obj2_bbox_max.z && obj1_bbox_max.z >= obj2_bbox_min.z;

    return x_axis_collision && y_axis_collision && z_axis_collision;    
}
