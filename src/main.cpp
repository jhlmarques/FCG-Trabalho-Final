#include "labfunctions.h"
#include "camera.h"
#include "globals.h"
#include "gameobject.h"

#include "puzzle.h"

// Controla qual o puzzle atual 
Puzzle* currentPuzzle = nullptr;

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

    ObjModel model_bust("../../data/marble_bust_01_1k.obj", "../../data/");
    GameObject obj_bust(&model_bust, 0);

    ObjModel model_light("../../data/lantern_chandelier_01_1k.obj", "../../data/");
    GameObject obj_light(&model_light, 0);

    ObjModel model_wooden_crate_9("../../data/wooden_crate_02_4k.obj", "../../data/");
    GameObject obj_crate_9(&model_wooden_crate_9, 0);

    ObjModel model_frame_crate_puzzle("../../data/crate_frame.obj", "../../data/");
    GameObject obj_frame_crate_puzzle(&model_frame_crate_puzzle, 0);
    ObjModel model_frame_crate_canvas_puzzle("../../data/crate_frame_canvas.obj", "../../data/");
    GameObject obj_canvas_crate_puzzle(&model_frame_crate_canvas_puzzle, 0);
    obj_frame_crate_puzzle.setIlluminationModel(LAMBERT);
    obj_canvas_crate_puzzle.setIlluminationModel(LAMBERT);
    
    // Note que, no sistema de coordenadas da câmera, os planos near e far
    // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
    float nearplane = -0.1f;  // Posição do "near plane"
    float farplane  = -100.0f; // Posição do "far plane"
    // Projeção Perspectiva.
    float field_of_view = 3.141592 / 3.0f;


    /*
        SETUP DO LOBBY PRINCIPAL
    */

    MainLobby puzzle_lobby;
    puzzle_lobby.setupRoom();
    puzzle_lobby.addObject("tile", &obj_tile);
    puzzle_lobby.addObject("statue", &obj_bust);
    puzzle_lobby.addObject("light", &obj_light);
    puzzle_lobby.addObject("crateFrame", &obj_frame_crate_puzzle);
    puzzle_lobby.addObject("crateCanvas", &obj_canvas_crate_puzzle);

    /*
        SETUP DO PUZZLE DA CAIXA DE MADEIRA
    */
    CratePuzzle puzzle_crate;
    puzzle_crate.setupRoom();
    puzzle_crate.addObject("crate", &obj_crate_9);
    
    // Define se estamos no lobby principal ou num puzzle
    bool isInLobby = true;

    currentPuzzle = &puzzle_crate;
    
    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {
        // Somente utilizamos a matriz de projeção em perspectiva
        glm::mat4 projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);
        glUniformMatrix4fv(g_projection_uniform, 1 , GL_FALSE , glm::value_ptr(projection));
        
        // Saída do puzzle (placeholder)
        if(!isInLobby && g_sPressed){
            isInLobby = true;
            puzzle_lobby.handleExitedPuzzle();
        }

        if(isInLobby){
            if(puzzle_lobby.hasEnteredPuzzle()){
                uint8_t id = puzzle_lobby.getCurrentPuzzleID();
                isInLobby = false;
                // Define novo puzzle baseado no ID
                switch (id){
                    case 0:
                        currentPuzzle = &puzzle_crate;
                        break;
                    default:
                        currentPuzzle = &puzzle_crate;
                        break;
                }

            }
            else{
                puzzle_lobby.step();
            }
        }
        else{
            currentPuzzle->step();
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
    currentPuzzle->handleCursorMovement(dx, dy);

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
        // g_lastNumberPressed = GLFW_KEY_2;
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

    currentPuzzle->handleScroll(xoffset, yoffset);
}

// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :