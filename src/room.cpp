#include "room.h"

Room::Room() : 
    backgroundColor(BLACK_BACKGROUND_COLOR), 
    camera(glm::vec3(0,0,0)) ,
    lightSource(glm::vec4(0,0,0,0), glm::vec4 (0,0,0,0), 0.0f)
    {}

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



