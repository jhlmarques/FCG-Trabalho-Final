#include "camera.h"

Camera mainCamera(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

Camera::Camera(float px, float py, float pz, float vx, float vy, float vz, float ux, float uy, float uz){
    position  = glm::vec4(px, py, pz, 1.0f);
    view      = glm::vec4(vx, vy, vz, 0.0f);
    up        = glm::vec4(ux, uy, uz, 0.0f);
    
    lookAtPoint = position + view;

    w = -view / norm(view);
    u = crossproduct(up, w)/norm(crossproduct(up, w));
    v = crossproduct(w, u);

}

// Define a posição da câmera
void Camera::setPosition(float x, float y, float z){
    position.x = x;
    position.y = y;
    position.z = z;
}

// Define a posição da câmera
void Camera::setPosition(glm::vec4 position){
    this->position = position;
}

// Retorna posição da câmera
glm::vec4 Camera::getPosition(){
    return position;
}

// Recalcula vetor view com a posição da câmera e um ponto
void Camera::lookAt(glm::vec4& point){
    lookAtPoint = point;
    view = lookAtPoint - position;

    // Recalcula u, v e w
    w = -view / norm(view);
    u = crossproduct(up, w)/norm(crossproduct(up, w));
    v = crossproduct(w, u);

}

// Define o vetor view
void Camera::setViewVector(glm::vec4 view){
    this->view = view;

    // Recalcula ponto fixo
    lookAtPoint = position + view;

    // Recalcula u, v e w
    w = -view / norm(view);
    u = crossproduct(up, w)/norm(crossproduct(up, w));
    v = crossproduct(w, u);
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

    // Recalcula u, v e w
    w = -view / norm(view);
    u = crossproduct(up, w)/norm(crossproduct(up, w));
    v = crossproduct(w, u);
    

}

glm::vec4 const& Camera::getLookAtPoint(){
    return lookAtPoint;
}

// Matriz view da câmera
glm::mat4 Camera::getViewMatrix(){    
    return Matrix_Camera_View(position, view, up);
}

// Vetor view
glm::vec4 Camera::getViewVec(){
    return view;
}

// Vetor u
glm::vec4 Camera::getUVec(){
    return u;
}

// Vetor v
glm::vec4 Camera::getVVec(){
    return v;
}

// Vetor w
glm::vec4 Camera::getWVec(){
    return w;
}


// Retorna verdadeiro se uma animação foi realizada
bool Camera::animate(){
    static float lastAnimationTime = (float) glfwGetTime();

    float current_time = (float) glfwGetTime();
    float delta_t = current_time - lastAnimationTime;
    lastAnimationTime = current_time;
    
    if(!animationFlags){
        return false;
    }
    
    if(animationFlags & CAMERA_MOVING){
        // Anda em linha reta ao novo ponto
        glm::vec4 direction = destinationPoint - position;
        float dNorm = norm(direction);

        // Se está perto o suficiente, pula para o ponto
        if(dNorm < 0.1){
            position = destinationPoint;
            animationFlags ^= CAMERA_MOVING;
        }
        // Senão, move-se
        else{
            glm::vec4 nDirection = direction / norm(direction);
            position = position + (nDirection * delta_t * CAMERA_MOVE_SPEED);
        }
    }
    if(animationFlags & CAMERA_ROTATING){
        // Matriz de rotação
        glm::mat4 rotation;
        
        // 90 graus/seg
        auto radiansFromDeltaT = rotationSign*glm::radians(CAMERA_ROTATE_SPEED*delta_t);
        
        // Talvez tenha uma maneira mais esperta de fazer isso, mas foi o que consegui pensar as
        // 23:12
        if(((rotationSign > 0) && ((radiansToRotate - radiansFromDeltaT) < 0)) || \
            ((rotationSign < 0) && ((radiansFromDeltaT - radiansToRotate ) < 0))){
            // Rotaciona a quantidade exata necessária
            switch(rotationAxis){
                case X: rotation = Matrix_Rotate(radiansToRotate, w); break;
                case Y: rotation = Matrix_Rotate(radiansToRotate, v); break;
                case Z: rotation = Matrix_Rotate(radiansToRotate, u); break;
            }
            radiansToRotate = 0.0f;
            animationFlags ^= CAMERA_ROTATING;
        }
        else{
            // Rotaciona conforme delta_t
            switch(rotationAxis){
                case X: rotation = Matrix_Rotate(radiansFromDeltaT, w); break;
                case Y: rotation = Matrix_Rotate(radiansFromDeltaT, v); break;
                case Z: rotation = Matrix_Rotate(radiansFromDeltaT, u); break;
            }
            radiansToRotate -= radiansFromDeltaT;    
        }

        // Aplica matriz de rotação a view
        setViewVector(rotation * view);
    }

    return true;
}

// Configura a câmera para rotacionar
void Camera::setradiansToRotate(float radians, cameraAxis axis){
    radiansToRotate = radians;
    rotationSign = radians > 0 ? 1 : -1;
    rotationAxis = axis;
    animationFlags |= CAMERA_ROTATING;
}

// Define um ponto o qual a câmera deve se movimentar a
void Camera::setDestinationPoint(glm::vec4 dst){
    destinationPoint = dst;
    animationFlags |= CAMERA_MOVING;
}
