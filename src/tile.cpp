#include "tile.h"

void Tile::setNorth(Tile* t){
    north = t;
}

void Tile::setSouth(Tile* t){
    south = t;
}

void Tile::setEast(Tile* t){
    east = t;
}

void Tile::setWest(Tile* t){
    west = t;
}

glm::vec4 Tile::getCenterPos(){
    return centerPos;
}

// Realiza movimento
void Tile::handleMovement(){
    static auto rotateLeft = Matrix_Rotate(M_PI_2, mainCamera.getVVec());
    static auto rotateRight = Matrix_Rotate(-M_PI_2, mainCamera.getVVec());

    Tile* dst;

    std::chrono::steady_clock::time_point cur_time = std::chrono::steady_clock::now();

    if((cur_time - lastProcessedInput) < std::chrono::milliseconds(500)){
        return;
    }

    if(g_aPressed){
        // TO-DO: ANIMAÇÃO
        // Vira para a esquerda
        auto v = mainCamera.getViewVec();
        glm::vec4 v_hat = rotateLeft * v;
        mainCamera.setViewVector(v_hat);
    }
    else if(g_dPressed){
        // TO-DO: ANIMAÇÃO
        // Vira para a direita
        auto v = mainCamera.getViewVec();
        glm::vec4 v_hat = rotateRight * v;
        mainCamera.setViewVector(v_hat);
    }


    lastProcessedInput = cur_time;




}
