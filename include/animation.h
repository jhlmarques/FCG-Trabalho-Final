#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "camera.h"
#include "gameobject.h"
#include <utility>

// Constantes para movimentação da câmera
#define ANIMATION_ROTATING 1
#define ANIMATION_MOVING 2
#define ANIMATION_BEZIER 4

// ID zero reservado para "nenhuma animação"
#define ANIMATION_ID_NONE 0
#define ANIMATION_ID_INITIAL_VALUE 1


enum Axis{
    X,
    Y,
    Z
};

struct AnimationData{
    uint8_t animationFlags;
    
    float radiansToRotate;
    int8_t rotationSign;
    Axis rotationAxis;
    glm::vec4 destinationPoint;

    glm::vec4 bezierPoint1;
    glm::vec4 bezierPoint2;
    glm::vec4 bezierPoint3;
    glm::vec4 bezierPoint4;

    float bezierPercentage;

    // Velocidades
    float linearMovementVelocity;
    float rotationVelocity;
    float bezierVelocity;


    AnimationData();

    void setradiansToRotate(float radians, Axis axis, float velocity);
    void setDestinationPoint(glm::vec4 dst, float velocity);
    // Faz com que o movimento ao ponto seja definido por uma curva de bezier
    void setBezierCurveJump(glm::vec4 p1, glm::vec4 p2, glm::vec4 p3, glm::vec4 p4, float velocity);
    bool isFinished();

    glm::vec4 computeBezierPos(float percentage);

};


class AnimationManager{

    private:
    int curAnimationID;

    std::map<int, std::pair<AnimationData, GameObject*> > mapAnimatedObjects;
    std::map<int, std::pair<AnimationData, Camera*> > mapAnimatedCameras;


    void animateCameras();
    void animateObjects();

    public:
    AnimationManager();

    int addAnimatedObject(GameObject* obj, AnimationData animation);
    int addAnimatedCamera(Camera* camera, AnimationData animation);

    bool hasAnimationFinished(int animationID, bool isObject);

    // Remove animação
    void removeAnimation(int id, bool isObject);

    // Realiza animações
    void animationStep();

};

extern AnimationManager g_AnimationManager;

#endif // __ANIMATION_H__
