#include "material.h"

Material::Material(GLuint firstIndex, tinyobj::material_t const& mat) :
    firstIndex(firstIndex), numIndices(0),
    kd(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]), diffMap(mat.diffuse_texname),
    ka(mat.ambient[0], mat.ambient[1], mat.ambient[2]), ambientMap(mat.ambient_texname),
    ks(mat.specular[0], mat.specular[1], mat.specular[2]), specularMap(mat.specular_highlight_texname),
    ns(mat.shininess)
{

}

void Material::addIdx(){
    numIndices++;
}

void Material::bindToShader(){
    static GLuint kd_coeff_uniform = glGetUniformLocation(g_GpuProgramID, "Kd0");
    static GLuint ka_coeff_uniform = glGetUniformLocation(g_GpuProgramID, "Ka0");
    static GLuint ks_coeff_uniform = glGetUniformLocation(g_GpuProgramID, "Ks0");
    static GLuint ns_coeff_uniform = glGetUniformLocation(g_GpuProgramID, "ns");

    // Difuso
    glUniform3f(kd_coeff_uniform, kd.r, kd.g, kd.b);
    diffMap.bind(GL_TEXTURE0);
    // Ambiente
    glUniform3f(ka_coeff_uniform, ka.r, ka.g, ka.b);
    ambientMap.bind(GL_TEXTURE1);
    // Especular
    glUniform3f(ks_coeff_uniform, ks.r, ks.g, ks.b);
    glUniform1f(ns_coeff_uniform, ns);
    specularMap.bind(GL_TEXTURE2);

}

void Material::unbind(){
    diffMap.unbind();
    specularMap.unbind();
}

GLuint Material::getFirstIndex(){
    return firstIndex;
}

GLuint Material::getNumIndices(){
    return numIndices;
}