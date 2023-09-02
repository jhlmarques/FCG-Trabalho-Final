#include "animation.h"

AnimationManager g_AnimationManager;

AnimationManager::AnimationManager() : curAnimationID(ANIMATION_ID_INITIAL_VALUE) {}

int AnimationManager::addAnimatedCamera(Camera* camera, AnimationData data){
    mapAnimatedCameras[curAnimationID] = std::pair<AnimationData, Camera*>(data, camera);
    return curAnimationID++;
}

int AnimationManager::addAnimatedObject(GameObject* obj, AnimationData data){
    mapAnimatedObjects[curAnimationID] = std::pair<AnimationData, GameObject*>(data, obj);
    return curAnimationID++;
}

bool AnimationManager::hasAnimationFinished(int animationID, bool isObject=true){
    if(isObject){
        if(mapAnimatedObjects.find(animationID) == mapAnimatedObjects.end()){
            return true;
        }
        return mapAnimatedObjects[animationID].first.isFinished();
    }
    else{
        if(mapAnimatedCameras.find(animationID) == mapAnimatedCameras.end()){
            return true;
        }
        return mapAnimatedCameras[animationID].first.isFinished();
    }
}

void AnimationManager::animationStep(){
    animateCameras();
    animateObjects();
}

void AnimationManager::animateObjects(){
static float lastAnimationTime = (float) glfwGetTime();

    float current_time = (float) glfwGetTime();
    float delta_t = current_time - lastAnimationTime;
    lastAnimationTime = current_time;

    // Itera por pares (chave, valor)
    for(auto& animationPair : mapAnimatedObjects){
        AnimationData& data = animationPair.second.first;
        GameObject* obj = animationPair.second.second;

        // Checa se animação acabou
        if(data.isFinished()){
            mapAnimatedObjects.erase(animationPair.first);
            continue;
        }

        glm::vec4 position = obj->getPosition();

        // Animações
        if(data.animationFlags & ANIMATION_MOVING){
            // Anda em linha reta ao novo ponto
            glm::vec4 direction = data.destinationPoint - position;
            float dNorm = norm(direction);

            // Se está perto o suficiente, pula para o ponto
            if(dNorm < 0.1){
                position = data.destinationPoint;
                data.animationFlags ^= ANIMATION_MOVING;
            }
            // Senão, move-se
            else{
                glm::vec4 nDirection = direction / norm(direction);
                position = position + (nDirection * delta_t * CAMERA_MOVE_SPEED);
            }

            // Atualiza posição
            obj->setPosition(position);
        }

        if(data.animationFlags & ANIMATION_ROTATING){
            // Matriz de rotação
            glm::mat4 rotation;
            
            // 90 graus/seg
            // TO-DO: AJUSTAR VELOCIDADE USANDO ANIMATION DATA
            auto radiansFromDeltaT = data.rotationSign*glm::radians(12.0*delta_t);
            
            // Talvez tenha uma maneira mais esperta de fazer isso, mas foi o que consegui pensar as
            // 23:12
            if(((data.rotationSign > 0) && ((data.radiansToRotate - radiansFromDeltaT) < 0)) || \
                ((data.rotationSign < 0) && ((radiansFromDeltaT - data.radiansToRotate ) < 0))){
                // Rotaciona a quantidade exata necessária
                switch(data.rotationAxis){
                    case X: obj->setEulerAngleX(obj->getEulerAngleX() + data.radiansToRotate); break;
                    case Y: obj->setEulerAngleY(obj->getEulerAngleY() + data.radiansToRotate); break;
                    case Z: obj->setEulerAngleZ(obj->getEulerAngleZ() + data.radiansToRotate); break;
                }
                data.radiansToRotate = 0.0f;
                data.animationFlags ^= ANIMATION_ROTATING;
            }
            else{
                // Rotaciona conforme delta_t
                switch(data.rotationAxis){
                    case X: obj->setEulerAngleX(obj->getEulerAngleX() + radiansFromDeltaT); break;
                    case Y: obj->setEulerAngleY(obj->getEulerAngleY() + radiansFromDeltaT); break;
                    case Z: obj->setEulerAngleZ(obj->getEulerAngleZ() + radiansFromDeltaT); break;
                }
                data.radiansToRotate -= radiansFromDeltaT;    
            }
        }
    }
}

