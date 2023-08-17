#include "gameobject.h"

void GameObject::setDiffMap(Texture* diffMap) { this->diffMap = diffMap;}
void GameObject::setNormalMap(Texture* normalMap) { this->normalMap = normalMap;}

void GameObject::draw(){
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