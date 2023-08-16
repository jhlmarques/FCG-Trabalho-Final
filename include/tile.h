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

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define TILE_WIDTH 12.0f


// Um objeto (modelo 3D com interação com o jogador) contido num tile
struct TileObject{
    const char* obj_str;
    glm::vec4 positionInTile; // Posição relativa ao tile
    TileObject(const char* obj_str, glm::vec4 positionInTile) : obj_str(obj_str), positionInTile(positionInTile) {}
};

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
    std::vector<TileObject> objects;

    public:

    Tile(float px, float py, float pz) :
    centerPos{px, py, pz, 1.0f} {}

    void setNorth(Tile* t);
    void setSouth(Tile* t);
    void setEast(Tile* t);
    void setWest(Tile* t);

    glm::vec4 getCenterPos();

    // Realiza movimento
    void handleMovement(Tile** curTile);

    // Adiciona um objeto ao tile
    void addObject(const char* obj_str, glm::vec4 positionInTile);

    // Acesso de leitura aos objetos
    std::vector<TileObject> const& getObjects();


};

#endif // __TILE_H__

