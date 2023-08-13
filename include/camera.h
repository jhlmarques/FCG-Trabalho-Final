/*

    Câmera

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

    // LOOK AT
    glm::vec4 lookAtPoint;

    // FREE
    float phi;
    float theta;
    float distance;

    bool lookat;

    public:
    
    Camera(float px, float py, float pz, float ux, float uy, float uz);

    // Define posição da câmera
    void setPosition(float x, float y, float z);

    // Retorna posição da câmera
    glm::vec4 getPosition();

    // Define como o vetor view é calculado.
    void setCameraMode(bool lookat);

    // Define o ponto utilizado para calcular o vetor view no modo Look At
    void setViewLookAtPoint(glm::vec4& point);

    // Define a magnitude do vetor view fixo no modo Free
    void setViewMagnitude(float distance);

    // Define as coordenadas polares usadas para o cálculo do vetor view fixo no modo Free
    void setViewPolarCoords(float phi, float theta);
    
    // Recalcula elementos da câmera
    void applyChanges();
    
    // Matriz view da câmera
    glm::mat4 const& getViewMatrix();

    // Vetor w
    glm::vec4 getWVec();

    // Vetor u
    glm::vec4 getUVec();



};

#endif // __CAMERA_H__


