/*

    Um tile ("quadrado") do mapa do jogo. Contém objetos com os quais
    o jogador pode interagir. Tiles realizam a interpretação dos inputs de movimento do jogador 

*/

#ifndef __TILE_H__
#define __TILE_H__

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

#include "camera.h"
#include "globals.h"

// forward declare
class Tile;

class Tile{

    private:

    // Posição na cena
    glm::vec4 centerPos;

    // Tiles vizinhos, com nomes relativos às posições dos vizinhos da perspectiva inicial do jogo
    Tile* north = nullptr;
    Tile* south = nullptr;
    Tile* east  = nullptr;
    Tile* west  = nullptr;

    // Tempo desde o último input processado
    std::chrono::steady_clock::time_point lastProcessedInput;

    public:

    Tile(float px, float py, float pz) :
    centerPos(px, py, pz, 1.0f)
    {}

    void setNorth(Tile* t);
    void setSouth(Tile* t);
    void setEast(Tile* t);
    void setWest(Tile* t);

    glm::vec4 getCenterPos();

    // Realiza movimento
    void handleMovement();


};

#endif // __TILE_H__

