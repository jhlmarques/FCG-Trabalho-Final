#ifndef __GLOBALS_H__
#define __GLOBALS_H__

/*

    VARIÁVEIS GLOBAIS, DEFINIDAS EM GLOBALS.CPP

*/

#include <map>
#include <stack>
#include <glad/glad.h>

#include "labstructures.h"


extern std::map<std::string, SceneObject> g_VirtualScene;
extern std::stack<glm::mat4>  g_MatrixStack;
extern float g_ScreenRatio;

extern bool g_LeftMouseButtonPressed;
extern bool g_RightMouseButtonPressed;
extern bool g_MiddleMouseButtonPressed;

extern GLuint g_GpuProgramID;
extern GLint g_model_uniform;
extern GLint g_view_uniform;
extern GLint g_projection_uniform;
extern GLint g_object_texture_scale;
extern GLint g_object_type_uniform;
extern GLint g_bbox_min_uniform;
extern GLint g_bbox_max_uniform;
extern GLfloat g_diffMap_uniform;
extern GLfloat g_ambientMap_uniform;
extern GLfloat g_specularMap_uniform;

extern GLuint g_NumLoadedTextures;

extern bool g_wPressed;
extern bool g_aPressed;
extern bool g_sPressed;
extern bool g_dPressed;

extern bool g_upPressed;
extern bool g_downPressed;
extern bool g_leftPressed;
extern bool g_rightPressed;

extern int g_lastNumberPressed;

enum ScrollDirection{
    SCROLL_NONE,
    SCROLL_UP,
    SCROLL_DOWN
};

extern ScrollDirection g_scrolledDirection;

extern std::map<std::string, ObjModel*> g_mapModels;

#endif // __GLOBALS_H__
