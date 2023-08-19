#include "labfunctions.h"
#include "camera.h"
#include "tile.h"
#include "globals.h"
#include "texture.h"
#include "gameobject.h"
#include <iostream>

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

// Funções para verificar qual é a sala atual
bool isCurrentRoomLobby();
bool isCurrentRoomCratePuzzle();
bool isCurrentRoomChairPuzzle();

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

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento Aula_14_Laboratorio_3_Revisao.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);


    /*
    
        MODIFICAÇÕES A PARTIR DAQUI
    
    
    
    */

    LoadShadersFromFiles();
    // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
    // os shaders de vértice e fragmentos).
    glUseProgram(g_GpuProgramID);

    Texture::setTextureFolderRoot("../../data/");

    // Carregamento de modelos de objetos

    ObjModel model_tile_floor("../../data/plane.obj", "../../data/");
    GameObject obj_tile(&model_tile_floor, 0);

    ObjModel model_bunny("../../data/bunny.obj", "../../data/");
    GameObject obj_bunny(&model_bunny, 0);

    //ObjModel model_chair("../../data/modern_arm_chair_01_1k.obj", "../../data/");
    ObjModel model_chair("../../data/marble_bust_01_1k.obj", "../../data/");
    GameObject obj_chair(&model_chair, 0);

    ObjModel model_wooden_crate_9("../../data/wooden_crate_02_4k.obj", "../../data/");
    ComputeNormals(&model_wooden_crate_9);
    GameObject obj_crate_9(&model_wooden_crate_9, 0);
    

    float prev_time = (float)glfwGetTime();
    // Note que, no sistema de coordenadas da câmera, os planos near e far
    // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
    float nearplane = -0.1f;  // Posição do "near plane"
    float farplane  = -100.0f; // Posição do "far plane"
    // Projeção Perspectiva.
    float field_of_view = 3.141592 / 3.0f;


    /*
        SETUP DO LOBBY PRINCIPAL
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

    tileVector[1].addObject(&obj_bunny);
    tileVector[2].addObject(&obj_bunny);
    tileVector[3].addObject(&obj_chair);

    Tile* cur_tile = &tileVector[0];

    // Obtém a posição inicial da câmera 
    auto tileCenter = cur_tile->getCenterPos();
    tileCenter.y += CAMERA_HEAD_HEIGHT;

    //  Câmera do lobby principal (inicialmente em modo "free")
    // O vetor view padrão é calculado por phi=0 theta=0 dist=epsilon, e deve ser atualizado de acordo
    Camera mainCamera(tileCenter);
    
    // Iluminação do lobby principal
    glm::vec4 mainCameraLightPosition = glm::vec4(TILE_WIDTH,5.0f,0.0f,1.0f);
    float mainCameraLightApertureAngle = M_PI; // Ilumina toda a sala
    glm::vec4 mainCameraLightDirection = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Convenção pra representar uma fonte de luz pontual (é descartado no shader)

    /*
        SETUP DO PUZZLE DA CAIXA DE MADEIRA
    */

    // Instanciação da câmera de lookat puzzle que aponta para a origem 
    Camera cratePuzzleCamera(glm::vec3(2.0f, 2.0f, 1.5f));
    glm::vec4 cratePuzzleLookatPoint = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    cratePuzzleCamera.setLookAtPoint(cratePuzzleLookatPoint);

    glm::vec4 cratePuzzleLightPosition = cratePuzzleCamera.getPosition();
    glm::vec4 cratePuzzleLightDirection = cratePuzzleCamera.getViewVec();
    float cratePuzzleLightApertureAngle = M_PI/8.0f;
    
    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {
        
        // Somente utilizamos a matriz de projeção em perspectiva
        glm::mat4 projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);
        glUniformMatrix4fv(g_projection_uniform, 1 , GL_FALSE , glm::value_ptr(projection));
        
        // Define qual a câmera que será utilizada
        switch (g_lastNumberPressed){
            case GLFW_KEY_1:
                g_currentCamera = &cratePuzzleCamera;
                break;
            default:
                g_currentCamera = &mainCamera;
                break;
        }
        /*
        
            LOBBY PRINCIPAL

            Uma sala com vários puzzles. O jogador se movimenta entre tiles, áreas
            análogas aos espaços de um tabuleiro, e pode interagir com objetos que estão dentro
            do seu tile.
        
        */
        if (isCurrentRoomLobby()){

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Apenas realizamos um movimento se a câmera não está animando
            if(!mainCamera.animate()){
                cur_tile->handleMovement(&cur_tile, mainCamera);
            }

            // Transformação da câmera
            glm::mat4 const& view = mainCamera.getViewMatrix();
            glUniformMatrix4fv(g_view_uniform, 1 , GL_FALSE , glm::value_ptr(view));


            glm::mat4 model = Matrix_Identity(); 

            // DESENHA TILES
            for(auto tile : tileVector){
                auto coords = tile.getCenterPos();
                auto scale = Matrix_Scale(TILE_WIDTH/2.0f, 1.0f, TILE_WIDTH/2.0f);
                
                // Escala para o tamanho do tile para cobrir espaçamento entre tiles e deslocamento para o centro do tile
                model = Matrix_Translate(coords.x, coords.y, coords.z) * scale;
                glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
                obj_tile.draw(mainCameraLightPosition, mainCameraLightDirection, mainCameraLightApertureAngle);
            }

            // DESENHA OBJETOS
            // TO-DO: Usar uma classe que represente um "interativo" - Um modelo 3D junto
            // de um hitbox, possivelmente com alguma lógica interna

            // Uma cadeira no tile norte
            auto pos = tileVector[1].getCenterPos();
            model =  Matrix_Translate(pos.x, pos.y, pos.z) * Matrix_Rotate_Y(M_PI) * Matrix_Scale(4.0f, 4.0f, 4.0f);
            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            obj_chair.draw(mainCameraLightPosition, mainCameraLightDirection, mainCameraLightApertureAngle);
        /*
        

            PRIMEIRO PUZZLE - Usuário precisa encontrar caixa com número 9
        
        
        */
        
        }
        else if (isCurrentRoomCratePuzzle()){

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Recalcula a posição da câmera de acordo com o clique do mouse do usuário
            glm::mat4 model = Matrix_Identity();
            model = Matrix_Translate(cratePuzzleLookatPoint.x, cratePuzzleLookatPoint.y, cratePuzzleLookatPoint.z) * 
                    Matrix_Scale(4.0f, 4.0f, 4.0f);
            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));

            glm::mat4 const& view = cratePuzzleCamera.getViewMatrix();

            // Enviamos as matrizes "view" para a placa de vídeo
            glUniformMatrix4fv(g_view_uniform, 1 , GL_FALSE , glm::value_ptr(view));

            // Atualiza a fonte de luz conforme a posição da câmera, a fim de fazer o efeito como se fosse uma lanterna
            cratePuzzleLightPosition = cratePuzzleCamera.getPosition();
            cratePuzzleLightDirection = cratePuzzleCamera.getViewVec();
            obj_crate_9.draw(cratePuzzleLightPosition, cratePuzzleLightDirection, cratePuzzleLightApertureAngle);
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

