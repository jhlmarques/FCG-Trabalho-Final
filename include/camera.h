/*

    

*/
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "matrices.h"

// Altura da câmera (deslocamento no eixo y). Simula altura da cabeça
#define CAMERA_HEAD_HEIGHT 4.0f

#define CAMERA_ROTATING 1
#define CAMERA_MOVING 2

#define CAMERA_ROTATE_SPEED 180.0f
#define CAMERA_MOVE_SPEED 10.0f

enum cameraAxis{
    X,
    Y,
    Z
};

class Camera{

    private:
    glm::vec4 position;
    glm::vec4 view;
    glm::vec4 up;

    glm::mat4 viewMatrix;

    glm::vec4 w;
    glm::vec4 u; 
    glm::vec4 v;   

    glm::vec4 lookAtPoint;

    // Animação
    uint8_t animationFlags;
    float radiansToRotate;
    int8_t rotationSign;
    cameraAxis rotationAxis;
    glm::vec4 destinationPoint;

    public:
    
    Camera(float px, float py, float pz, float vx, float vy, float vz, float ux, float uy, float uz);

    // Define posição da câmera com coordenadas esféricas 
    void setPositionSpheric(float phi, float theta, float r);

    // Define posição da câmera com coordenadas cartesianas
    void setPositionCartesian(float x, float y, float z);

    // Define a posição da câmera com um ponto
    void setPosition(glm::vec4 position);

    // Retorna posição da câmera
    glm::vec4 getPosition();

    // Recalcula vetor view com a posição da câmera e o novo ponto que a câmera irá ficar fixa
    void lookAt(glm::vec4& point);

    // Define o vetor view diretamente (utilizado para câmeras livres)
    void setViewVector(glm::vec4 view);

    // Define o vetor view usando coordenadas esféricas (utilizado para câmeras lookat)
    void setViewVectorSpheric(float phi, float theta, float r);

    // Obtém ponto o qual a  está olhando
    glm::vec4 const& getLookAtPoint();
    
    // Matriz view da câmera
    glm::mat4 getViewMatrix();

    // Vetor view
    glm::vec4 getViewVec();

    // Vetor u
    glm::vec4 getUVec();

    // Vetor v
    glm::vec4 getVVec();

    // Vetor w
    glm::vec4 getWVec();

    // Retorna verdadeiro se uma animação foi realizada
    bool animate();

    // Configura a câmera para rotacionar
    void setradiansToRotate(float radians, cameraAxis axis);
    
    // Define um ponto o qual a câmera deve se movimentar a
    void setDestinationPoint(glm::vec4 dst);


};

//  principal (inicialmente em modo "free")
// O vetor view padrão é calculado por phi=0 theta=0 dist=epsilon, e deve ser atualizado de acordo
extern Camera mainCamera;

#endif // __CAMERA_H__


