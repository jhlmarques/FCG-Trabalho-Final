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
    
    Material(GLuint firstIndex, GLuint numIndices, Texture& tex);
    Material(GLuint firstIndex, GLuint numIndices, tinyobj::material_t const& mat);

    public:
    void addIdx();
    void bindToShader();
    void unbind();
    GLuint getFirstIndex();
    GLuint getNumIndices();

    // Named constructors
    static Material createFromObjFile(GLuint firstIndex, GLuint numIndices, tinyobj::material_t const& mat);
    static Material createFromTexture(GLuint firstIndex, GLuint numIndices, Texture tex);

};

#endif // __MATERIAL_H__
