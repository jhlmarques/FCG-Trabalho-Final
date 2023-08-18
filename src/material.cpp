#include "material.h"

void Material::addIdx(){
    numIndices++;
}

void Material::bindToShader(){
    static GLuint kd_coeff_uniform = glGetUniformLocation(g_GpuProgramID, "Kd0");

    // Difuso
    glUniform3f(kd_coeff_uniform, kd.r, kd.g, kd.b);
    diffMap.bind(0);
}

void Material::unbind(){
    diffMap.unbind();
}

GLuint Material::getFirstIndex(){
    return firstIndex;
}

GLuint Material::getNumIndices(){
    return numIndices;
}