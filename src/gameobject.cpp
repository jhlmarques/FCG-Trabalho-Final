#include "gameobject.h"

void GameObject::setDiffMap(Texture* diffMap) { this->diffMap = diffMap;}
void GameObject::setNormalMap(Texture* normalMap) { this->normalMap = normalMap;}
void GameObject::setTextureScale(float scale) { this->textureScale = scale;}

void GameObject::draw(){
    glUniform1f(g_object_texture_scale, textureScale);
    glUniform1i(g_object_type_uniform, type);
    
    if(diffMap){
        diffMap->bind(0);
    }
    if(normalMap){
        normalMap->bind(1);
    }
    DrawVirtualObject(objName);
    if(diffMap){
        diffMap->unbind();
    }
    if(normalMap){
        normalMap->unbind();
    }
    
}