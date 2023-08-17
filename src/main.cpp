#include "labfunctions.h"
#include "camera.h"
#include "tile.h"
#include "globals.h"

#define MAIN_ROOM 0
#define TILE_FLOOR  1
#define GENERIC_OBJECT 2

// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

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


// Carrega um arquivo de textura e retorna o id da textura
Texture::Texture(const char* filename)
{
    printf("Carregando imagem \"%s\"... ", filename);

    stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int channels;
    
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 3);

    if ( data == NULL )
    {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }

    printf("OK (%dx%d).\n", width, height);

    // Agora criamos objetos na GPU com OpenGL para armazenar a textura
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    // Parâmetros de sampling
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
}

Texture::~Texture(){
    // Removemos nossa textura da GPU
    glDeleteTextures(1, &texture_id);
    glDeleteSamplers(1, &sampler_id);
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
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindSampler(bound_unit, 0);
}









int main(int argc, char* argv[])
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impressão de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
    // funções modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 800 colunas e 600 linhas
    // de pixels, e com título "INF01047 ...".
    GLFWwindow* window;
    window = glfwCreateWindow(800, 600, "FOGE FOGE", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos a função de callback que será chamada sempre que o usuário
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os botões do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, 800, 600); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    // Imprimimos no terminal informações sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);



    /*
    
        MODIFICAÇÕES A PARTIR DAQUI
    
    
    
    */

    // Criação do programa de GPU
    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");
    // Deletamos o programa de GPU anterior, caso ele exista.
    if ( g_GpuProgramID != 0 )
        glDeleteProgram(g_GpuProgramID);
    g_GpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    // Configurações das variáveis utilizadas no shader

    // Transformações
    g_model_uniform      = glGetUniformLocation(g_GpuProgramID, "model"); // Variável da matriz "model"
    g_view_uniform       = glGetUniformLocation(g_GpuProgramID, "view"); // Variável da matriz "view" em shader_vertex.glsl
    g_projection_uniform = glGetUniformLocation(g_GpuProgramID, "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    
    // Informações do objeto
    g_object_id_uniform  = glGetUniformLocation(g_GpuProgramID, "object_id"); // Variável "object_id" em shader_fragment.glsl
    g_bbox_min_uniform   = glGetUniformLocation(g_GpuProgramID, "bbox_min");
    g_bbox_max_uniform   = glGetUniformLocation(g_GpuProgramID, "bbox_max");

    // Alteração de valores
    glUseProgram(g_GpuProgramID);

    // Configuração dos samplers a texture units
    // Carregamos texturas para dentro do programa ao fazer
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "diffMap"), 0);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "normalMap"), 1);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "AOMap"), 2);
    
    glUseProgram(0);

    // Texturas
    // Imediatamente carregadas à GPU
    // Precisamos fazer bind() quando queremos utilizá-las
    Texture floorTexture("../../data/floor_texture.jpg");
    Texture wallTexture("../../data/wall_texture.jpg");

    // Objetos

    ObjModel tile_floor("../../data/plane.obj");
    ComputeNormals(&tile_floor);
    BuildTrianglesAndAddToVirtualScene(&tile_floor);

    ObjModel bunny("../../data/bunny.obj");
    ComputeNormals(&bunny);
    BuildTrianglesAndAddToVirtualScene(&bunny);

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento Aula_14_Laboratorio_3_Revisao.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    float prev_time = (float)glfwGetTime();

    /*
    
        PLACEHOLDER/TESTAGEM
    
    */

    // Vetor estático de todos tiles existentes
    std::vector<Tile> tileVector;
    tileVector.emplace_back(0.0f, 0.0f, 0.0f);
    tileVector.emplace_back(0.0f, 0.0f, TILE_WIDTH); // n
    tileVector.emplace_back(TILE_WIDTH, 0.0f, 0.0f); // w
    tileVector.emplace_back(0.0f, 0.0f, -TILE_WIDTH);// s
    tileVector.emplace_back(-TILE_WIDTH, 0.0f, 0.0f); // e

    tileVector.emplace_back(TILE_WIDTH, 0.0f, TILE_WIDTH); // NW
    tileVector.emplace_back(TILE_WIDTH, 0.0f, -TILE_WIDTH); // SW 
    tileVector.emplace_back(-TILE_WIDTH, 0.0f, -TILE_WIDTH); // SE
    tileVector.emplace_back(-TILE_WIDTH, 0.0f, TILE_WIDTH); // NE

    tileVector[0].setNorth(&tileVector[1]);
    tileVector[0].setWest(&tileVector[2]);
    tileVector[0].setSouth(&tileVector[3]);
    tileVector[0].setEast(&tileVector[4]);

    tileVector[1].setWest(&tileVector[5]);
    tileVector[2].setNorth(&tileVector[5]);

    tileVector[3].setWest(&tileVector[6]);
    tileVector[2].setSouth(&tileVector[6]);

    tileVector[4].setSouth(&tileVector[7]);
    tileVector[3].setEast(&tileVector[7]);

    tileVector[1].setEast(&tileVector[8]);
    tileVector[4].setNorth(&tileVector[8]);

    // Assumindo que o "plano" tem distância do centro ao lado entre 0 e 1
    tileVector[1].addObject("the_bunny", glm::vec4(-0.7f, 1.0f, 0.0f, 1.0f));
    tileVector[2].addObject("the_bunny", glm::vec4(-0.3f, 1.0f, 0.2f, 1.0f));


    Tile* cur_tile = &tileVector[0];

    // Move câmera para esse ponto
    auto tileCenter = cur_tile->getCenterPos();
    tileCenter.y += CAMERA_HEAD_HEIGHT;

    mainCamera.setPosition(tileCenter);
    mainCamera.setViewVector(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));



    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {
        // Aqui executamos as operações de renderização

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor é
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto é:
        // Vermelho, Verde, Azul, Alpha (valor de transparência).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
        //
        //           R     G     B     A
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e também resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vértice e fragmentos).
        glUseProgram(g_GpuProgramID);
        
        // Apenas realizamos um movimento se a câmera não está animando
        if(!mainCamera.animate()){
            cur_tile->handleMovement(&cur_tile);
        }

        glm::mat4 const& view = mainCamera.getViewMatrix();

        // Agora computamos a matriz de Projeção.
        glm::mat4 projection;

        // Note que, no sistema de coordenadas da câmera, os planos near e far
        // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
        float nearplane = -0.1f;  // Posição do "near plane"
        float farplane  = -100.0f; // Posição do "far plane"




        // Projeção Perspectiva.
        // Para definição do field of view (FOV), veja slides 205-215 do documento Aula_09_Projecoes.pdf.
        float field_of_view = 3.141592 / 3.0f;
        projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);

        glm::mat4 model = Matrix_Identity(); // Transformação identidade de modelagem

        // Enviamos as matrizes "view" e "projection" para a placa de vídeo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));

        // Desenhamos a sala principal

        model = Matrix_Identity();

