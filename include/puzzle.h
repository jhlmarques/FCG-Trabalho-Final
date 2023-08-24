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

    float currentTheta = 0.0f;
    float currentPhi = 0.0f;

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

    // Movimento do cursor
    virtual void handleCursorMovement(float dx, float dy);

    // Scroll
    virtual void handleScroll(double xoffset, double yoffset);

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

class CratePuzzle : public Puzzle{
    public:
    void updateCamera();
    void setupRoom();
    void handleInputs();
    void drawObjects();

    void handleCursorMovement(float dx, float dy);
    void handleScroll(double xoffset, double yoffset);
};

#endif // __PUZZLE_H__
