/*

    

*/
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "matrices.h"

// Altura da câmera (deslocamento no eixo y). Simula altura da cabeça
#define CAMERA_HEAD_HEIGHT 1.5f

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

    public:
    
    Camera(float px, float py, float pz, float vx, float vy, float vz, float ux, float uy, float uz);

    // Define posição da câmera
    // TO-DO: POSIÇÃO != LOOK AT
    void setPosition(float x, float y, float z);

    // Define a posição da câmera
    void setPosition(glm::vec4 position);

    // Retorna posição da 
    glm::vec4 getPosition();

    // Recalcula vetor view com a posição da  e um ponto
    void lookAt(glm::vec4& point);

    // Define o vetor view
    void setViewVector(glm::vec4 view);

    // Ajusta o vetor view usando coordenadas esféricas
    void setViewVector(float phi, float theta, float r);

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


};

//  principal (inicialmente em modo "free")
// O vetor view padrão é calculado por phi=0 theta=0 dist=epsilon, e deve ser atualizado de acordo
extern Camera mainCamera;


#endif // __CAMERA_H__


