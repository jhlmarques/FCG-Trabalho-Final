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

void Puzzle::drawObjects(){
    glm::mat4 model;
    for(auto& it : objects){
        const auto obj = it.second;
        const auto pos = obj->getPosition();
        model = Matrix_Translate(pos.x, pos.y, pos.z) * obj->getScaleMatrix() * obj->getRotationMatrix();
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        obj->draw(room.getLightSource());
    }
}

// void Puzzle::addObject(std::string name, GameObject* obj){
//     objects[name] = obj;
// }


// GameObject* Puzzle::getObject(std::string obj_name){
//     return objects[obj_name];
// }


MainLobby::MainLobby() : 
playerPosition(0.0f, CAMERA_HEAD_HEIGHT, 0.0f, 1.0f), 
curFacingDirection(NORTH),
enteredPuzzle(false),
cameraAnimationID(ANIMATION_ID_NONE)
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
                AnimationData animation;
                animation.setradiansToRotate(M_PI_4, Z, CAMERA_ROTATE_SPEED);
                cameraAnimationID = g_AnimationManager.addAnimatedCamera(&room.getCamera(), animation);
            }
            else if(g_scrolledDirection == SCROLL_DOWN){
                AnimationData animation;
                animation.setradiansToRotate(-M_PI_4, Z, CAMERA_ROTATE_SPEED);
                cameraAnimationID = g_AnimationManager.addAnimatedCamera(&room.getCamera(), animation);
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
                if(playerPosition.z == -(LOBBY_LENGTH*STEP_SIZE)){
                    return;
                }
                playerPosition.z -= STEP_SIZE;
                break;
            case EAST:
                if(playerPosition.x == (STEP_SIZE*LOBBY_SIDE_WIDTH)){
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
                if(playerPosition.x == -(STEP_SIZE*LOBBY_SIDE_WIDTH)){
                    return;
                }
                playerPosition.x -= STEP_SIZE;
                break;
        }

        // Realiza animação da câmera até a nova posição
        AnimationData animation;
        animation.setDestinationPoint(playerPosition, 10.0f);
        cameraAnimationID = g_AnimationManager.addAnimatedCamera(&room.getCamera(), animation);

    }
    else if(g_aPressed){
        // Vira para a esquerda
        // Animação
        AnimationData animation;
        animation.setradiansToRotate(M_PI_2, Y, CAMERA_ROTATE_SPEED);
        cameraAnimationID = g_AnimationManager.addAnimatedCamera(&room.getCamera(), animation);
        

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
        AnimationData animation;
        animation.setradiansToRotate(-M_PI_2, Y, CAMERA_ROTATE_SPEED);
        cameraAnimationID = g_AnimationManager.addAnimatedCamera(&room.getCamera(), animation);

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
        return LOBBY_LENGTH -(playerPosition.z / STEP_SIZE);
    }
    // Saída
    else if(playerPosition.z == -LOBBY_LENGTH*STEP_SIZE){
        return PUZZLE_ID_LOCK;
    }

    return PUZZLE_ID_NONE;
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
    camera.setViewVectorFree(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    room.setCamera(camera);
    
    // Iluminação do lobby principal
    glm::vec4 lightPosition = glm::vec4(0.0f, LOBBY_LIGHT_SOURCE_HEIGHT, -(LOBBY_LENGTH*STEP_SIZE / 2.0f), 1.0f);
    LightSource lightSource(lightPosition);
    
    room.setLightSource(lightSource);

    // Objetos
    GameObject* newObj;
    // Tiles
    for(int i=0; i <= LOBBY_LENGTH; i++){
        for(int j=0; j < LOBBY_WIDTH; j++){
            auto coords = glm::vec4(
                (-(LOBBY_SIDE_WIDTH) + j) * STEP_SIZE,
                0.0f,
                (float) i * -STEP_SIZE,
                1.0f);
            newObj = new GameObject(g_mapModels["parquet"], 0);
            newObj->setPosition(coords);
            newObj->setScale(STEP_SIZE / 2.0, 1.0, STEP_SIZE / 2.0);
            newObj->setIlluminationModel(BLINN_PHONG);
            std::string objName = (std::string("tile_") + std::to_string(i*LOBBY_WIDTH+j));
            objects[objName] = newObj;
        }
    }
    // Paredes
    newObj = new GameObject(g_mapModels["stone_wall"], 0);
    newObj->setTextureScale(4.0);
    newObj->setPosition(glm::vec4(0.0f, LOBBY_HEIGHT/2.0f, -(LOBBY_LENGTH*STEP_SIZE + STEP_SIZE/2.0f), 1.0f));
    newObj->setEulerAngleX(M_PI_2);
    newObj->setScale(LOBBY_WIDTH*STEP_SIZE/2.0f, LOBBY_HEIGHT/2.0f, LOBBY_HEIGHT/2.0f);
    objects["wall_north"] = newObj;
    
    newObj = new GameObject(g_mapModels["stone_wall"], 0);
    newObj->setTextureScale(4.0);
    newObj->setPosition(glm::vec4((STEP_SIZE*LOBBY_SIDE_WIDTH + STEP_SIZE/2.0f), LOBBY_HEIGHT/2.0f, -LOBBY_LENGTH*STEP_SIZE/2.0f, 1.0f));
    newObj->setEulerAngleX(M_PI_2);
    newObj->setEulerAngleZ(M_PI_2);
    newObj->setScale(1.0, LOBBY_HEIGHT / 2.0f, LOBBY_LENGTH*STEP_SIZE/2.0f + STEP_SIZE/2.0f);
    objects["wall_east"] = newObj;
    
    newObj = new GameObject(g_mapModels["stone_wall"], 0);
    newObj->setTextureScale(4.0);
    newObj->setPosition(glm::vec4(0.0f, LOBBY_HEIGHT/2.0f, STEP_SIZE/2.0f, 1.0f));
    newObj->setEulerAngleX(-M_PI_2);
    newObj->setScale(LOBBY_WIDTH*STEP_SIZE/2.0f, LOBBY_HEIGHT/2.0f, LOBBY_HEIGHT/2.0f);
    objects["wall_south"] = newObj;
    
    newObj = new GameObject(g_mapModels["stone_wall"], 0);
    newObj->setTextureScale(4.0);
    newObj->setPosition(glm::vec4(-(STEP_SIZE*LOBBY_SIDE_WIDTH + STEP_SIZE/2.0f), LOBBY_HEIGHT/2.0f, -LOBBY_LENGTH*STEP_SIZE/2.0f, 1.0f));
    newObj->setEulerAngleX(-M_PI_2);
    newObj->setEulerAngleZ(-M_PI_2);
    // newObj->setScale(LOBBY_HEIGHT/2.0f, 1.0f, LOBBY_LENGTH*STEP_SIZE/2.0f + STEP_SIZE/2.0f);
    newObj->setScale(1.0, LOBBY_HEIGHT / 2.0f, LOBBY_LENGTH*STEP_SIZE/2.0f + STEP_SIZE/2.0f);
    objects["wall_west"] = newObj;

    // Teto
    newObj = new GameObject(g_mapModels["wood_ceiling"], 0);
    // Adicionei um offset (0.2) porque havia vãos pretos nos lados do teto
    newObj->setScale((STEP_SIZE*(LOBBY_SIDE_WIDTH + 0.2) + STEP_SIZE/2.0f), 1.0f, STEP_SIZE*LOBBY_LENGTH + STEP_SIZE/2.0f);
    newObj->setEulerAngleZ(M_PI);
    // 0.25 de offset no Y para que a fonte de luz fique um pouco abaixo do teto e do chandelier 
    newObj->setPosition(room.getLightSource().getPosition() + glm::vec4(0.0f, 0.25f, 0.0f, 0.0f));
    newObj->setTextureScale(8.0);
    objects["ceiling"] = newObj;


    // Fonte de luz
    newObj = new GameObject(g_mapModels["light"], 0);
    newObj->setPosition(room.getLightSource().getPosition() + glm::vec4(0.0f, 0.25f, 0.0f, 0.0f));
    objects["light"] = newObj;

    // Busto
    newObj = new GameObject(g_mapModels["bust"], 0);
    newObj->setPosition(glm::vec4(0.0, 0.0, -STEP_SIZE, 1.0));
    objects["bust"] = newObj;

    // Quadro do puzzle da caixa
    newObj = new GameObject(g_mapModels["frame"], 0);
    newObj->setPosition(glm::vec4(-(STEP_SIZE*LOBBY_SIDE_WIDTH + STEP_SIZE/2.0f) , CAMERA_HEAD_HEIGHT, -LOBBY_LENGTH*STEP_SIZE/2.0f, 1.0));
    newObj->setScale(1.0f, FRAME_SIZE, FRAME_SIZE*g_ScreenRatio);
    newObj->setEulerAngleY(M_PI_2);
    objects["frame_crate"] = newObj;

    newObj = new GameObject(g_mapModels["frame_crate_canvas"], 0);
    newObj->setPosition(glm::vec4(-(STEP_SIZE*LOBBY_SIDE_WIDTH + STEP_SIZE/2.0f) , CAMERA_HEAD_HEIGHT, -LOBBY_LENGTH*STEP_SIZE/2.0f, 1.0));
    newObj->setScale(1.0f, FRAME_SIZE, FRAME_SIZE*g_ScreenRatio);
    newObj->setEulerAngleY(M_PI_2);
    objects["frame_crate_canvas"] = newObj;

    // // Quadro do puzzle do gnomo
    newObj = new GameObject(g_mapModels["frame"], 0);
    newObj->setPosition(glm::vec4((STEP_SIZE*LOBBY_SIDE_WIDTH + STEP_SIZE/2.0f) , CAMERA_HEAD_HEIGHT, -LOBBY_LENGTH*STEP_SIZE/2.0f, 1.0));
    newObj->setScale(1.0f, FRAME_SIZE, FRAME_SIZE*g_ScreenRatio);
    newObj->setEulerAngleY(-M_PI_2);
    objects["frame_gnome"] = newObj;

    // Porta de saída
    newObj = new GameObject(g_mapModels["door"], 0);
    newObj->setPosition(glm::vec4(0.0 , 0.0, -LOBBY_LENGTH*STEP_SIZE - (STEP_SIZE/2) +0.01, 1.0));
    newObj->setEulerAngleY(-M_PI_2);
    objects["door"] = newObj;

}

