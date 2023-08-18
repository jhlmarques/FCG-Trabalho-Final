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
void Tile::handleMovement(Tile** curTile, Camera& mainCamera){
    static auto rotateLeft = Matrix_Rotate(M_PI_2, mainCamera.getVVec());
    static auto rotateRight = Matrix_Rotate(-M_PI_2, mainCamera.getVVec());
    // Tempo desde o último input processado
    static float lastProcessedInput = (float) glfwGetTime();;
    // Estado atual de scroll (basicamente a inclinação)
    static ScrollDirection curScrollDirection = SCROLL_NONE;


    float cur_time = (float) glfwGetTime();
    if((cur_time - lastProcessedInput) < 0.5f){
        return;
    }

    // Não podemos tratar outros movimentos enquanto estivermos 
    // com input de scroll novo ou com a câmera inclinada
    // para cima (por questão de design e facilidade de código)
    if( (g_scrolledDirection != SCROLL_NONE)  ||
        (curScrollDirection != SCROLL_NONE))
        {
        // Faz algo se houve scroll para uma direção contrária
        if((g_scrolledDirection != SCROLL_NONE) && (g_scrolledDirection != curScrollDirection)){
            if(g_scrolledDirection == SCROLL_UP){
                mainCamera.setradiansToRotate(M_PI_4, Z);
            }
            else if(g_scrolledDirection == SCROLL_DOWN){
                mainCamera.setradiansToRotate(-M_PI_4, Z);
            }
            // Se não estávamos inclinados, agora estamos; senão, estamos olhando reto
            curScrollDirection = (curScrollDirection == SCROLL_NONE) ? g_scrolledDirection : SCROLL_NONE;
        }
        g_scrolledDirection = SCROLL_NONE; // Reseta scroll

    }
    // Comandos de movimento
    else if(g_wPressed){
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
            auto pointToMove = dst->getCenterPos();
            pointToMove.y += CAMERA_HEAD_HEIGHT;
            mainCamera.setDestinationPoint(pointToMove);
        }

    }
    else if(g_aPressed){
        // Vira para a esquerda
        // Animação
        mainCamera.setradiansToRotate(M_PI_2, Y);

        curFacingDirection--;
        if(curFacingDirection < 0){
            curFacingDirection = 3;
        }


    }
    else if(g_sPressed){
        // TO-DO: ANIMAÇÃO
        // Volta do puzzle para o centro do quadrado
        // mainCamera.setPosition(centerPos);
    }
    else if(g_dPressed){
        // Vira para a direita
        // Animação
        mainCamera.setradiansToRotate(-M_PI_2, Y);

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

void Tile::addObject(GameObject* obj){
    objects.emplace_back(obj);
}

std::vector<GameObject* > const& Tile::getObjects(){
    return objects;
} 