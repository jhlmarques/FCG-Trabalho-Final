#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <glad/glad.h>
#include <stb_image.h>

class Texture{

    // ID do Sampler Object
    GLuint sampler_id;
    // ID do Texture object
    GLuint texture_id;
    // Texture unit à qual a textura está atualmente ligada
    GLuint bound_unit;

    public:
    Texture(const char* filename);
    ~Texture();
    // Realiza bind dessa textura a um texture unit
    void bind(GLuint unit);
    // Realiza unbind dessa textura a um texture unit
    void unbind();

};



#endif // __TEXTURE_H__
