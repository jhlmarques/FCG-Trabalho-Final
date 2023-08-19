#include "texture.h"
#include "stdio.h"

std::map<std::string, std::pair<GLuint, GLuint>> g_loadedTextures;
std::string Texture::pathToTextureFolder;

// Textura vazia
Texture::Texture() : texture_id(0), sampler_id(0) {}

Texture::Texture(std::string filename)
{
    // Checa se a textura já está carregada na GPU
    if(g_loadedTextures.count(filename) > 0){
        texture_id = g_loadedTextures[filename].first;
        sampler_id = g_loadedTextures[filename].second;

        return;
    }

    // Filename vazio leva a textura vazia
    if(filename.empty()){
        printf("Usando textura vazia...\n");
        Texture();
        return;
    }


    printf("Carregando imagem \"%s\"... ", filename.c_str());

    stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int channels;

    std::string fullPath = pathToTextureFolder + filename;
    
    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &channels, 3);

    if ( data == NULL )
    {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", fullPath.c_str());
        std::exit(EXIT_FAILURE);
    }

    printf("OK (%dx%d).\n", width, height);

    // Agora criamos objetos na GPU com OpenGL para armazenar a textura
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    // Parâmetros de sampling
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Agora enviamos a imagem lida do disco para a GPU
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    // Binding da textura, fazendo com que operações GL a afetem
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // A esse ponto, a textura está carregada na GPU

    stbi_image_free(data);

    // Adiciona ao mapa global de texturas
    g_loadedTextures[filename].first = texture_id;
    g_loadedTextures[filename].second = sampler_id;


}

// Realiza bind dessa textura a um texture unit
void Texture::bind(GLuint unit){
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindSampler(unit, sampler_id);
    bound_unit = unit;
}

// Unbind do objeto de textura do slot atual (definido em bind())
void Texture::unbind(){
    glActiveTexture(bound_unit);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindSampler(bound_unit, 0);
}

// Define o local onde ficam as texturas
void Texture::setTextureFolderRoot(const char* path){
    pathToTextureFolder = path;
}