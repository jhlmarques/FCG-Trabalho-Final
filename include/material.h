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
    Texture diffMap;

    public:
    Material(GLuint firstIndex, tinyobj::material_t const& mat) :
    firstIndex(firstIndex), numIndices(0),
    kd(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]), diffMap(mat.diffuse_texname)
    {};
    void addIdx();
    void bindToShader();
    void unbind();
    GLuint getFirstIndex();
    GLuint getNumIndices();

};

#endif // __MATERIAL_H__
