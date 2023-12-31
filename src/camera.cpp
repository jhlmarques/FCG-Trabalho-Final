#include "camera.h"

/* 
    TO-DO: diferenciar a movimentação da câmera movendo o mouse alterando o vetor de view da câmera free da posição da câmera lookat.
    Cada uma delas necessita de um conjunto (theta, phi, r) separado. Por enquanto não utilizamos esse tipo de movimentação na câmera livre,
    mas se quisermos implementar precisaremos diferenciar.
*/
Camera::Camera(glm::vec3 cameraStartingPosition) : 
    up(CAMERA_DEFAULT_UP_VECTOR), view(CAMERA_DEFAULT_VIEW_VECTOR)
{
    position  = glm::vec4(cameraStartingPosition.x, cameraStartingPosition.y, cameraStartingPosition.z, 1.0f);
    
    w = -view / norm(view);
    u = crossproduct(up, w)/norm(crossproduct(up, w));
    v = crossproduct(w, u);
}

void Camera::setPositionFree(glm::vec4 position){
    this->position = position;
}

void Camera::setViewVectorFree(glm::vec4 view){
    this->view = view;
    // Recalcula u, v e w
    w = -view / norm(view);
    u = crossproduct(up, w)/norm(crossproduct(up, w));
    v = crossproduct(w, u);
}

glm::vec4 Camera::getPosition(){
    return position;
}

void Camera::setLookAtPoint(glm::vec4 point){
    lookAtPoint = point;
    // Converte a posição para coordenadas esféricas, para que possamos alterar cada uma delas
    this->r = sqrt(pow(position.x, 2) + pow(position.y, 2) + pow(position.z, 2));
    this->theta = atan2(position.x, position.z);
    this->phi = atan2(position.y, sqrt(pow(position.x, 2) + pow(position.z, 2)));
    view = lookAtPoint - position;
    // Recalcula u, v e w
    w = -view / norm(view);
    u = crossproduct(up, w)/norm(crossproduct(up, w));
    v = crossproduct(w, u);

}

void Camera::updateViewVecLookAt(){
    position.x = r*cos(phi)*sin(theta);
    position.y = r*sin(phi);
    position.z = r*cos(phi)*cos(theta);

    view = lookAtPoint - position;

    // Recalcula u, v e w
    w = -view / norm(view);
    u = crossproduct(up, w)/norm(crossproduct(up, w));
    v = crossproduct(w, u);
}

glm::vec4 const& Camera::getLookAtPoint(){
    return lookAtPoint;
}

void Camera::setCameraTheta(float theta){
    this->theta = theta;
}

float Camera::getCameraTheta(){
    return theta;
}

void Camera::setCameraPhi(float phi){    
    // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
    float phimax = 3.141592f/2; // Por algum motivo não funciona com M_PI_2
    float phimin = -phimax;

    if (phi > phimax)
        this->phi = phimax;
    else if (phi < phimin)
        this->phi = phimin;
    else{
        this->phi = phi;    
    }
}

float Camera::getCameraPhi(){
    return phi;
}

void Camera::setCameraDistance(float r){
    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela está olhando, pois isto gera problemas de divisão por zero na
    // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
    // nunca pode ser zero.
    const float epsilon = std::numeric_limits<float>::epsilon();
    if (r < epsilon){
        this->r = epsilon;
    }
    else{
        this->r = r;
    }
}

float Camera::getCameraDistance(){
    return r;
}

glm::mat4 Camera::getViewMatrix(){    
    return Matrix_Camera_View(position, view, up);
}

glm::vec4 Camera::getViewVec(){
    return view;
}

glm::vec4 Camera::getUVec(){
    return u;
}

glm::vec4 Camera::getVVec(){
    return v;
}

glm::vec4 Camera::getWVec(){
    return w;
}