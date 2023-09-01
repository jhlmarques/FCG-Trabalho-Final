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
#define CAMERA_ROTATE_SPEED 180.0f
#define CAMERA_MOVE_SPEED 10.0f
// Constantes para inicialização da câmera
#define CAMERA_DEFAULT_THETA 0.0f
#define CAMERA_DEFAULT_PHI 0.0f
#define CAMERA_DEFAULT_DISTANCE 3.5f
#define CAMERA_DEFAULT_UP_VECTOR glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) // Para cima
#define CAMERA_DEFAULT_VIEW_VECTOR glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) // Para frente

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
    float theta;
    float phi;
    float r;

    public:
    
    // A câmera por padrão é free, sendo necessário setar um ponto de lookat caso queira mudar para lookat
    Camera(glm::vec3 cameraStartingPosition);

    // Retorna posição da câmera
    glm::vec4 getPosition();    
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

    // CÂMERA FREE

    // Define o vetor view diretamente 
    void setViewVectorFree(glm::vec4 view);
    // Define a posição da câmera com um ponto
    void setPositionFree(glm::vec4 position);

    // CÂMERA LOOKAT

    // Chamado nos callbacks após o mouse ser movido. Assume que houve alguma alteração nas coordenadas esféricas 
    void updateViewVecLookAt();
    // Define o ponto que a câmera ficará olhando fixamente
    void setLookAtPoint(glm::vec4 point);
    // Obtém ponto o qual a  está olhando
    glm::vec4 const& getLookAtPoint();
    // Define o theta da câmera em coordenadas esféricas
    void setCameraTheta(float theta);
    float getCameraTheta();
    // Define o phi da câmera em coordenadas esféricas
    void setCameraPhi(float phi);
    float getCameraPhi();
    // Define a distância da câmera até o ponto de lookat
    void setCameraDistance(float r);
    float getCameraDistance();
    
};

#endif // __CAMERA_H__


