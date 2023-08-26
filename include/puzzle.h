#ifndef __PUZZLE_H__
#define __PUZZLE_H__

#include "room.h"
#include <vector>
#include <map>
#include <string>
#include "gameobject.h"

class Puzzle;

class Puzzle{

    private:
    std::map<std::string, GameObject*> objects;


    protected:
    Room room;

    float currentTheta = 0.0f;
    float currentPhi = 0.0f;

    // Atualiza câmera
    virtual void updateCamera();
    // Configura sala
    virtual void setupRoom() = 0;
    // Lida com inputs
    virtual void updateState() = 0;
    // Desenha objetos do puzzle
    virtual void drawObjects() = 0;

    public:

    void step(); // Realiza atualizações e retorna puzzle 

    // Adiciona objeto
    void addObject(std::string name, GameObject* obj);

    // Obtém objeto
    GameObject* getObject(std::string obj_name);

    // Movimento do cursor
    virtual void handleCursorMovement(float dx, float dy);

    // Scroll
    virtual void handleScroll(double xoffset, double yoffset);


};

#define CAMERA_HEAD_HEIGHT 5.0f //Valor somado a y para representar a altura da cabeça do personagem
#define STEP_SIZE 12.0f //Valor do deslocamento
#define ROOM_SIDE_WIDTH 1.0f // Máximo de unidades de deslocamento para o lado
#define ROOM_WIDTH 3.0f // Máximo de espaços no eixo horizontal (sempre impar, considerando que deve sempre haver um centro, esquerda e direita)
#define ROOM_LENGTH 4.0f // Máximo de unidades de deslocamento para a frente 
#define ROOM_HEIGHT 12.0f

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

class MainLobby : public Puzzle{
    private:

    glm::vec4 playerPosition; // A posição atual do jogador
    int8_t curFacingDirection;
    bool enteredPuzzle;

public:
    MainLobby();
    void setupRoom();
    void updateState();
    void drawObjects();

    // Verdadeiro se entra
    bool hasEnteredPuzzle();
    // Tenta realizar um movimento
    void playerMove();
    // Retorna o ID do puzzle atual (relativo aos quadrados)
    uint8_t getCurrentPuzzleID();
    // Lida com a saída de um puzzle
    void handleExitedPuzzle();

};

class CratePuzzle : public Puzzle
{
public:
    void updateCamera();
    void setupRoom();
    void updateState();
    void drawObjects();

    void handleCursorMovement(float dx, float dy);
    void handleScroll(double xoffset, double yoffset);
};

#endif // __PUZZLE_H__
