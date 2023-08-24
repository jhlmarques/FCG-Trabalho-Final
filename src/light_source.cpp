#include "light_source.h"

LightSource::LightSource(glm::vec4 position, glm::vec4 direction, float aperture_angle){
    this->position = position;
    this->direction = direction;
    this->aperture_angle = aperture_angle;
    
}

void LightSource::setApertureAngle(float aperture_angle){
    this->aperture_angle = aperture_angle;
}

void LightSource::setPosition(glm::vec4 position){
    this->position = position;
}

glm::vec4 LightSource::getPosition(){
    return this->position;
}

float LightSource::getApertureAngle(){
    return this->aperture_angle;
}

void LightSource::setDirection(glm::vec4 direction){
    this->direction = direction;
}

glm::vec4 LightSource::getDirection(){
    return this->direction;
}




