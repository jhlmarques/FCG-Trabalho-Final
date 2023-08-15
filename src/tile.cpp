#include "tile.h"

void Tile::setNorth(Tile* t){
    north = t;
    t->south = this;
}

void Tile::setSouth(Tile* t){
    south = t;
    t->north = this;
}

void Tile::setEast(Tile* t){
    east = t;
    t->west = this;
}

void Tile::setWest(Tile* t){
    west = t;
    t->east = this;
}

glm::vec4 Tile::getCenterPos(){
    return centerPos;
}

// Realiza movimento
void Tile::handleMovement(Tile** curTile){
    static auto rotateLeft = Matrix_Rotate(M_PI_2, mainCamera.getVVec());
    static auto rotateRight = Matrix_Rotate(-M_PI_2, mainCamera.getVVec());
    // Tempo desde o último input processado
    static std::chrono::steady_clock::time_point lastProcessedInput;


    std::chrono::steady_clock::time_point cur_time = std::chrono::steady_clock::now();

    if((cur_time - lastProcessedInput) < std::chrono::milliseconds(500)){
        return;
    }

    if(g_wPressed){
        Tile* dst;

        switch (curFacingDirection)
        {
            case NORTH:
                dst = north;
                break;
            case EAST:
                dst = east;
                break;
            case SOUTH:
                dst = south;
                break;
            case WEST:
                dst = west;
                break;
        }

        if(dst != nullptr){
            // Ponteiro agora aponta para o novo tile
            // TO-DO: Ao invés de passar ponteiro, passar uma classe que controla o estado
            // do jogo e que pode ser então modificada?
            *curTile = dst;
            dst->curFacingDirection = curFacingDirection;
        }

    }
    else if(g_aPressed){
        // TO-DO: ANIMAÇÃO
        // Vira para a esquerda
        auto v = mainCamera.getViewVec();
        glm::vec4 v_hat = rotateLeft * v;
        mainCamera.setViewVector(v_hat);

        curFacingDirection--;
        if(curFacingDirection < 0){
            curFacingDirection = 3;
        }

    }
    else if(g_sPressed){
        // TO-DO: ANIMAÇÃO
        // Volta do puzzle para o centro do quadrado
        mainCamera.setPosition(centerPos);
    }
    else if(g_dPressed){
        // TO-DO: ANIMAÇÃO
        // Vira para a direita
        auto v = mainCamera.getViewVec();
        glm::vec4 v_hat = rotateRight * v;
        mainCamera.setViewVector(v_hat);

        curFacingDirection++;
        if(curFacingDirection > 3){
            curFacingDirection = 0;
        }
    }
    else{
        return;
    }


    lastProcessedInput = cur_time;

}