void AnimationManager::animateCameras(){
    static float lastAnimationTime = (float) glfwGetTime();

    float current_time = (float) glfwGetTime();
    float delta_t = current_time - lastAnimationTime;
    lastAnimationTime = current_time;

    // Itera por pares (chave, valor)
    for(auto& animationPair : mapAnimatedCameras){
        AnimationData& data = animationPair.second.first;
        Camera* camera = animationPair.second.second;

        // Checa se animação acabou
        if(data.isFinished()){
            mapAnimatedCameras.erase(animationPair.first);
            continue;
        }

        glm::vec4 position = camera->getPosition();

        // Animações
        if(data.animationFlags & ANIMATION_MOVING){
            // Anda em linha reta ao novo ponto
            glm::vec4 direction = data.destinationPoint - position;
            float dNorm = norm(direction);

            // Se está perto o suficiente, pula para o ponto
            if(dNorm < 0.1){
                position = data.destinationPoint;
                data.animationFlags ^= ANIMATION_MOVING;
            }
            // Senão, move-se
            else{
                glm::vec4 nDirection = direction / norm(direction);
                position = position + (nDirection * delta_t * CAMERA_MOVE_SPEED);
            }

            // Atualiza posição
            camera->setPositionFree(position);
        }

        if(data.animationFlags & ANIMATION_ROTATING){
            // Matriz de rotação
            glm::mat4 rotation;
            
            // 90 graus/seg
            // TO-DO: AJUSTAR VELOCIDADE USANDO ANIMATION DATA
            auto radiansFromDeltaT = data.rotationSign*glm::radians(CAMERA_ROTATE_SPEED*delta_t);
            
            // Talvez tenha uma maneira mais esperta de fazer isso, mas foi o que consegui pensar as
            // 23:12
            if(((data.rotationSign > 0) && ((data.radiansToRotate - radiansFromDeltaT) < 0)) || \
                ((data.rotationSign < 0) && ((radiansFromDeltaT - data.radiansToRotate ) < 0))){
                // Rotaciona a quantidade exata necessária
                switch(data.rotationAxis){
                    case X: rotation = Matrix_Rotate(data.radiansToRotate, camera->getWVec()); break;
                    case Y: rotation = Matrix_Rotate(data.radiansToRotate, camera->getVVec()); break;
                    case Z: rotation = Matrix_Rotate(data.radiansToRotate, camera->getUVec()); break;
                }
                data.radiansToRotate = 0.0f;
                data.animationFlags ^= ANIMATION_ROTATING;
            }
            else{
                // Rotaciona conforme delta_t
                switch(data.rotationAxis){
                    case X: rotation = Matrix_Rotate(radiansFromDeltaT, camera->getWVec()); break;
                    case Y: rotation = Matrix_Rotate(radiansFromDeltaT, camera->getVVec()); break;
                    case Z: rotation = Matrix_Rotate(radiansFromDeltaT, camera->getUVec()); break;
                }
                data.radiansToRotate -= radiansFromDeltaT;    
            }

            // Aplica matriz de rotação a view
            camera->setViewVectorFree(rotation * camera->getViewVec());
        }
    }

}

AnimationData::AnimationData() :
    radiansToRotate(0.0),
    rotationSign(0),
    rotationAxis(Y),
    destinationPoint{0.0,0.0,0.0,0.0},
    animationFlags(0)
{}

void AnimationData::setradiansToRotate(float radians, Axis axis){
    radiansToRotate = radians;
    rotationSign = radians > 0 ? 1 : -1;
    rotationAxis = axis;
    animationFlags |= ANIMATION_ROTATING;
}

void AnimationData::setDestinationPoint(glm::vec4 dst){
    destinationPoint = dst;
    animationFlags |= ANIMATION_MOVING;
}

bool AnimationData::isFinished(){
    return (animationFlags == 0);
}