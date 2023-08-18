#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <glad/glad.h>
#include <utility>
#include "texture.h"
#include "labfunctions.h"
#include "labstructures.h"
#include "globals.h"
#include "material.h"

#define OBJ_GENERIC 0
#define OBJ_SPHERICAL 1


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
    
    // Materiais do objeto
    std::vector<Material> materials;

    Texture defaultTexture;


    // Tipo de objeto (para interações mais peculiares)
    GLuint type;
    // Escala da textura do objeto
    float textureScale;

    public:
    
    GameObject(ObjModel* model, GLuint shapeIdx);

    void setTextureScale(float scale);

    // Desenha o objeto
    void draw();

    // Recebe um ObjModel e retorna os GameObjects criados, com seus triângulos carregados na GPU
    // Adaptação de BuildTrianglesAndAddToVirtualScene()
    static void buildObjectsFromModel(ObjModel* model, std::vector<GameObject>& out);

};


// Desenha o objeto com este nome contido na cena virtual
//void DrawVirtualGameObject(const char* name);

#endif // __GAMEOBJECT_H__
