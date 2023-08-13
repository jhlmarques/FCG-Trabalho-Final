/*

    

*/
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "matrices.h"

class Camera{

    private:
    glm::vec4 position;
    glm::vec4 view;
    glm::vec4 up;

    glm::mat4 viewMatrix;

    glm::vec4 w;
    glm::vec4 u;    

    glm::vec4 lookAtPoint;

    public:
    
    Camera(float px, float py, float pz, float vx, float vy, float vz, float ux, float uy, float uz);

    // Define posição da 
    // TO-DO: POSIÇÃO != LOOK AT
    void setPosition(float x, float y, float z);

    // Retorna posição da 
    glm::vec4 getPosition();

    // Recalcula vetor view com a posição da  e um ponto
    void lookAt(glm::vec4& point);

    // Ajusta o vetor view usando coordenadas esféricas
    void setViewVector(float phi, float theta, float r);

    // Obtém ponto o qual a  está olhando
    glm::vec4 const& getLookAtPoint();
    
    // Matriz view da câmera
    glm::mat4 getViewMatrix();

    // Vetor w
    glm::vec4 getWVec();

    // Vetor u
    glm::vec4 getUVec();



};

//  principal (inicialmente em modo "free")
// O vetor view padrão é calculado por phi=0 theta=0 dist=epsilon, e deve ser atualizado de acordo
extern Camera mainCamera;


#endif // __CAMERA_H__

