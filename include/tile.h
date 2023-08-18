/*

    Um tile ("quadrado") do mapa do jogo. Contém objetos com os quais
    o jogador pode interagir. Tiles realizam a interpretação dos inputs de movimento do jogador 

*/

#ifndef __TILE_H__
#define __TILE_H__

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "camera.h"
#include "globals.h"
#include "gameobject.h"

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define TILE_WIDTH 12.0f


// forward declare
class Tile;

class Tile{

    private:

    // Posição na cena, em coordenadas globais
    glm::vec4 centerPos;

    // Tiles vizinhos, com nomes relativos às posições dos vizinhos da perspectiva inicial do jogo
    Tile* north = nullptr;
    Tile* south = nullptr;
    Tile* east  = nullptr;
    Tile* west  = nullptr;

    // Tile para o qual estamos olhando atualmente
    int8_t curFacingDirection = NORTH; // Z positivo

    // Objetos contidos desse tile. Interações via mouse com um objeto só são permitidas
    // se estamos no tile dele
    std::vector<GameObject* > objects;

    public:

    Tile(float px, float py, float pz) :
    centerPos{px, py, pz, 1.0f} {}

    void setNorth(Tile* t);
    void setSouth(Tile* t);
    void setEast(Tile* t);
    void setWest(Tile* t);

    glm::vec4 getCenterPos();

    // Realiza movimento
    void handleMovement(Tile** curTile, Camera& mainCamera);

    // Adiciona um objeto ao tile
    void addObject(GameObject* obj);

    // Acesso de leitura aos objetos
    std::vector<GameObject* > const& getObjects();


};

#endif // __TILE_H__

