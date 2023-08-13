#include "camera.h"

Camera mainCamera(0.0f, 0.0f, -5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

Camera::Camera(float px, float py, float pz, float vx, float vy, float vz, float ux, float uy, float uz){
    position  = glm::vec4(px, py, pz, 1.0f);
    view      = glm::vec4(vx, vy, vz, 0.0f);
    up        = glm::vec4(ux, uy, uz, 0.0f);
    
    lookAtPoint = position + view;
}

// Define a posição da câmera
void Camera::setPosition(float x, float y, float z){
    position.x = x;
    position.y = y;
    position.z = z;
}

// Retorna posição da câmera
glm::vec4 Camera::getPosition(){
    return position;
}

// Recalcula vetor view com a posição da câmera e um ponto
void Camera::lookAt(glm::vec4& point){
    lookAtPoint = position;
    view = lookAtPoint - position;
    
    // Recalcula w e u
    w = -view / norm(view);
    u = crossproduct(up, w)/norm(crossproduct(up, w));

}

// Ajusta o vetor view usando coordenadas esféricas
void Camera::setViewVector(float phi, float theta, float r){
    
    if(r < std::numeric_limits<float>::epsilon()){
        r = std::numeric_limits<float>::epsilon();
    }
    
    view.x = r*cos(phi)*sin(theta);
    view.y = r*sin(phi);
    view.z = r*cos(phi)*cos(theta);

    // Recalcula ponto fixo
    lookAtPoint = position + view;

    // Recalcula w e u
    w = -view / norm(view);
    u = crossproduct(up, w)/norm(crossproduct(up, w));
    

}

glm::vec4 const& Camera::getLookAtPoint(){
    return lookAtPoint;
}

// Matriz view da câmera
glm::mat4 Camera::getViewMatrix(){    
    return Matrix_Camera_View(position, view, up);
}

// Vetor w
glm::vec4 Camera::getWVec(){
    return -view / norm(view);
}

// Vetor u
glm::vec4 Camera::getUVec(){
    return crossproduct(up, w)/norm(crossproduct(up, w));
}