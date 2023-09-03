#include "globals.h"

// Abaixo definimos variáveis globais utilizadas em várias funções do código.

// A cena virtual é uma lista de objetos nomeados, guardados em um dicionário
// (map).  Veja dentro da função BuildTrianglesAndAddToVirtualScene() como que são incluídos
// objetos dentro da variável g_VirtualScene, e veja na função main() como
// estes são acessados.
std::map<std::string, SceneObject> g_VirtualScene;

// Pilha que guardará as matrizes de modelagem.
std::stack<glm::mat4>  g_MatrixStack;

// Razão de proporção da janela (largura/altura). Veja função FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;

// "g_LeftMouseButtonPressed = true" se o usuário está com o botão esquerdo do mouse
// pressionado no momento atual. Veja função MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false; // Análogo para botão direito do mouse
bool g_MiddleMouseButtonPressed = false; // Análogo para botão do meio do mouse

// Variáveis que definem um programa de GPU (shaders). Veja função LoadShadersFromFiles().
GLuint g_GpuProgramID = 0;
// Transformações
GLint g_model_uniform;
GLint g_view_uniform;
GLint g_projection_uniform;

// GameObject
GLint g_bbox_min_uniform;
GLint g_bbox_max_uniform;

// Samplers, os quais são associados a TEXTURE UNITs
GLfloat g_diffMap_uniform;

GLint g_object_texture_scale;
GLint g_object_type_uniform;

// Número de texturas carregadas pela função getCachedTextureImage()
GLuint g_NumLoadedTextures = 0;

// Variáveis que controlam o estado das teclas W, A, S, D.
bool g_wPressed = false;
bool g_aPressed = false;
bool g_sPressed = false;
bool g_dPressed = false;

// Variáveis que controlam as setas do teclado
bool g_upPressed = false;
bool g_downPressed = false;
bool g_leftPressed = false;
bool g_rightPressed = false;

// Controla o estado do último número apertado
int g_lastNumberPressed = GLFW_KEY_UNKNOWN;

// Direção (ou ausência de direção) de scroll do mouse
ScrollDirection g_scrolledDirection;

// Estado atual da movimentação da câmera
uint8_t g_movingCamera = 0;

// Mapa com todos modelos carregados
std::map<std::string, ObjModel*> g_mapModels;

// Define se o cadeado foi aberto
bool g_lockOpened = false;

// Define se os puzzles estão completos
bool g_puzzlesCompleted = false;
