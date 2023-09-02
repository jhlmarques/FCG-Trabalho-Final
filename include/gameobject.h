#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <glad/glad.h>
#include <utility>
#include "texture.h"
#include "labfunctions.h"
#include "labstructures.h"
#include "globals.h"
#include "material.h"
#include "light_source.h"
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#define OBJ_GENERIC 0
#define OBJ_SPHERICAL 1

// Modelos de iluminação
#define LAMBERT 0
#define PHONG 1
#define BLINN_PHONG 2


// Um objeto do jogo. 
// Adaptado da estrutura "SceneObject" dos laboratórios

class GameObject{
    
    private:
    std::string  name;        // Nome do objeto
    size_t       first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    size_t       num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    GLuint       vertex_array_object_id; // ID do VAO onde estão armazenados os atributos do modelo
    glm::vec3    bbox_min; // Axis-Aligned Bounding Box do objeto
    glm::vec3    bbox_max;    
    int          illumination_model;

    // Escala do objeto
    glm::mat4 scaleMatrix;
    // Rotação do objeto
    glm::mat4 rotationMatrix;

    // Posição do objeto
    glm::vec4 position;
    // Ângulos de euler
    float eulerAngleX;
    float eulerAngleY;
    float eulerAngleZ;


    // Materiais do objeto
    std::vector<Material> materials;

    // Tipo de objeto (para interações mais peculiares)
    GLuint type;
    // Escala da textura do objeto
    float textureScale;

    public:
    
    GameObject(ObjModel* model, GLuint shapeIdx);

    void setTextureScale(float scale);
    void setIlluminationModel(int illumination_model);

    // Desenha o objeto
    void draw(LightSource& lightSource);

    // Define posição do objeto
    void setPosition(glm::vec4 pos);
    // Retorna posição do objeto
    glm::vec4 getPosition();
    // Obtém vetor view
    glm::vec4 getView();
    // Define vetor view
    void setView(glm::vec4 view);
    // Define a escala
    void setScale(float x, float y, float z);
    // Retorna matriz de escala
    glm::mat4 getScaleMatrix();
    // Retorna matriz de rotação baseado nos ângulo de euler
    glm::mat4 getRotationMatrix();

    void setEulerAngleX(float angle);
    float getEulerAngleX();
    void setEulerAngleY(float angle);
    float getEulerAngleY();
    void setEulerAngleZ(float angle);
    float getEulerAngleZ();

    glm::vec4 getWVec();
    glm::vec4 getVVec();
    glm::vec4 getUVec();


};


// Desenha o objeto com este nome contido na cena virtual
//void DrawVirtualGameObject(const char* name);

#endif // __GAMEOBJECT_H__
