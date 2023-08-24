#include "room.h"

Room::Room(Camera camera, LightSource lightSource, glm::vec4 backgroundColor) : 
    camera(camera), 
    lightSource(lightSource), 
    backgroundColor(backgroundColor)
{
}

void Room::setBackgroundColor(glm::vec4 backgroundColor){
    this->backgroundColor = backgroundColor;
}

void Room::setCamera(Camera camera){
    this->camera = camera;
}

void Room::setLightSource(LightSource lightSource){
    this->lightSource = lightSource;
}

glm::vec4 Room::getBackgroundColor(){
    return this->backgroundColor;
}

Camera& Room::getCamera(){
    return this->camera;
}

LightSource& Room::getLightSource(){
    return this->lightSource;
}



