#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include "tiny_obj_loader.h"
#include "texture.h"
#include "globals.h"

class Material{

    private:
    GLuint firstIndex;
    GLuint numIndices;

    glm::vec3 kd;
    glm::vec3 ka;
    glm::vec3 ks;
    float ns;
    Texture diffMap;
    Texture ambientMap;
    Texture specularMap;
    

    public:
    Material(GLuint firstIndex, tinyobj::material_t const& mat);
    void addIdx();
    void bindToShader();
    void unbind();
    GLuint getFirstIndex();
    GLuint getNumIndices();

};

#endif // __MATERIAL_H__
