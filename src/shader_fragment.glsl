#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Escala da textura do objeto
uniform float object_texture_scale;

// Identificador do tipo de objeto
#define GENERIC_OBJECT 0
#define SPHERICAL_OBJECT 1

uniform int object_type;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Mapas de textura provenientes do material

// Decide entre utilizar as normais do material ou as computadas

uniform vec3 Kd0;
uniform vec3 Ka0;
uniform vec3 Ks0;
uniform float ns; // Expoente especular
uniform sampler2D diffMap;

// Posição, direção e abertura da fonte de luz da sala
uniform vec4 light_position;
uniform vec4 light_direction;
uniform float light_aperture_angle;

// Modelo de iluminação do objeto
uniform int illumination_model;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

// Modelos de iluminação
#define LAMBERT 0
#define PHONG 1
#define BLINN_PHONG 2

// Define se usamos a interpolação de gouraud
uniform bool interp_Gouraud;

// Cor obtida por interp de gouraud (vertex shader)
in vec4 color_interp;


void main()
{
    if(interp_Gouraud){
        color = color_interp;
    }   
    else{
        // Coordenadas de textura U e V
        float U = 0.0;
        float V = 0.0;

        // Aqui usamos o tipo do objeto. Possivelmente podemos ir passando
        // outros atributos para o shader, como por exemplo o tipo de interpolação de iluminação

        // Objetos genéricos (falta de um nome melhor) usam as coordenadas
        // de textura do objeto
        if ( object_type == GENERIC_OBJECT){
            U = texcoords.x;
            V = texcoords.y;
        }
        // Meio que um placeholder, mas mapeia textura com coordenadas esféricas
        else if ( object_type == SPHERICAL_OBJECT){

            // Cálculo de texturas com coordenadas esféricas
            vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
            vec4 p_vec = position_model - bbox_center;
            vec4 p_hat = bbox_center + (p_vec / length(p_vec));
            
            float phi = asin(p_hat[1]);
            float theta = atan(p_hat[0],p_hat[2]);
            U = (theta + M_PI) / (2 * M_PI);
            V = (phi + M_PI_2) / M_PI;
        }

        // Obtemos a posição da câmera utilizando a inversa da matriz que define o
        // sistema de coordenadas da câmera.
            // Obtemos a posição da câmera utilizando a inversa da matriz que define o
        // sistema de coordenadas da câmera.
        vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 camera_position = inverse(view) * origin;

        // O fragmento atual é coberto por um ponto que percente à superfície de um
        // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
        // sistema de coordenadas global (World coordinates). Esta posição é obtida
        // através da interpolação, feita pelo rasterizador, da posição de cada
        // vértice.
        vec4 p = position_world;

        // Normal do fragmento atual, interpolada pelo rasterizador a partir das
        // normais de cada vértice.
        vec4 n = normalize(normal);    

        // Vetor que define o sentido da câmera em relação ao ponto atual.
        vec4 v = normalize(camera_position - p);

        // Posição da fonte de luz
        vec4 l = light_position - p;
        
        const float epsilon = 1e-5;    
        float cos_beta = dot(normalize(light_direction), normalize(p - light_position));
        float cos_alpha = cos(light_aperture_angle);

        /*
            MODELOS DE ILUMINAÇÃO
        */

        vec3 Kd = Kd0 * texture(diffMap, vec2(U,V) * object_texture_scale).rgb;
        vec3 Ka = Ka0;
        vec3 Ks = Ks0; // Refletância especular do objeto
        float q = ns; // Expoente especular para o modelo de iluminação de Phong
        vec3 final_color;
        
        // Espectro da fonte de iluminação
        vec3 I = vec3(1.0,1.0,1.0); 
        
        // ILUMINAÇÃO DE LAMBERT (DIFUSA)
        // Termo difuso de Lambert
        vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l)); 

        // Equação da iluminação de lambert
        vec3 lambert_shading = lambert_diffuse_term;

        // ILUMINAÇÃO DE PHONG 
        // Espectro da luz ambiente
        vec3 Ia = vec3(0.2,0.2,0.2);
        // Termo ambiente 
        vec3 ambient_term = Ka * Ia; 
        // Vetor que define o sentido da reflexão especular ideal.
        vec4 r = -l + 2 * n * dot(n, l);
        // Termo especular utilizando o modelo de iluminação de Phong
        vec3 phong_specular_term  = Ks * I * pow(max(0, dot(r, v)), q); 

        // Equação da iluminação de Phong
        vec3 phong_shading = lambert_diffuse_term + ambient_term + phong_specular_term;

        // ILUMINAÇÃO DE BLINN-PHONG
        // Half-vector: vetor do meio do caminho entre v e l
        vec4 h = normalize(v + l);
        vec3 blinn_phong_specular_term = Ks * I * pow(max(0, dot(n, h)), q);

        // Equação da iluminação de Blinn-Phong
        vec3 blinn_phong_shading = lambert_diffuse_term + ambient_term + blinn_phong_specular_term;
        
        if (cos_beta < cos_alpha){
            color.rgb = ambient_term;
        }
        else{
            switch(illumination_model){
                case LAMBERT:
                    color.rgb = lambert_shading;
                    break;
                case PHONG:
                    color.rgb = phong_shading;
                    break;
                case BLINN_PHONG:
                    color.rgb = blinn_phong_shading;
                    break;  
            }
        }

        // NOTE: Se você quiser fazer o rendering de objetos transparentes, é
        // necessário:
        // 1) Habilitar a operação de "blending" de OpenGL logo antes de realizar o
        //    desenho dos objetos transparentes, com os comandos abaixo no código C++:
        //      glEnable(GL_BLEND);
        //      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // 2) Realizar o desenho de todos objetos transparentes *após* ter desenhado
        //    todos os objetos opacos; e
        // 3) Realizar o desenho de objetos transparentes ordenados de acordo com
        //    suas distâncias para a câmera (desenhando primeiro objetos
        //    transparentes que estão mais longe da câmera).
        // Alpha default = 1 = 100% opaco = 0% transparente
        color.a = 1;

        // Cor final com correção gamma, considerando monitor sRGB.
        // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
        color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
    }
}

