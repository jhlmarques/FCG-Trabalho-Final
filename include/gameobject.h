#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <glad/glad.h>
#include "texture.h"
#include "labfunctions.h"
#include "globals.h"

#define OBJ_GENERIC 0
#define OBJ_SPHERICAL 1


// Um objeto do jogo. Contém mapas de textura e outras informações relevantes
class GameObject{
    private:
    // O nome de um objeto na cena virtual
    const char* objName;
    // Mapas de textura
    Texture* diffMap;
    Texture* normalMap;
    // Tipo de objeto
    GLuint type;
    // Escala da textura do objeto
    float textureScale;

    public:
    GameObject(const char* objName, GLuint type) : 
        objName(objName), diffMap(nullptr), normalMap(nullptr), type(type), textureScale(1.0f) {}

    void setDiffMap(Texture* diffMap);
    void setNormalMap(Texture* normalMap);
    void setTextureScale(float scale);

    // Desenha o objeto
    void draw();

};

#endif // __GAMEOBJECT_H__
