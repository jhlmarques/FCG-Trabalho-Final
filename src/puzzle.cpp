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
                animation.setradiansToRotate(M_PI_4, Z);
                cameraAnimationID = g_AnimationManager.addAnimatedCamera(&room.getCamera(), animation);
            }
            else if(g_scrolledDirection == SCROLL_DOWN){
                AnimationData animation;
                animation.setradiansToRotate(-M_PI_4, Z);
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
        animation.setDestinationPoint(playerPosition);
        cameraAnimationID = g_AnimationManager.addAnimatedCamera(&room.getCamera(), animation);

    }
    else if(g_aPressed){
        // Vira para a esquerda
        // Animação
        AnimationData animation;
        animation.setradiansToRotate(M_PI_2, Y);
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
        animation.setradiansToRotate(-M_PI_2, Y);
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

    // Se não é um dos puzzles laterais, é o jogo de cartas
    return CARD_PUZZLE;
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
            newObj = new GameObject(g_mapModels["plane"], 0);
            newObj->setPosition(coords);
            newObj->setScale(STEP_SIZE / 2.0, 1.0, STEP_SIZE / 2.0);
            std::string objName = (std::string("tile_") + std::to_string(i*LOBBY_WIDTH+j));
            objects[objName] = newObj;
        }
    }
    // Paredes
    newObj = new GameObject(g_mapModels["plane"], 0);
    newObj->setPosition(glm::vec4(0.0f, LOBBY_HEIGHT/2.0f, -(LOBBY_LENGTH*STEP_SIZE + STEP_SIZE/2.0f), 1.0f));
    newObj->setEulerAngleX(M_PI_2);
    newObj->setScale(LOBBY_WIDTH*STEP_SIZE/2.0f, LOBBY_HEIGHT/2.0f, LOBBY_HEIGHT/2.0f);
    objects["wall_north"] = newObj;
    
    newObj = new GameObject(g_mapModels["plane"], 0);
    newObj->setPosition(glm::vec4((STEP_SIZE*LOBBY_SIDE_WIDTH + STEP_SIZE/2.0f), LOBBY_HEIGHT/2.0f, -LOBBY_LENGTH*STEP_SIZE/2.0f, 1.0f));
    newObj->setEulerAngleZ(M_PI_2);
    newObj->setScale(1.0, LOBBY_HEIGHT / 2.0f, LOBBY_LENGTH*STEP_SIZE/2.0f + STEP_SIZE/2.0f);
    objects["wall_east"] = newObj;
    
    newObj = new GameObject(g_mapModels["plane"], 0);
    newObj->setPosition(glm::vec4(0.0f, LOBBY_HEIGHT/2.0f, STEP_SIZE/2.0f, 1.0f));
    newObj->setEulerAngleX(-M_PI_2);
    newObj->setScale(LOBBY_WIDTH*STEP_SIZE/2.0f, LOBBY_HEIGHT/2.0f, LOBBY_HEIGHT/2.0f);
    objects["wall_south"] = newObj;
    
    newObj = new GameObject(g_mapModels["plane"], 0);
    newObj->setPosition(glm::vec4(-(STEP_SIZE*LOBBY_SIDE_WIDTH + STEP_SIZE/2.0f), LOBBY_HEIGHT/2.0f, -LOBBY_LENGTH*STEP_SIZE/2.0f, 1.0f));
    newObj->setEulerAngleZ(-M_PI_2);
    // newObj->setScale(LOBBY_HEIGHT/2.0f, 1.0f, LOBBY_LENGTH*STEP_SIZE/2.0f + STEP_SIZE/2.0f);
    newObj->setScale(1.0, LOBBY_HEIGHT / 2.0f, LOBBY_LENGTH*STEP_SIZE/2.0f + STEP_SIZE/2.0f);
    objects["wall_west"] = newObj;

    // Fonte de luz
    newObj = new GameObject(g_mapModels["light"], 0);
    newObj->setPosition(room.getLightSource().getPosition());
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
    newObj->setIlluminationModel(LAMBERT);
    objects["frame_crate"] = newObj;

    newObj = new GameObject(g_mapModels["frame_crate_canvas"], 0);
    newObj->setPosition(glm::vec4(-(STEP_SIZE*LOBBY_SIDE_WIDTH + STEP_SIZE/2.0f) , CAMERA_HEAD_HEIGHT, -LOBBY_LENGTH*STEP_SIZE/2.0f, 1.0));
    newObj->setScale(1.0f, FRAME_SIZE, FRAME_SIZE*g_ScreenRatio);
    newObj->setEulerAngleY(M_PI_2);
    newObj->setIlluminationModel(LAMBERT);
    objects["frame_crate_canvas"] = newObj;

    // // Quadro do puzzle do gnomo
    newObj = new GameObject(g_mapModels["frame"], 0);
    newObj->setPosition(glm::vec4((STEP_SIZE*LOBBY_SIDE_WIDTH + STEP_SIZE/2.0f) , CAMERA_HEAD_HEIGHT, -LOBBY_LENGTH*STEP_SIZE/2.0f, 1.0));
    newObj->setScale(1.0f, FRAME_SIZE, FRAME_SIZE*g_ScreenRatio);
    newObj->setEulerAngleY(-M_PI_2);
    newObj->setIlluminationModel(LAMBERT);
    objects["frame_gnome"] = newObj;

}

void MainLobby::updateState(){
    static int statueAnimationID = ANIMATION_ID_NONE;
    static bool statueStatus = false;
    

    // Apenas realizamos um movimento se a câmera não está animando
    if(g_AnimationManager.hasAnimationFinished(cameraAnimationID, false)){
        
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

    if(g_AnimationManager.hasAnimationFinished(statueAnimationID, true)){
        AnimationData animation;
        if(statueStatus){
            //animation.setDestinationPoint(glm::vec4(STEP_SIZE, 0.0f, -STEP_SIZE, 1.0f));
            animation.setradiansToRotate(M_PI_2, Y);
        }
        else{
            //animation.setDestinationPoint(glm::vec4(-STEP_SIZE, 0.0f, -STEP_SIZE, 1.0f));
            animation.setradiansToRotate(-M_PI_2, Y);
        }
        statueStatus = !statueStatus;
        statueAnimationID = g_AnimationManager.addAnimatedObject(objects["bust"], animation);

    }

}

int Card::numCards = 0;

Card::Card(ObjModel* model, GLuint shapeIdx ,uint8_t health, uint8_t attack, uint8_t cost) : 
GameObject(model, shapeIdx), health(health), attackPower(attack), bloodCost(cost)
{
    numCards++;
}

void Card::handleAttackingCard(CardGame& game, Card& attacked){
    attacked.handleAttacked(game, *this);
}

void Card::handleAttackingPlayer(CardGame& game, bool isCPU){
    if(isCPU){
        game.reduceHealth(true);
    }
    else{
        game.reduceHealth(false);
    }
}

void Card::handleAttacked(CardGame& game, Card& attacker){
    health -= attacker.attackPower;
    if(health < 0){
        health = 0;
        handleDeath(game);
    }
}

void Card::handleDeath(CardGame& game){
    return;
}

std::string Card::getObjName(){
    return std::string("card_") + std::to_string(numCards-1);
}

CardGame::CardGame() : 
currentLevel(0), 
playerCanPlay(false), 
playerSideCards{nullptr, nullptr, nullptr, nullptr},
enemySideCards{nullptr, nullptr, nullptr, nullptr},
enemyFutureCards{nullptr, nullptr, nullptr, nullptr}
{

}

void CardGame::setupRoom(){
    auto camera = Camera(glm::vec3(0.0f, 0.7f, 0.0f));
    camera.setLookAtPoint(glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
    room.setCamera(camera);
    
    glm::vec4 lightPosition = glm::vec4(0.0f, 2.0f, -1.0f, 1.0f);
    LightSource lightSource(lightPosition);
    room.setLightSource(lightSource);    

    auto newObj = new GameObject(g_mapModels["plane"], 0);
    newObj->setPosition(glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
    objects["table"] = newObj;

}

void CardGame::updateState(){
    if(g_dPressed){
        drawCard();
        g_dPressed = false;
    }
    
    return;
}

void CardGame::handlePlayerTurn(){

}

void CardGame::handleTurnEnded(){

}

void CardGame::handleEnemyTurn(){

}

void CardGame::handleMatchEnd(){

}

void CardGame::drawCard(){
    for(auto card : playerHand){
        AnimationData animation;
        auto pos = card->getPosition();
        pos.x -= 0.05;
        pos.z -= 0.001;
        animation.setDestinationPoint(pos);
        g_AnimationManager.addAnimatedObject(card, animation);
    }
    // Cria nova carta e configura
    auto newCard = new Card(g_mapModels["plane"],0, 3, 1, 1);
    newCard->setScale(0.15, 0.2, 1.0);
    newCard->setEulerAngleX(1.309);
    
    // Posiciona a carta na frente da câmera
    auto cameraPos = room.getCamera().getPosition();
    auto cameraW = room.getCamera().getWVec();
    auto cameraV = room.getCamera().getVVec();
    newCard->setPosition((cameraPos + (-cameraV * glm::vec4(0.4, 0.4, 0.4, 1.0))) + (-cameraW * glm::vec4(0.3, 0.3, 0.3, 1.0)));

    playerHand.push_back(newCard);
    auto objName = newCard->getObjName();
    objects[objName] = newCard;


}
void CardGame::playCard(Card* card, uint8_t pos){

}

void CardGame::sacrificeSelectedCards(){

}

void CardGame::reduceHealth(bool playerOrCPU){

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

// void CratePuzzle::drawObjects(){
//     auto obj_crate_9 = Puzzle::getObject("crate");

//     glm::mat4 model = Matrix_Identity(); 
//     model = Matrix_Scale(4.0f, 4.0f, 4.0f);
//     glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
//     obj_crate_9->draw(room.getLightSource());
// }

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
prev_time((float)glfwGetTime()),
gnome_position(0.0f, 0.0f, 0.0f, 1.0f)
{

}

void GnomePuzzle::setupRoom(){
    // Câmera está olhando em direção ao gnomo e um pouco acima dele (offset no Y)
    Camera camera(gnome_position + glm::vec4(0.0f, 0.25f, -1.0f, 0.0f));    

    glm::vec4 lightPosition = camera.getPosition();
    LightSource lightSource(lightPosition);

    room.setCamera(camera);
    room.setLightSource(lightSource);
    room.setBackgroundColor(WHITE_BACKGROUND_COLOR);
}

void GnomePuzzle::updateState(){
    return;
}


void GnomePuzzle::updateCamera(){
    room.getCamera().setPositionFree(gnome_position + glm::vec4(0.0f, 0.25f, -1.0f, 0.0f));
    Puzzle::updateCamera();
}

// void GnomePuzzle::drawObjects(){
//     auto obj_gnome = Puzzle::getObject("gnome");

//     glm::mat4 model = Matrix_Identity(); 
//     moveGnome(model);
//     model = model * Matrix_Rotate_Y(-M_PI_2);
//     glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
//     obj_gnome->draw(room.getLightSource());

//     // Utilizado para teste de colisão
//     model = Matrix_Translate(-0.5f, 0.0f, 0.0f);
//     glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
//     obj_gnome->draw(room.getLightSource());
// }

void GnomePuzzle::moveGnome(glm::mat4& model){
    static float speed = 0.5f;

    float current_time = (float)glfwGetTime();
    float delta_t = current_time - prev_time;
    prev_time = current_time;

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
    
    model = model * Matrix_Translate(gnome_position.x, gnome_position.y, gnome_position.z);
}