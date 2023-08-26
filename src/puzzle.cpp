#include "puzzle.h"


void Puzzle::step(){
    glm::vec4 bg = room.getBackgroundColor();
    glClearColor(bg.r, bg.g, bg.b, bg.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateCamera();
    updateState();
    drawObjects();

}

void Puzzle::handleCursorMovement(float dx, float dy){
    currentTheta -=0.003f*dx;
    currentPhi += 0.003f*dy;
    return;
}

void Puzzle::handleScroll(double xoffset, double yoffset){
    return;
}

void Puzzle::updateCamera(){
    // Transformação da câmera
    glm::mat4 const& view = room.getCamera().getViewMatrix();
    glUniformMatrix4fv(g_view_uniform, 1 , GL_FALSE , glm::value_ptr(view));
}

void Puzzle::addObject(std::string name, GameObject* obj){
    objects[name] = obj;
}


GameObject* Puzzle::getObject(std::string obj_name){
    return objects[obj_name];
}


MainLobby::MainLobby() : 
playerPosition(0.0f, CAMERA_HEAD_HEIGHT, 0.0f, 1.0f), 
curFacingDirection(NORTH),
enteredPuzzle(false)
{

}

bool MainLobby::hasEnteredPuzzle(){
    return enteredPuzzle;
}

void MainLobby::playerMove(){
    static auto rotateLeft = Matrix_Rotate(M_PI_2, room.getCamera().getVVec());
    static auto rotateRight = Matrix_Rotate(-M_PI_2, room.getCamera().getVVec());
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
                room.getCamera().setradiansToRotate(M_PI_4, Z);
            }
            else if(g_scrolledDirection == SCROLL_DOWN){
                room.getCamera().setradiansToRotate(-M_PI_4, Z);
            }
            // Se não estávamos inclinados, agora estamos; senão, estamos olhando reto
            curScrollDirection = (curScrollDirection == SCROLL_NONE) ? g_scrolledDirection : SCROLL_NONE;
        }
        g_scrolledDirection = SCROLL_NONE; // Reseta scroll

    }
    // Comandos de movimento
    else if(g_wPressed){
        switch (curFacingDirection)
        {
            case NORTH:
                if(playerPosition.z == -(ROOM_LENGTH*STEP_SIZE)){
                    return;
                }
                playerPosition.z -= STEP_SIZE;
                break;
            case EAST:
                if(playerPosition.x == (STEP_SIZE*ROOM_SIDE_WIDTH)){
                    return;
                }
                playerPosition.x += STEP_SIZE;
                break;
            case SOUTH:
                if(playerPosition.z == 0){
                    return;
                }
                playerPosition.z += STEP_SIZE;
                break;
            case WEST:
                if(playerPosition.x == -(STEP_SIZE*ROOM_SIDE_WIDTH)){
                    return;
                }
                playerPosition.x -= STEP_SIZE;
                break;
        }

        // Realiza animação da câmera até a nova posição
        room.getCamera().setDestinationPoint(playerPosition);

    }
    else if(g_aPressed){
        // Vira para a esquerda
        // Animação
        room.getCamera().setradiansToRotate(M_PI_2, Y);

        curFacingDirection--;
        if(curFacingDirection < NORTH){
            curFacingDirection = WEST;
        }

    }
    else if(g_sPressed){
        // TO-DO: ANIMAÇÃO
        // Volta do puzzle para o centro do quadrado
        // lobbyCamera.setPositionFree(centerPos);
    }
    else if(g_dPressed){
        // Vira para a direita
        // Animação
        room.getCamera().setradiansToRotate(-M_PI_2, Y);

        curFacingDirection++;
        if(curFacingDirection > WEST){
            curFacingDirection = NORTH;
        }

    }
    else{
        return;
    }

    lastProcessedInput = cur_time;    
}

uint8_t MainLobby::getCurrentPuzzleID(){
    // IDs começam em zero, no canto esquerdo, e sobem até o fim do canto esquerdo
    // depois continuam no canto inferior direito e sobem até o fim do canto direito
    if(playerPosition.x < 0){
        return -(playerPosition.z / STEP_SIZE);
    }
    else if(playerPosition.x > 0){
        return ROOM_LENGTH -(playerPosition.z / STEP_SIZE);
    }
    return 255;
}

void MainLobby::handleExitedPuzzle(){
    enteredPuzzle = false;
}

void MainLobby::setupRoom(){
    playerPosition = glm::vec4(0.0f, CAMERA_HEAD_HEIGHT, 0.0f, 1.0f);
    curFacingDirection = NORTH;
    enteredPuzzle = false;

    //  Câmera do lobby principal (inicialmente em modo "free")
    auto camera = Camera(playerPosition);
    camera.setLookAtPoint(glm::vec4(0.0f, CAMERA_HEAD_HEIGHT, -1.0f, 1.0f));
    room.setCamera(camera);
    
    // Iluminação do lobby principal
    glm::vec4 lightPosition = glm::vec4(0.0f, LOBBY_LIGHT_SOURCE_HEIGHT, -(ROOM_LENGTH*STEP_SIZE / 2.0f), 1.0f);
    LightSource lightSource(lightPosition);
    room.setLightSource(lightSource);

}

void MainLobby::updateState(){
    // Apenas realizamos um movimento se a câmera não está animando
    if(!room.getCamera().animate()){
        // Puzzles sempre estão nas laterais
        if(
            ((curFacingDirection == WEST) && (playerPosition.x == -(STEP_SIZE*ROOM_SIDE_WIDTH))) ||
            ((curFacingDirection == EAST) && (playerPosition.x == (STEP_SIZE*ROOM_SIDE_WIDTH)))
        ){
            if(g_wPressed){
                // TO-DO: ANIMAÇÃO DA CÂMERA
                enteredPuzzle = true;
                return;
            }
        }
        // Senão, realizamos movimento
        playerMove();
    }

}