//        glLineWidth(4.0f);

        // DESENHA SALA PRINCIPAL
        // glCullFace(GL_FRONT);
        // glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        // glUniform1i(g_object_id_uniform, MAIN_ROOM);
        // DrawVirtualObject("the_main_room");
        // glCullFace(GL_BACK);

        // DESENHA TILES
        for(auto tile : tileVector){
            auto coords = tile.getCenterPos();
            auto scale = Matrix_Scale(TILE_WIDTH/2.0f, 1.0f, TILE_WIDTH/2.0f);
            
            // Escala para o tamanho do tile para cobrir espaçamento entre tiles e deslocamento para o centro do tile
            model = Matrix_Translate(coords.x, coords.y, coords.z) * scale;

            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, TILE_FLOOR);
            
            // Desenhamos usando a textura do chão
            floorTexture.bind(0);
            DrawVirtualObject("the_plane");
            floorTexture.unbind();

            // Desenha objetos
            for (auto obj : tile.getObjects()){
                // Ajusta coordenadas de acordo com o esticamento do plano
                // (as coordenadas do objeto no plano assumem que o plano está em escala 1x,
                // logo precisamos escalar esse deslocamento de acordo com o plano)
                auto obj_coords = ((obj.positionInTile * scale) + coords);
                model = Matrix_Translate(obj_coords.x, obj_coords.y, obj_coords.z);
                glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
                glUniform1i(g_object_id_uniform, GENERIC_OBJECT); // TO-DO: ARRUMAR
                
                // Desenhamos usando textura de parede
                wallTexture.bind(0);
                DrawVirtualObject(obj.obj_str);
                wallTexture.unbind();
            }

        }

        // O framebuffer onde OpenGL executa as operações de renderização não
        // é o mesmo que está sendo mostrado para o usuário, caso contrário
        // seria possível ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuário
        // tudo que foi renderizado pelas funções acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma interação do
        // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
        // definidas anteriormente usando glfwSet*Callback() serão chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
    }

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}