bool isCurrentRoomLobby(){
    return g_lastNumberPressed == GLFW_KEY_0 || g_lastNumberPressed == GLFW_KEY_UNKNOWN;
}

bool isCurrentRoomCratePuzzle(){
    return g_lastNumberPressed == GLFW_KEY_1;
}

bool isCurrentRoomChairPuzzle(){
    return g_lastNumberPressed == GLFW_KEY_2;
}


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

    if (!g_LeftMouseButtonPressed || isCurrentRoomLobby())
        return;

    float dx = xpos - g_LastCursorPosX;
    float dy = ypos - g_LastCursorPosY;

    // Mapeamos o deslocamento horizontal do mouse para uma variação no ângulo
    // theta, e o deslocamento vertical para uma variação no ângulo phi, que
    // irão definir o view vector. O valor 0.003 é um fator de escalamento
    // escolhido empiricamente, que define a conversão entre coordenadas de TELA
    // para radianos. Este fator controla a "velocidade" de rotação da câmera de
    // acordo com a movimentação do mouse (a "sensitividade" do mouse).
    float currentTheta = g_currentCamera->getCameraTheta();
    float currentPhi = g_currentCamera->getCameraPhi();

    currentTheta -= 0.003f*dx;
    currentPhi += 0.003f*dy;

    g_currentCamera->setCameraTheta(currentTheta);
    g_currentCamera->setCameraPhi(currentPhi);
    g_currentCamera->updateViewVecLookAt();

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

    if (key == GLFW_KEY_0){
        g_lastNumberPressed = GLFW_KEY_0;
    }
    else if (key == GLFW_KEY_1)
    {
        g_lastNumberPressed = GLFW_KEY_1;
    }
    else if (key == GLFW_KEY_2)
    {
        g_lastNumberPressed = GLFW_KEY_2;
    }
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (isCurrentRoomLobby()){
        if(yoffset > 0.0){
            g_scrolledDirection = SCROLL_UP;
        }
        else if(yoffset < -0.0){
            g_scrolledDirection = SCROLL_DOWN;
        }
    }
    else if (isCurrentRoomChairPuzzle()){
         // Atualizamos a distância da câmera para a origem utilizando a
        // movimentação da "rodinha", simulando um ZOOM.
        float currentCameraDistance = g_currentCamera->getCameraDistance();
        currentCameraDistance -= 0.1f*yoffset;

        g_currentCamera->setCameraDistance(currentCameraDistance);
        g_currentCamera->updateViewVecLookAt();
    }
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :