#ifndef __PUZZLE_H__
#define __PUZZLE_H__

#include "room.h"
#include "tile.h"
#include <vector>
#include <map>
#include <string>
#include "gameobject.h"

class Puzzle{

    private:
    std::map<std::string, GameObject*> objects;

    protected:
    Room room;

    // Atualiza câmera
    virtual void updateCamera();
    // Configura sala
    virtual void setupRoom() = 0;
    // Lida com inputs
    virtual void handleInputs() = 0;
    // Desenha objetos do puzzle
    virtual void drawObjects() = 0;

    public:

    // Atualiza sala
    void update();

    // Adiciona objeto
    void addObject(std::string name, GameObject* obj);

    // Obtém objeto
    GameObject* getObject(std::string obj_name);

};

class MainLobby : public Puzzle{
    private:
    std::vector<Tile> tileVector;
    Tile* cur_tile;
    
    public:
    MainLobby();
    void setupRoom();
    void handleInputs();
    void drawObjects();
};

#endif // __PUZZLE_H__
