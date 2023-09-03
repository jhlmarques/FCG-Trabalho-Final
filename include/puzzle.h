#ifndef __PUZZLE_H__
#define __PUZZLE_H__

#include "room.h"
#include <vector>
#include <map>
#include <string>
#include "gameobject.h"
#include "animation.h"
#include "collisions.h"
#include <random>

#define PUZZLE_ID_LOCK 254
#define PUZZLE_ID_NONE 255

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
#define FRAME_SIZE 1.5f
#define FRAME_LATERAL_OFFSET 0.2f
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
    // Lida com os puzzles estarem completos
    void handlePuzzlesCompleted();

};

#define LOCK_PUZZLE_RING_ANSWER1 9 
#define LOCK_PUZZLE_RING_ANSWER2 4
#define LOCK_PUZZLE_RING_ANSWER3 1
class LockPuzzle : public Puzzle{
    private:
    uint8_t lock_ring_val1;
    uint8_t lock_ring_val2;
    uint8_t lock_ring_val3;
    
    public:
    void setupRoom();
    void updateState();    
};


class CratePuzzle : public Puzzle
{
    public:
    void updateCamera();
    void setupRoom();
    void updateState();

    void handleCursorMovement(float dx, float dy);
};

class GnomePuzzle : public Puzzle
{
    private: 
    // Move o gnomo utilizando as setas do teclado
    void moveGnome();
    void checkColisions();
    float prev_time;
    
    int gnomeJumpAnimationID;
    int numberCanvasAnimationID;
    int gnomeRotateAnimationID;

    int actual_num_gnomes;
    float speed;

    bool already_ended;

    // Animação que mostra o número
    void showNumber();
    // Aumenta a velocidade do gnomo
    void increaseSpeed();

    // Constantes da classe
    const glm::vec4 camera_offset = glm::vec4(0.0f, 0.25f, -1.0f, 0.0f);
    const glm::vec4 gnome_initial_position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    const float floor_size = 20.0f;
    const float collision_gnome_size = 0.5f;
    const float collision_gnome_offset = -1.5f;
    const int max_gnomes = 10;
    const float speed_increase_rate = 0.005f;
    const float jump_height = collision_gnome_size + 0.01f;
    const float jump_distance = std::abs(collision_gnome_offset)/2.0f;
    const float control_point_disloc = jump_distance/2.0f;
    const float max_speed = 2.0f;
    const float max_distance = floor_size - 0.25f;

    const glm::vec4 number_canvas_start_position = glm::vec4(-floor_size, 5.0f, 0.0f, 1.0f);
    const glm::vec4 number_canvas_offset_from_gnome = glm::vec4(-0.5, 0.5f, 0.0f, 0.0f);

    const float canvas_drop_velocity = 2.0f;
    const float gnome_rotate_velocity = 20.0f;


    public:
    void updateCamera();
    void setupRoom();   
    void updateState();

    GnomePuzzle();
};

class BallPuzzle : public Puzzle{

    private:
    // Objetos relevantes a colisão
    GameObject* player;
    GameObject* ball;
    GameObject* ball_puzzle; // A bola com a resposta
    GameObject* cur_ball;

    // Posição atual
    uint8_t curPos;
    
    // Round atual
    int round;

    // IDs de animações
    int ballMovementAnimationID;
    int playerMovementAnimationID;
    
    public:
    void setupRoom();
    void updateState();
    // Movimenta jogador (em círculo)
    void movePlayer();


};

#endif // __PUZZLE_H__
