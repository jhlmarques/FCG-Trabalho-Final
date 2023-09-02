#ifndef __PUZZLE_H__
#define __PUZZLE_H__

#include "room.h"
#include <vector>
#include <map>
#include <string>
#include "gameobject.h"
#include "animation.h"
#include "collisions.h"

class Puzzle;

class Puzzle{
    private:

    protected:
    Room room;

    float currentTheta = 0.0f;
    float currentPhi = 0.0f;

    std::map<std::string, GameObject*> objects;

    // Atualiza câmera
    virtual void updateCamera();
    // Configura sala
    virtual void setupRoom() = 0;
    // Lida com inputs
    virtual void updateState() = 0;
    // Desenha objetos do puzzle
    void drawObjects();

    public:

    void step(); // Realiza atualizações e retorna puzzle 

    // Movimento do cursor
    virtual void handleCursorMovement(float dx, float dy);

    // Scroll
    virtual void handleScroll(double xoffset, double yoffset);



};

#define CAMERA_HEAD_HEIGHT 1.4f //Valor somado a y para representar a altura da cabeça do personagem

#define STEP_SIZE 3.0f //Valor do deslocamento
#define LOBBY_SIDE_WIDTH 2.0f // Máximo de unidades de deslocamento para o lado
#define LOBBY_WIDTH 5.0f // Máximo de espaços no eixo horizontal (sempre impar, considerando que deve sempre haver um centro, esquerda e direita)
#define LOBBY_LENGTH 4.0f // Máximo de unidades de deslocamento para a frente 
#define LOBBY_HEIGHT 6.0f
#define FRAME_SIZE 4.5f
#define LOBBY_LIGHT_SOURCE_HEIGHT (LOBBY_HEIGHT)


#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3



class MainLobby : public Puzzle{
    private:

    glm::vec4 playerPosition; // A posição atual do jogador
    int8_t curFacingDirection;
    bool enteredPuzzle;

    // Animações
    int cameraAnimationID;

    public:
    MainLobby();
    void setupRoom();
    void updateState();

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

    void handleCursorMovement(float dx, float dy);
    void handleScroll(double xoffset, double yoffset);
};

#endif // __PUZZLE_H__

class GnomePuzzle : public Puzzle
{
    public:
    void updateCamera();
    void setupRoom();   
    void updateState();

    GnomePuzzle();

    private: 
    // Move o gnomo utilizando as setas do teclado
    void moveGnome();
    float prev_time;

    const glm::vec4 gnome_initial_position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
};

class FallingBallsPuzzle : public Puzzle{

    private:
    // O personagem controlado
    GameObject* player;
    // Espaços para se movimentar
    GameObject* tile1;
    GameObject* tile2;
    GameObject* tile3;
    GameObject* tile4;
    GameObject* marker;
    // Posição atual
    uint8_t curPos;
    
    // Round atual
    int round;

    // ID da animação de moviment
    int movementAnimationID;
    
    public:
    void setupRoom();
    void updateState();
    // Movimenta jogador (em círculo)
    void movePlayer();


};
