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
                position = position + (nDirection * delta_t * data.linearMovementVelocity);
            }


            // Atualiza posição
            obj->setPosition(position);
        }

        if(data.animationFlags & ANIMATION_BEZIER){
            if(data.bezierPercentage > 0.99){
                position = data.bezierPoint4;
                data.animationFlags ^= ANIMATION_BEZIER;
            }
            else{
                position = data.computeBezierPos(data.bezierPercentage + (delta_t * data.bezierVelocity));
            }
            // Atualiza posição
            obj->setPosition(position);
        }

        if(data.animationFlags & ANIMATION_ROTATING){
            // Matriz de rotação
            glm::mat4 rotation;
            
            auto radiansFromDeltaT = data.rotationSign*glm::radians(data.rotationVelocity*delta_t);
            
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
                position = position + (nDirection * delta_t * data.linearMovementVelocity);
            }

            // Atualiza posição
            camera->setPositionFree(position);
        }

        if(data.animationFlags & ANIMATION_BEZIER){
            if(data.bezierPercentage > 0.99){
                position = data.bezierPoint4;
                data.animationFlags ^= ANIMATION_BEZIER;
            }
            else{
                position = data.computeBezierPos(data.bezierPercentage + (delta_t * data.bezierVelocity));
            }
            // Atualiza posição
            camera->setPositionFree(position);
        }

        if(data.animationFlags & ANIMATION_ROTATING){
            // Matriz de rotação
            glm::mat4 rotation;
            
            auto radiansFromDeltaT = data.rotationSign*glm::radians(data.rotationVelocity*delta_t);
            
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

void AnimationManager::removeAnimation(int id, bool isObject){
    if(isObject){
        mapAnimatedObjects.erase(id);
    }
    else{
        mapAnimatedCameras.erase(id);
    }

}

AnimationData::AnimationData() :
    radiansToRotate(0.0),
    rotationSign(0),
    rotationAxis(Y),
    destinationPoint{0.0,0.0,0.0,0.0},
    animationFlags(0)
{}

void AnimationData::setradiansToRotate(float radians, Axis axis, float velocity){
    radiansToRotate = radians;
    rotationSign = radians > 0 ? 1 : -1;
    rotationAxis = axis;
    animationFlags |= ANIMATION_ROTATING;
    rotationVelocity = velocity;
}

void AnimationData::setDestinationPoint(glm::vec4 dst, float velocity){
    destinationPoint = dst;
    animationFlags |= ANIMATION_MOVING;
    linearMovementVelocity = velocity;
}

void AnimationData::setBezierCurveJump(glm::vec4 p1, glm::vec4 p2, glm::vec4 p3, glm::vec4 p4, float velocity){
    this->bezierPoint1 = p1;
    this->bezierPoint2 = p2;
    this->bezierPoint3 = p3;
    this->bezierPoint4 = p4;
    bezierPercentage = 0.0;
    animationFlags |= ANIMATION_BEZIER;
    bezierVelocity = velocity;
}

bool AnimationData::isFinished()
{
    return (animationFlags == 0);
}

glm::vec4 AnimationData::computeBezierPos(float percentage){
    bezierPercentage = percentage;
    
    auto c12 = bezierPoint1 + percentage*(bezierPoint2 - bezierPoint1);
    auto c23 = bezierPoint2 + percentage*(bezierPoint3 - bezierPoint2);
    auto c34 = bezierPoint3 + percentage*(bezierPoint4 - bezierPoint3);
    auto c123 = c12 + percentage*(c23 - c12);
    auto c234 = c23 + percentage*(c34 - c23);
    return c123 + percentage*(c234 - c123);
}
