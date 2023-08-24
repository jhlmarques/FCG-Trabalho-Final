#ifndef __ROOM_H__
#define __ROOM_H__

#include "camera.h"
#include "light_source.h"
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <string>

#define BLACK_BACKGROUND_COLOR glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
#define WHITE_BACKGROUND_COLOR glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)

class Room
{
    private:
    glm::vec4 backgroundColor;
    Camera camera;
    LightSource lightSource;

    public:
    
    // TO-DO: Eu tava pensando em fazer um map para acessar os objetos da sala, mas ainda vou elaborar a ideia
    // static std::map<std::string, GameObject> gameObjects;
    
    void setBackgroundColor(glm::vec4 backgroundColor);
    void setCamera(Camera camera);
    void setLightSource(LightSource lightSource);
    glm::vec3 getBackgroundColor();
    // Retorna a referência da câmera para que possamos alterar seus atributos
    Camera& getCamera();
    LightSource& getLightSource();

    Room(Camera camera, LightSource lightSource, glm::vec4 backgroundColor=WHITE_BACKGROUND_COLOR);
};

#endif