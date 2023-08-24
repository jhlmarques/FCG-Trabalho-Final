#include "puzzle.h"


void Puzzle::update(){
    glm::vec4 bg = room.getBackgroundColor();
    glClearColor(bg.r, bg.g, bg.b, bg.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateCamera();
    handleInputs();
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


MainLobby::MainLobby(){
    tileVector.emplace_back(0.0f, 0.0f, 0.0f);
    tileVector.emplace_back(0.0f, 0.0f, TILE_WIDTH); // n
    tileVector.emplace_back(TILE_WIDTH, 0.0f, 0.0f); // w
    tileVector.emplace_back(0.0f, 0.0f, -TILE_WIDTH);// s
    tileVector.emplace_back(-TILE_WIDTH, 0.0f, 0.0f); // e

    tileVector.emplace_back(TILE_WIDTH, 0.0f, TILE_WIDTH); // NW
    tileVector.emplace_back(TILE_WIDTH, 0.0f, -TILE_WIDTH); // SW 
    tileVector.emplace_back(-TILE_WIDTH, 0.0f, -TILE_WIDTH); // SE
    tileVector.emplace_back(-TILE_WIDTH, 0.0f, TILE_WIDTH); // NE

    tileVector[0].setNorth(&tileVector[1]);
    tileVector[0].setWest(&tileVector[2]);
    tileVector[0].setSouth(&tileVector[3]);
    tileVector[0].setEast(&tileVector[4]);

    tileVector[1].setWest(&tileVector[5]);
    tileVector[2].setNorth(&tileVector[5]);

    tileVector[3].setWest(&tileVector[6]);
    tileVector[2].setSouth(&tileVector[6]);

    tileVector[4].setSouth(&tileVector[7]);
    tileVector[3].setEast(&tileVector[7]);

    tileVector[1].setEast(&tileVector[8]);
    tileVector[4].setNorth(&tileVector[8]);

    tileVector[1].addObject(Puzzle::getObject("bunny"));
    tileVector[2].addObject(Puzzle::getObject("bunny"));
    tileVector[3].addObject(Puzzle::getObject("statue"));

    cur_tile = &tileVector[0];
}

void MainLobby::setupRoom(){
    // Obtém a posição inicial da câmera 
    auto tileCenter = cur_tile->getCenterPos();
    tileCenter.y += CAMERA_HEAD_HEIGHT;

    //  Câmera do lobby principal (inicialmente em modo "free")
    room.setCamera(Camera(tileCenter));
    // Iluminação do lobby principal
    glm::vec4 lightPosition = glm::vec4(TILE_WIDTH,5.0f,0.0f,1.0f);
    LightSource lightSource(lightPosition);
    room.setLightSource(lightSource);

}

void MainLobby::handleInputs(){
    // Apenas realizamos um movimento se a câmera não está animando
    if(!room.getCamera().animate()){
        cur_tile->handleMovement(&cur_tile, room.getCamera());
    }
}

void MainLobby::drawObjects(){
    glm::mat4 model = Matrix_Identity();
    auto obj_tile = Puzzle::getObject("tile");
    auto obj_statue = Puzzle::getObject("statue");

    // DESENHA TILES
    for(auto tile : tileVector){
        auto coords = tile.getCenterPos();
        auto scale = Matrix_Scale(TILE_WIDTH/2.0f, 1.0f, TILE_WIDTH/2.0f);
        
        // Escala para o tamanho do tile para cobrir espaçamento entre tiles e deslocamento para o centro do tile
        model = Matrix_Translate(coords.x, coords.y, coords.z) * scale;
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
        obj_tile->draw(room.getLightSource());
    }

    // DESENHA OBJETOS
    // TO-DO: Usar uma classe que represente um "interativo" - Um modelo 3D junto
    // de um hitbox, possivelmente com alguma lógica interna

    // Uma cadeira no tile norte
    auto pos = tileVector[1].getCenterPos();
    model =  Matrix_Translate(pos.x, pos.y, pos.z) * Matrix_Rotate_Y(M_PI) * Matrix_Scale(4.0f, 4.0f, 4.0f);
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


void CratePuzzle::handleInputs(){
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