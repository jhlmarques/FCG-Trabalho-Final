#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTrianglesAndAddToVirtualScene() em "main.cpp".
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec2 texture_coefficients;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Atributos de vértice que serão gerados como saída ("out") pelo Vertex Shader.
// ** Estes serão interpolados pelo rasterizador! ** gerando, assim, valores
// para cada fragmento, os quais serão recebidos como entrada pelo Fragment
// Shader. Veja o arquivo "shader_fragment.glsl".
out vec4 position_world;
out vec4 position_model;
out vec4 normal;
out vec2 texcoords;

/*

    MESMA COISAS QUE NO FRAGMENT, USADAS PARA GOURAUD

*/

uniform float object_texture_scale;
#define GENERIC_OBJECT 0
#define SPHERICAL_OBJECT 1
uniform int object_type;
uniform vec4 bbox_min;
uniform vec4 bbox_max;
uniform vec3 Kd0;
uniform vec3 Ka0;
uniform vec3 Ks0;
uniform float ns;
uniform sampler2D diffMap;
uniform vec4 light_position;
uniform vec4 light_direction;
uniform float light_aperture_angle;
uniform int illumination_model;
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define LAMBERT 0
#define PHONG 1
#define BLINN_PHONG 2
out vec4 color_interp;
uniform bool interp_Gouraud;


void main()
{
    // A variável gl_Position define a posição final de cada vértice
    // OBRIGATORIAMENTE em "normalized device coordinates" (NDC), onde cada
    // coeficiente estará entre -1 e 1 após divisão por w.
    // Veja {+NDC2+}.
    //
    // O código em "main.cpp" define os vértices dos modelos em coordenadas
    // locais de cada modelo (array model_coefficients). Abaixo, utilizamos
    // operações de modelagem, definição da câmera, e projeção, para computar
    // as coordenadas finais em NDC (variável gl_Position). Após a execução
    // deste Vertex Shader, a placa de vídeo (GPU) fará a divisão por W. Veja
    // slides 41-67 e 69-86 do documento Aula_09_Projecoes.pdf.

    gl_Position = projection * view * model * model_coefficients;

    // Como as variáveis acima  (tipo vec4) são vetores com 4 coeficientes,
    // também é possível acessar e modificar cada coeficiente de maneira
    // independente. Esses são indexados pelos nomes x, y, z, e w (nessa
    // ordem, isto é, 'x' é o primeiro coeficiente, 'y' é o segundo, ...):
    //
    //     gl_Position.x = model_coefficients.x;
    //     gl_Position.y = model_coefficients.y;
    //     gl_Position.z = model_coefficients.z;
    //     gl_Position.w = model_coefficients.w;
    //

    // Agora definimos outros atributos dos vértices que serão interpolados pelo
    // rasterizador para gerar atributos únicos para cada fragmento gerado.

    // Posição do vértice atual no sistema de coordenadas global (World).
    position_world = model * model_coefficients;

    // Posição do vértice atual no sistema de coordenadas local do modelo.
    position_model = model_coefficients;

    // Normal do vértice atual no sistema de coordenadas global (World).
    // Veja slides 123-151 do documento Aula_07_Transformacoes_Geometricas_3D.pdf.
    normal = inverse(transpose(model)) * normal_coefficients;
    normal.w = 0.0;

    // Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
    texcoords = texture_coefficients;


    /*

        Checa se realizamos interpolação de Gouraud.

    */
    if(interp_Gouraud){
        float U = 0.0;
        float V = 0.0;
        if ( object_type == GENERIC_OBJECT){
            U = texcoords.x;
            V = texcoords.y;
        }
        else if ( object_type == SPHERICAL_OBJECT){
            vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
            vec4 p_vec = position_model - bbox_center;
            vec4 p_hat = bbox_center + (p_vec / length(p_vec));
            
            float phi = asin(p_hat[1]);
            float theta = atan(p_hat[0],p_hat[2]);
            U = (theta + M_PI) / (2 * M_PI);
            V = (phi + M_PI_2) / M_PI;
        }
        vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 camera_position = inverse(view) * origin;
        vec4 p = position_world;
        vec4 n = normalize(normal);    
        vec4 v = normalize(camera_position - p);
        vec4 l = light_position - p;
        
        const float epsilon = 1e-5;    
        float cos_beta = dot(normalize(light_direction), normalize(p - light_position));
        float cos_alpha = cos(light_aperture_angle);

        vec3 Kd = Kd0 * texture(diffMap, vec2(U,V) * object_texture_scale).rgb;
        vec3 Ka = Ka0;
        vec3 Ks = Ks0; // Refletância especular do objeto
        float q = ns; // Expoente especular para o modelo de iluminação de Phong
        vec3 final_color;
        
        vec3 I = vec3(1.0,1.0,1.0); 
        
        vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l)); 
        vec3 lambert_shading = lambert_diffuse_term;
        vec3 Ia = vec3(0.2,0.2,0.2);
        vec3 ambient_term = Ka * Ia; 
        vec4 r = -l + 2 * n * dot(n, l);
        vec3 phong_specular_term  = Ks * I * pow(max(0, dot(r, v)), q); 
        vec3 phong_shading = lambert_diffuse_term + ambient_term + phong_specular_term;
        vec4 h = normalize(v + l);
        vec3 blinn_phong_specular_term = Ks * I * pow(max(0, dot(n, h)), q);
        vec3 blinn_phong_shading = lambert_diffuse_term + ambient_term + blinn_phong_specular_term;
        
        if (cos_beta < cos_alpha){
            color_interp.rgb = ambient_term;
        }
        else{
            switch(illumination_model){
                case LAMBERT:
                    color_interp.rgb = lambert_shading;
                    break;
                case PHONG:
                    color_interp.rgb = phong_shading;
                    break;
                case BLINN_PHONG:
                    color_interp.rgb = blinn_phong_shading;
                    break;  
            }
        }
        color_interp.a = 1;
        color_interp.rgb = pow(color_interp.rgb, vec3(1.0,1.0,1.0)/2.2);
    }









}