void MainLobby::updateState(){
    static int statueAnimationID = ANIMATION_ID_NONE;
    static bool statueStatus = false;
    

    // Apenas realizamos um movimento se a câmera não está animando
    if(g_AnimationManager.hasAnimationFinished(cameraAnimationID, false)){
        g_AnimationManager.removeAnimation(cameraAnimationID, false);
        // Puzzles sempre estão nas laterais
        // Jogo de cartas fica ao norte da sala
        if(
            !enteredPuzzle &&
            (
                ((curFacingDirection == WEST) && (playerPosition.x == -(STEP_SIZE*LOBBY_SIDE_WIDTH))) ||
                ((curFacingDirection == EAST) && (playerPosition.x == (STEP_SIZE*LOBBY_SIDE_WIDTH))) ||
                ((curFacingDirection == NORTH) && (playerPosition.x == 0) && (playerPosition.z == -(STEP_SIZE*LOBBY_LENGTH)))
            ) 
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

    // // PLACEHOLDER / TESTAGEM
    // if(g_AnimationManager.hasAnimationFinished(statueAnimationID, true)){
    //     g_AnimationManager.removeAnimation(statueAnimationID, true);
    //     AnimationData animation;
    //     auto bust = objects["bust"];
    //     auto pos = bust->getPosition();
    //     if(statueStatus){
    //         //animation.setDestinationPoint(glm::vec4(STEP_SIZE, 0.0f, -STEP_SIZE, 1.0f));
    //         animation.setBezierCurveJump(pos, glm::vec4(-1.0,2.0,0.0,1.0), glm::vec4(-2.0,1.0,0.0,1.0), glm::vec4(-3.0,0.0,0.0,1.0), 2.0f);
    //     }
    //     else{
    //         //animation.setDestinationPoint(glm::vec4(-STEP_SIZE, 0.0f, -STEP_SIZE, 1.0f));
    //         animation.setBezierCurveJump(pos, glm::vec4(1.0,2.0,0.0,1.0), glm::vec4(2.0,1.0,0.0,1.0), glm::vec4(3.0,0.0,0.0,1.0), 2.0f);
    //     }
    //     statueStatus = !statueStatus;
    //     statueAnimationID = g_AnimationManager.addAnimatedObject(bust, animation);

    // }

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

    room.setCamera(camera);
    room.setLightSource(lightSource);

    auto newObj = new GameObject(g_mapModels["wooden_crate"], 0);
    newObj->setScale(4.0f, 4.0f, 4.0f);
    objects["crate"] = newObj;

}


void CratePuzzle::updateState(){
    return;
}

void CratePuzzle::handleCursorMovement(float dx, float dy){
    Puzzle::handleCursorMovement(dx, dy);
    room.getCamera().setCameraTheta(currentTheta);
    room.getCamera().setCameraPhi(currentPhi);
    room.getCamera().updateViewVecLookAt();
}

void CratePuzzle::handleScroll(double xoffset, double yoffset){
    return; // a príncipio não pode dar zoom nesse puzzle
}

// GNOMO

GnomePuzzle::GnomePuzzle():
prev_time((float)glfwGetTime())
{

}

void GnomePuzzle::setupRoom(){
    // Câmera está olhando em direção ao gnomo e um pouco acima dele (offset no Y)
    glm::vec4 gnome_position = glm::vec4(gnome_initial_position);
    Camera camera(gnome_position + glm::vec4(0.0f, 0.25f, -1.0f, 0.0f));    
    room.setCamera(camera);

    glm::vec4 lightPosition = camera.getPosition();
    LightSource lightSource(lightPosition);    
    room.setLightSource(lightSource);

    room.setBackgroundColor(WHITE_BACKGROUND_COLOR);

    // Chão


    // Gnomo principal que será movido
    auto newObj = new GameObject(g_mapModels["gnome"], 0);
    newObj->setPosition(gnome_position);
    newObj->setEulerAngleY(-M_PI_2);
    newObj->setIlluminationModel(BLINN_PHONG);
    objects["gnome"] = newObj;
    

    newObj = new GameObject(g_mapModels["gnome"], 0);
    newObj->setEulerAngleY(M_PI);
    newObj->setIlluminationModel(BLINN_PHONG);
    newObj->setScale(0.5f, 0.5f, 0.5f); // Escala menor para o teste de colisão
    newObj->setPosition(glm::vec4(-0.75f, 0.0f, 0.0f, 1.0f));
    objects["gnome_test"] = newObj;
}

void GnomePuzzle::updateState(){
    moveGnome();
    if (checkPlaneToPlaneCollision(objects["gnome"], objects["gnome_test"])){
        // Reseta a posição do gnomo quando há colisão
        objects["gnome"]->setPosition(gnome_initial_position);
    }
}


void GnomePuzzle::updateCamera(){
    room.getCamera().setPositionFree(objects["gnome"]->getPosition() + glm::vec4(0.0f, 0.25f, -1.0f, 0.0f));
    Puzzle::updateCamera();
    room.getLightSource().setPosition(room.getCamera().getPosition());
}

void GnomePuzzle::moveGnome(){
    static float speed = 0.5f;

    float current_time = (float)glfwGetTime();
    float delta_t = current_time - prev_time;
    prev_time = current_time;

    glm::vec4 gnome_position = objects["gnome"]->getPosition();

    if (g_upPressed){
        gnome_position.y += speed*delta_t;
    }
    if (g_downPressed){
        gnome_position.y -= speed*delta_t;
    }
    if (g_leftPressed){
        gnome_position.x += speed*delta_t;
    }
    if (g_rightPressed){
        gnome_position.x -= speed*delta_t;
    }
    objects["gnome"]->setPosition(gnome_position);

}

void BallPuzzle::setupRoom(){
    round = 0;
    curPos = 0;
    playerMovementAnimationID = ANIMATION_ID_NONE;
    
    auto camera = Camera(glm::vec4(2.0f, 3.0f, 2.0f, 1.0f));
    camera.setLookAtPoint(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    room.setCamera(camera);
    
    glm::vec4 lightPosition = glm::vec4(0.0f, 3.0f, 0.0f, 1.0f);
    LightSource lightSource(lightPosition);
    room.setLightSource(lightSource);

    auto newObj = new GameObject(g_mapModels["platform"], 0);
    newObj->setPosition(glm::vec4(-1.0, 0.0, 1.0, 1.0));
    objects["tile1"] = newObj;

    newObj = new GameObject(g_mapModels["platform"], 0);
    newObj->setPosition(glm::vec4(1.0, 0.0, 1.0, 1.0));
    objects["tile2"] = newObj;

    newObj = new GameObject(g_mapModels["platform"], 0);
    newObj->setPosition(glm::vec4(1.0, 0.0, -1.0, 1.0));
    objects["tile3"] = newObj;

    newObj = new GameObject(g_mapModels["platform"], 0);
    newObj->setPosition(glm::vec4(-1.0, 0.0, -1.0, 1.0));
    objects["tile4"] = newObj;

    newObj = new GameObject(g_mapModels["bust"], 0);
    newObj->setPosition(glm::vec4(-1.0, 0.0, 1.0, 1.0));
    newObj->setEulerAngleY(M_PI_2);
    objects["player"] = newObj;
    player = newObj;

    newObj = new GameObject(g_mapModels["ball"], 0);
    newObj->setPosition(glm::vec4(-200.0, 0.0, 0.0, 1.0));
    newObj->setScale(0.5,0.5,0.5);
    objects["ball"] = newObj;
    ball = newObj;

    newObj = new GameObject(g_mapModels["ball_puzzle"], 0);
    newObj->setPosition(glm::vec4(-200.0, 0.0, 0.0, 1.0));
    newObj->setScale(0.5,0.5,0.5);
    objects["ball_puzzle"] = newObj;
    ball_puzzle = newObj;
    
}

void BallPuzzle::updateState(){
    static float lastBallLaunched = (float) glfwGetTime();
    static bool ballLaunched = false;
    float cur_time = (float) glfwGetTime();

    if((cur_time - lastBallLaunched) > 10.0f){
        // Lança bola
        ballLaunched = true;
        AnimationData animation;
        GameObject* ballToLaunch;
        // Chance de ser a bola com a resposta do puzzle
        if(std::rand() % 10){
            ballToLaunch = ball;
        }
        else{
            ballToLaunch = ball_puzzle;
        }

        switch(std::rand() % 4){
            case 0:
                ballToLaunch->setPosition(glm::vec4(-10.0, 1.0,-1.0, 1.0));
                animation.setDestinationPoint(glm::vec4(10.0, 0.0,-1.0, 1.0), 2.5);
                break;
            case 1:
                ballToLaunch->setPosition(glm::vec4(-10.0, 1.0, 1.0, 1.0));
                animation.setDestinationPoint(glm::vec4(10.0, 0.0,1.0, 1.0), 2.5);
                break;
            case 2:
                ballToLaunch->setPosition(glm::vec4(-1.0, 1.0,-10.0, 1.0));
                animation.setDestinationPoint(glm::vec4(-1.0, 0.0, 10.0, 1.0), 2.5);
                break;
            case 3:
                ballToLaunch->setPosition(glm::vec4(1.0, 1.0,-10.0, 1.0));
                animation.setDestinationPoint(glm::vec4(-1.0, 0.0, 10.0, 1.0), 2.5);
                break;
        }
        ballMovementAnimationID = g_AnimationManager.addAnimatedObject(ballToLaunch, animation);
        
        lastBallLaunched = cur_time;
    }

    if(g_AnimationManager.hasAnimationFinished(playerMovementAnimationID, true)){
        if(g_wPressed){
            AnimationData animation;
            glm::vec4 start = player->getPosition();
            glm::vec4 end;
            switch(curPos){
                case 0:
                    end = objects["tile2"]->getPosition();
                    break;
                case 1:
                    end = objects["tile3"]->getPosition();
                    break;
                case 2:
                    end = objects["tile4"]->getPosition();
                    break;
                case 3:
                    end = objects["tile1"]->getPosition();
                    break;
            }
            curPos = (curPos == 3) ? 0 : ++curPos;

            auto distance = end - start;
            auto disloc = glm::vec4(distance.x/2, 0.0, distance.z/2, 0.0f);
            auto p2 = start + disloc;
            p2.y = start.y + 0.4;
            auto p3 = end - disloc;
            p3.y = start.y + 0.2;
            animation.setBezierCurveJump(start, p2, p3, end, 3.0);
            animation.setradiansToRotate(M_PI_2, Y, 360.0);

            playerMovementAnimationID = g_AnimationManager.addAnimatedObject(player, animation);
        }
    }

}

void LockPuzzle::setupRoom(){
    lock_ring_val1 = 0;
    lock_ring_val2 = 0;
    lock_ring_val3 = 0;
    
    auto camera = Camera(glm::vec4(0.0f, 0.5f, 0.0f, 1.0f));
    camera.setLookAtPoint(glm::vec4(0.0f, 0.5f, -1.0f, 1.0f));
    room.setCamera(camera);
    
    glm::vec4 lightPosition = glm::vec4(0.0f, 3.0f, 0.0f, 1.0f);
    LightSource lightSource(lightPosition);
    room.setLightSource(lightSource);

    // Começa "saltado", pois é o primeiro input 
    auto newObj = new GameObject(g_mapModels["lock_ring"], 0);
    newObj->setPosition(glm::vec4(0.0, 0.75, -3.2, 1.0));
    objects["lock_ring1"] = newObj;

    newObj = new GameObject(g_mapModels["lock_ring"], 0);
    newObj->setPosition(glm::vec4(0.0, 0.5, -3.5, 1.0));
    objects["lock_ring2"] = newObj;

    newObj = new GameObject(g_mapModels["lock_ring"], 0);
    newObj->setPosition(glm::vec4(0.0, 0.25, -3.5, 1.0));
    objects["lock_ring3"] = newObj;

    newObj = new GameObject(g_mapModels["lock_body"], 0);
    newObj->setPosition(glm::vec4(0.0, 0.5, -3.5, 1.0));
    objects["lock_body"] = newObj;


}

void LockPuzzle::updateState(){
    static uint8_t curSelectedRing = 0;
    static int animationPrevSel = ANIMATION_ID_NONE;
    static int animationSel = ANIMATION_ID_NONE;
    static float lastProcessedInput = (float) glfwGetTime();;

    float cur_time = (float) glfwGetTime();
    if((cur_time - lastProcessedInput) < 0.25f){
        return;
    }

    if( !g_AnimationManager.hasAnimationFinished(animationPrevSel, true) ||
        !g_AnimationManager.hasAnimationFinished(animationSel, true)
    ){
        return;
    }

    if(g_downPressed){
        AnimationData anim1, anim2;
        glm::vec4 pos_prev_selected;
        glm::vec4 pos_selected;
        GameObject* prev_selected;
        GameObject* selected;
        switch(curSelectedRing){
            case 0:
                prev_selected = objects["lock_ring1"];
                selected = objects["lock_ring2"];
                curSelectedRing = 1;
                break;
            case 1:
                prev_selected = objects["lock_ring2"];
                selected = objects["lock_ring3"];
                curSelectedRing = 2;
                break;
            case 2:
                prev_selected = objects["lock_ring3"];
                selected = objects["lock_ring1"];
                curSelectedRing = 0;
                break;
        }

        pos_prev_selected = prev_selected->getPosition();
        pos_prev_selected.z = -3.5;
        pos_selected = selected->getPosition();
        pos_selected.z = -3.2;
        anim1.setDestinationPoint(pos_prev_selected, 3.0f);
        anim2.setDestinationPoint(pos_selected, 3.0f);
        animationPrevSel = g_AnimationManager.addAnimatedObject(prev_selected, anim1);
        animationSel = g_AnimationManager.addAnimatedObject(selected, anim2);
    }
    else if(g_upPressed){
        AnimationData anim1, anim2;
        glm::vec4 pos_prev_selected;
        glm::vec4 pos_selected;
        GameObject* prev_selected;
        GameObject* selected;
        switch(curSelectedRing){
            case 0:
                prev_selected = objects["lock_ring1"];
                selected = objects["lock_ring3"];
                curSelectedRing = 2;
                break;
            case 1:
                prev_selected = objects["lock_ring2"];
                selected = objects["lock_ring1"];
                curSelectedRing = 0;
                break;
            case 2:
                prev_selected = objects["lock_ring3"];
                selected = objects["lock_ring2"];
                curSelectedRing = 1;
                break;
        }

        pos_prev_selected = prev_selected->getPosition();
        pos_prev_selected.z = -3.5;
        pos_selected = selected->getPosition();
        pos_selected.z = -3.2;
        anim1.setDestinationPoint(pos_prev_selected, 3.0f);
        anim2.setDestinationPoint(pos_selected, 3.0f);
        animationPrevSel = g_AnimationManager.addAnimatedObject(prev_selected, anim1);
        animationSel = g_AnimationManager.addAnimatedObject(selected, anim2);
    }
    else if(g_rightPressed){
        AnimationData animation;
        GameObject* selected;
        switch(curSelectedRing){
            case 0:
                selected = objects["lock_ring1"];
                lock_ring_val1 = (lock_ring_val1 == 0) ? 9 : --lock_ring_val1;
                break;
            case 1:
                selected = objects["lock_ring2"];
                lock_ring_val2 = (lock_ring_val2 == 0) ? 9 : --lock_ring_val2;
                break;
            case 2:
                selected = objects["lock_ring3"];
                lock_ring_val3 = (lock_ring_val3 == 0) ? 9 : --lock_ring_val3;
                break;
        }
        animation.setradiansToRotate(glm::radians(36.0f), Y, 102.0f);
        animationSel = g_AnimationManager.addAnimatedObject(selected, animation);        
    }
    else if(g_leftPressed){
        AnimationData animation;
        GameObject* selected;
        switch(curSelectedRing){
            case 0:
                selected = objects["lock_ring1"];
                lock_ring_val1 = (lock_ring_val1 == 9) ? 0 : ++lock_ring_val1;
                break;
            case 1:
                selected = objects["lock_ring2"];
                lock_ring_val2 = (lock_ring_val2 == 9) ? 0 : ++lock_ring_val2;
                break;
            case 2:
                selected = objects["lock_ring3"];
                lock_ring_val3 = (lock_ring_val3 == 9) ? 0 : ++lock_ring_val3;
                break;
        }
        animation.setradiansToRotate(glm::radians(-36.0f), Y, 102.0f);
        animationSel = g_AnimationManager.addAnimatedObject(selected, animation);        
    }
    else{
        return;
    }

    // Checa se combinação é a certa
    if( (lock_ring_val1 == LOCK_PUZZLE_RING_ANSWER1) &&
        (lock_ring_val2 == LOCK_PUZZLE_RING_ANSWER2) &&
        (lock_ring_val3 == LOCK_PUZZLE_RING_ANSWER3)
    ){
        g_lockOpened = true;
    }

    lastProcessedInput = cur_time;

}