// Variáveis globais que armazenam a última posição do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
double g_LastCursorPosX, g_LastCursorPosY;

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_LeftMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Calculamos o deslocamento horizontal (dx) e vertical (dy) do ponteiro do
    // mouse, em COORDENADAS DE TELA (pixels), desde a última chamada à função
    // CursorPosCallback().

    if (!g_LeftMouseButtonPressed)
        return;

    float dx = xpos - g_LastCursorPosX;
    float dy = ypos - g_LastCursorPosY;

    // Mapeamos o deslocamento horizontal do mouse para uma variação no ângulo
    // theta, e o deslocamento vertical para uma variação no ângulo phi, que
    // irão definir o view vector. O valor 0.003 é um fator de escalamento
    // escolhido empiricamente, que define a conversão entre coordenadas de TELA
    // para radianos. Este fator controla a "velocidade" de rotação da câmera de
    // acordo com a movimentação do mouse (a "sensitividade" do mouse).
    g_CameraTheta -= 0.003f*dx;
    g_CameraPhi -= 0.003f*dy;

    // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
    float phimax = 3.141592f/2;
    float phimin = -phimax;

    if (g_CameraPhi > phimax)
        g_CameraPhi = phimax;

    if (g_CameraPhi < phimin)
        g_CameraPhi = phimin;

    // Atualizamos as variáveis globais para armazenar a posição atual do
    // mouse como sendo a última posição conhecida do mouse.
    g_LastCursorPosX = xpos;
    g_LastCursorPosY = ypos;
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // ==================
    // Não modifique este loop! Ele é utilizando para correção automatizada dos
    // laboratórios. Deve ser sempre o primeiro comando desta função KeyCallback().
    for (int i = 0; i < 10; ++i)
        if (key == GLFW_KEY_0 + i && action == GLFW_PRESS && mod == GLFW_MOD_SHIFT)
            std::exit(100 + i);
    // ==================

    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_W)
    {
        if (action == GLFW_PRESS)
            g_wPressed = true;
        else if (action == GLFW_RELEASE)
            g_wPressed = false;
        else if (action == GLFW_REPEAT)
            ;
    }
    if (key == GLFW_KEY_S)
    {
        if (action == GLFW_PRESS)
            g_sPressed = true;
        else if (action == GLFW_RELEASE)
            g_sPressed = false;
        else if (action == GLFW_REPEAT)
            ;
    }
    if (key == GLFW_KEY_A)
    {
        if (action == GLFW_PRESS)
            g_aPressed = true;
        else if (action == GLFW_RELEASE)
            g_aPressed = false;
        else if (action == GLFW_REPEAT)
            ;
    }
    if (key == GLFW_KEY_D)
    {
        if (action == GLFW_PRESS)
            g_dPressed = true;
        else if (action == GLFW_RELEASE)
            g_dPressed = false;
        else if (action == GLFW_REPEAT)
            ;
    }

}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(yoffset > 0.0){
        g_scrolledDirection = SCROLL_UP;
    }
    else if(yoffset < -0.0){
        g_scrolledDirection = SCROLL_DOWN;
    }
    
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :

