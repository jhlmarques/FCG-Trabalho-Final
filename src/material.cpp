#include "material.h"

Material Material::createFromObjFile(GLuint firstIndex, GLuint numIndices, tinyobj::material_t const& mat){
    return Material(firstIndex, numIndices, mat);
}

Material Material::createFromTexture(GLuint firstIndex, GLuint numIndices, Texture tex){
    return Material(firstIndex, numIndices, tex);
}

Material::Material(GLuint firstIndex, GLuint numIndices, Texture& tex): 
firstIndex(firstIndex), numIndices(numIndices),
kd(1.0f, 1.0f, 1.0f), diffMap(tex), useBumpMap(false)
{

}

Material::Material(GLuint firstIndex, GLuint numIndices, tinyobj::material_t const& mat) :
    firstIndex(firstIndex), numIndices(numIndices),
    kd(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]), diffMap(mat.diffuse_texname),
    ka(mat.ambient[0], mat.ambient[1], mat.ambient[2]), ambientMap(mat.ambient_texname),
    ks(mat.specular[0], mat.specular[1], mat.specular[2]), specularMap(mat.specular_highlight_texname),
    ns(mat.shininess),
    bumpMap(mat.bump_texname)
{
    useBumpMap = !mat.bump_texname.empty();
}

void Material::bindToShader(){
    static GLuint kd_coeff_uniform = glGetUniformLocation(g_GpuProgramID, "Kd0");
    static GLuint ka_coeff_uniform = glGetUniformLocation(g_GpuProgramID, "Ka0");
    static GLuint ks_coeff_uniform = glGetUniformLocation(g_GpuProgramID, "Ks0");
    static GLuint ns_coeff_uniform = glGetUniformLocation(g_GpuProgramID, "ns");
    static GLuint bumpMap_option_uniform = glGetUniformLocation(g_GpuProgramID, "useBMap");

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
    // Mapa normal
    glUniform1i(bumpMap_option_uniform, useBumpMap);
    bumpMap.bind(GL_TEXTURE3);

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