void MainLobby::drawObjects(){
    glm::mat4 model;
    auto obj_tile = Puzzle::getObject("tile");
    auto obj_statue = Puzzle::getObject("statue");
    auto obj_light = Puzzle::getObject("light");

    // DESENHA TILES
    auto scale = Matrix_Scale(STEP_SIZE/2.0f, 1.0f, STEP_SIZE/2.0f);
    for(int i=0; i <= ROOM_LENGTH; i++){
        for(int j=0; j < ROOM_WIDTH; j++){
            auto coords = glm::vec3(
                (-(ROOM_SIDE_WIDTH) + j) * STEP_SIZE,
                0.0f,
                (float) i * -STEP_SIZE);
            // Escala para o tamanho do tile para cobrir espaçamento entre tiles e deslocamento para o centro do tile
            model = Matrix_Translate(coords.x, coords.y, coords.z) * scale;
            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            obj_tile->draw(room.getLightSource());
        }
        
    }

    // Paredes
    // Esquerda
    //model = Matrix_Scale(ROOM_LENGTH*STEP_SIZE, 1.0f, 1.0f);
    model = Matrix_Translate(-(STEP_SIZE*ROOM_SIDE_WIDTH + STEP_SIZE/2.0f), ROOM_HEIGHT/2.0f, -ROOM_LENGTH*STEP_SIZE/2.0f);
    model = model * Matrix_Rotate_Z(-M_PI_2);
    model = model * Matrix_Scale(ROOM_HEIGHT/2.0f, 1.0f, ROOM_LENGTH*STEP_SIZE/2.0f + STEP_SIZE/2.0f);
    
    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
    obj_tile->draw(room.getLightSource());
    // Direita
    model = Matrix_Translate((STEP_SIZE*ROOM_SIDE_WIDTH + STEP_SIZE/2.0f), ROOM_HEIGHT/2.0f, -ROOM_LENGTH*STEP_SIZE/2.0f);
    model = model * Matrix_Rotate_Z(M_PI_2);
    model = model * Matrix_Scale(ROOM_HEIGHT/2.0f, 1.0f, ROOM_LENGTH*STEP_SIZE/2.0f + STEP_SIZE/2.0f);
    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
    obj_tile->draw(room.getLightSource());

    // Traseira
    model = Matrix_Translate(0.0f, ROOM_HEIGHT/2.0f, STEP_SIZE/2.0f);
    model = model * Matrix_Rotate_X(-M_PI_2);
    model = model * Matrix_Scale(ROOM_WIDTH*STEP_SIZE/2.0f, 1.0f, ROOM_HEIGHT/2.0f);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
    obj_tile->draw(room.getLightSource());

    // Dianteira
    model = Matrix_Translate(0.0f, ROOM_HEIGHT/2.0f,  -(ROOM_LENGTH*STEP_SIZE + STEP_SIZE/2.0f));
    model = model * Matrix_Scale(ROOM_WIDTH*STEP_SIZE/2.0f, ROOM_HEIGHT/2.0f, 1.0f);
    model = model * Matrix_Rotate_X(M_PI_2);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
    obj_tile->draw(room.getLightSource());

    // DESENHA OBJETOS

    // Fonte de luz
    auto coords = room.getLightSource().getPosition();
    model = Matrix_Translate(coords.x, coords.y + 0.5f, coords.z);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
    obj_light->draw(room.getLightSource());

    // Busto
    coords = room.getLightSource().getPosition();
    model = Matrix_Translate(coords.x, 0.0f, coords.z);

    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
    obj_statue->draw(room.getLightSource());
    
}

void CratePuzzle::updateCamera(){
    Puzzle::updateCamera();
    room.getLightSource().setPosition(room.getCamera().getPosition());
    room.getLightSource().setDirection(room.getCamera().getViewVec());
}

void CratePuzzle::setupRoom(){
    // Instanciação da câmera de lookat puzzle que aponta para a origem 
    Camera camera(glm::vec3(2.0f, 2.0f, 1.5f));

    glm::vec4 lookatPoint = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    camera.setLookAtPoint(lookatPoint);

    currentTheta = camera.getCameraTheta();
    currentPhi = camera.getCameraPhi();

    glm::vec4 lightPosition = camera.getPosition();
    glm::vec4 lightDirection = camera.getViewVec();
    float lightApertureAngle = M_PI/8.0f;

    LightSource lightSource(lightPosition, lightDirection, lightApertureAngle);

    room.setBackgroundColor(BLACK_BACKGROUND_COLOR);
    room.setCamera(camera);
    room.setLightSource(lightSource);
}


void CratePuzzle::updateState(){
    return;
}

void CratePuzzle::drawObjects(){
    auto obj_crate_9 = Puzzle::getObject("crate");

    glm::mat4 model = Matrix_Identity(); 
    model = Matrix_Scale(4.0f, 4.0f, 4.0f);
    glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
    obj_crate_9->draw(room.getLightSource());
}

void CratePuzzle::handleCursorMovement(float dx, float dy){
    Puzzle::handleCursorMovement(dx, dy);
    room.getCamera().setCameraTheta(currentTheta);
    room.getCamera().setCameraPhi(currentPhi);
    room.getCamera().updateViewVecLookAt();
}

void CratePuzzle::handleScroll(double xoffset, double yoffset){
    float currentCameraDistance = room.getCamera().getCameraDistance();
    currentCameraDistance -= 0.1f*yoffset;

    room.getCamera().setCameraDistance(currentCameraDistance);
    room.getCamera().updateViewVecLookAt();
}