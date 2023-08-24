#ifndef __LIGHTSOURCE_H__
#define __LIGHTSOURCE_H__

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "matrices.h"

#define DEFAULT_APERTURE_ANGLE M_PI
#define DEFAULT_DIRECTION glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)

class LightSource{
    private:
    // Posição da fonte de luz
    glm::vec4 position;
    // Ângulo de abertura da fonte de luz 
    float aperture_angle;
    // Direção da fonte de luz
    glm::vec4 direction;

    public:
    // Construtor padrão. Os parâmetros opcionais são utilizados somente simular o efeito spotlight em fontes de luz
    LightSource(glm::vec4 position, glm::vec4 direction=DEFAULT_DIRECTION, float aperture_angle=DEFAULT_APERTURE_ANGLE);

    void setPosition(glm::vec4 position);
    glm::vec4 getPosition();
    void setApertureAngle(float aperture_angle);
    float getApertureAngle();
    void setDirection(glm::vec4 direction);
    glm::vec4 getDirection();
};


#endif // __LIGHTSOURCE_H__