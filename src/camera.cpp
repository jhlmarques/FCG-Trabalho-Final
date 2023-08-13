#include "camera.h"


Camera::Camera(float px, float py, float pz, float ux, float uy, float uz){
    position  = glm::vec4(px, py, pz, 1.0f);
    up        = glm::vec4(ux, uy, uz, 0.0f);
    
    distance = std::numeric_limits<float>::epsilon();
    lookat = false;
    lookAtPoint = position + view;
    // Calcula vetor e matriz view, além de outros;
    applyChanges();
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

// Define como o vetor view é calculado.
void Camera::setCameraMode(bool lookat){
    this->lookat = lookat;
}
// Define o ponto utilizado para calcular o vetor view no modo Look At
void Camera::setViewLookAtPoint(glm::vec4& point){
    lookAtPoint = point;
}

// Define a magnitude do vetor view fixo no modo Free
void Camera::setViewMagnitude(float distance){
    if(distance < std::numeric_limits<float>::epsilon()){
        distance = std::numeric_limits<float>::epsilon();
    }
    else{
        this->distance = distance;
    }
    
}

// Define as coordenadas polares usadas para o cálculo do vetor view fixo no modo Free
void Camera::setViewPolarCoords(float phi, float theta){
    this->phi = phi;
    this->theta = theta;
}

// Recalcula elementos da câmera
void Camera::applyChanges(){
    // Recalcula vetor view
    if(lookat){
        view = lookAtPoint - position;
    }
    else{
        view.x = distance*cos(phi)*sin(theta);
        view.y = distance*sin(phi);
        view.z = distance*cos(phi)*cos(theta);

    }
    
    // Recalcula matriz view
    viewMatrix =  Matrix_Camera_View(position, view, up);

    // Recalcula w e u
    w = -view / norm(view);
    u = crossproduct(up, w)/norm(crossproduct(up, w));

}

// Matriz view da câmera
glm::mat4 const& Camera::getViewMatrix(){    
    return viewMatrix;
}

// Vetor w
glm::vec4 Camera::getWVec(){
    return w;
}

// Vetor u
glm::vec4 Camera::getUVec(){
    return u;
}