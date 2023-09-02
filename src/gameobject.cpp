#include "gameobject.h"



// Carrega o objeto de índice "shape" de um arquivo .obj carregado
// É possível que dê problemas para .obj com mais de um shape; cuidado (TO-DO)
GameObject::GameObject(ObjModel* model, GLuint shapeIdx) :
    textureScale(1.0), type(OBJ_GENERIC), illumination_model(BLINN_PHONG),
    position{0.0, 0.0, 0.0, 1.0},
    scaleMatrix(Matrix_Identity()), rotationMatrix(Matrix_Identity()),
    eulerAngleX(0.0), eulerAngleY(0.0), eulerAngleZ(0.0)
{
    
    std::vector<float>  model_coefficients;
    std::vector<float>  normal_coefficients;
    std::vector<float>  texture_coefficients;
    // Os índices para o vetor de índices
    std::vector<GLuint> indices;

    const float minval = std::numeric_limits<float>::min();
    const float maxval = std::numeric_limits<float>::max();

    bbox_min = glm::vec3(maxval,maxval,maxval);
    bbox_max = glm::vec3(minval,minval,minval);

    ComputeNormals(model);
    

    //https://stackoverflow.com/questions/67638439/how-do-i-draw-an-obj-file-in-opengl-using-tinyobjloader
    // Um objeto dentro do arquivo .obj
    tinyobj::shape_t shape = model->shapes[shapeIdx];

    // Vetor de índices de faces
    // mesh contém informação das faces, e indices as coordenadas de vértice, normais, e textura
    auto const& mesh_indices = shape.mesh.indices;
    // Materiais contidos no .obj
    auto const& model_materials = model->materials;
    // Ids dos materiais de cada face
    auto const& mat_ids = shape.mesh.material_ids;
    
    bool hasMaterials = model_materials.size() > 0;

    int cur_mat_idx = mat_ids[0]; // O índice de material atual, usado para checar se o material trocou entre as faces
    int cur_mat_first_index = 0;
    int cur_mat_idx_count = 0; // O número de índices de vértices sobre os quais o material está aplicado

    printf("Carregando objeto \"%s\"\n", shape.name.c_str());
    
    if(hasMaterials){
        printf("%d materiais presentes\n", model_materials.size());
    }


    // Percorremos as faces (3 vértices)
    for(size_t face_idx = 0; face_idx < shape.mesh.num_face_vertices.size(); face_idx++){
        // Para cada índice da face (vértice, normal, textura)
        for(size_t vertex_idx = 0; vertex_idx < 3; vertex_idx++){
            const size_t cur_idx = 3*face_idx + vertex_idx;
            indices.push_back(cur_idx);
            
            tinyobj::index_t idx = mesh_indices[3*face_idx + vertex_idx];
            // Coordenadas de vértices (índice de vértice)
            const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
            const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
            const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
            model_coefficients.push_back( vx ); // X
            model_coefficients.push_back( vy ); // Y
            model_coefficients.push_back( vz ); // Z
            model_coefficients.push_back( 1.0f ); // W
            
            // Bounding box
            bbox_min.x = std::min(bbox_min.x, vx);
            bbox_min.y = std::min(bbox_min.y, vy);
            bbox_min.z = std::min(bbox_min.z, vz);
            bbox_max.x = std::max(bbox_max.x, vx);
            bbox_max.y = std::max(bbox_max.y, vy);
            bbox_max.z = std::max(bbox_max.z, vz);

            // Normais (índice de normal)
            if ( idx.normal_index != -1 ){ // Checa se há normal
                const float nx = model->attrib.normals[3*idx.normal_index + 0];
                const float ny = model->attrib.normals[3*idx.normal_index + 1];
                const float nz = model->attrib.normals[3*idx.normal_index + 2];
                normal_coefficients.push_back( nx ); // X
                normal_coefficients.push_back( ny ); // Y
                normal_coefficients.push_back( nz ); // Z
                normal_coefficients.push_back( 0.0f ); // W
            }

            // Coordenadas de textura
            if ( idx.texcoord_index != -1 ){ // Checa se há textura
                const float u = model->attrib.texcoords[2*idx.texcoord_index + 0];
                const float v = model->attrib.texcoords[2*idx.texcoord_index + 1];
                texture_coefficients.push_back( u );
                texture_coefficients.push_back( v );
            }

        }

        if(hasMaterials){

            const int idx = mat_ids[face_idx];
            // Checa se essa face tem um material diferente
            if(idx != cur_mat_idx){
                // Se sim, guarda o material que estava até agora sendo utilizado
                const auto& material = model_materials[cur_mat_idx];
                materials.push_back(Material::createFromObjFile(cur_mat_first_index, cur_mat_idx_count, material));
                // Começa a contar os índices que o novo material se aplica
                cur_mat_first_index = 3*(face_idx);
                cur_mat_idx_count = 0;
                cur_mat_idx = idx;
            }
            
            cur_mat_idx_count += 3;

        }

    }

    // Cria último material
    if(hasMaterials){
        const auto& material = model_materials[cur_mat_idx];
        materials.push_back(Material::createFromObjFile(cur_mat_first_index, cur_mat_idx_count, material));
    }

    name           = shape.name;
    first_index    = 0; // Primeiro índice
    num_indices    = indices.size(); // Número de indices
    rendering_mode = GL_TRIANGLES;       // Índices correspondem ao tipo de rasterização GL_TRIANGLES.

    // Se não tem materiais, preenche com material default
    if(!hasMaterials){
        printf("Usando material default\n");
        materials.push_back(Material::createFromTexture(0, indices.size(), Texture("textures/default.jpeg")));
    }



    // Envia triângulos para a GPU

    // Envio para a GPU
    // Geramos um VAO (Vertix Array Object) que irá conter os buffers necessários
    // para renderização do objeto
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    // VBOs

    // Coordenadas de vértices

    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, model_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, model_coefficients.size() * sizeof(float), model_coefficients.data());
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind

    // Normais (se houver)
    // TO-DO: Mover cálculo de normais com base nos triângulos para dentro desta classe?

    if ( !normal_coefficients.empty() )
    {
        GLuint VBO_normal_coefficients_id;
        glGenBuffers(1, &VBO_normal_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, normal_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normal_coefficients.size() * sizeof(float), normal_coefficients.data());
        location = 1; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Coordenadas de Texturas

    if ( !texture_coefficients.empty() )
    {
        GLuint VBO_texture_coefficients_id;
        glGenBuffers(1, &VBO_texture_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, texture_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, texture_coefficients.size() * sizeof(float), texture_coefficients.data());
        location = 2; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 2; // vec2 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Buffer de índices
    // Isso talvez cause problemas? Envolvendo o .attrib do objModel
    GLuint indices_id;
    glGenBuffers(1, &indices_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());
    glBindVertexArray(0);


}



void GameObject::draw(LightSource& lightSource){
    
    glBindVertexArray(vertex_array_object_id);

    // Variáveis enviadas para o fragment shader

    // AABB
    glUniform4f(g_bbox_min_uniform, bbox_min.x, bbox_min.y, bbox_min.z, 1.0f);
    glUniform4f(g_bbox_max_uniform, bbox_max.x, bbox_max.y, bbox_max.z, 1.0f);

    // Fonte de luz da sala incidente no objeto. 
    static GLuint light_position_coefficient = glGetUniformLocation(g_GpuProgramID, "light_position");
    glm::vec4 lightPosition = lightSource.getPosition();
    glUniform4f(light_position_coefficient, lightPosition.x, lightPosition.y, lightPosition.z, 1.0f);

    glm::vec4 lightDirection = lightSource.getDirection();
    static GLuint light_direction_coefficient = glGetUniformLocation(g_GpuProgramID, "light_direction");
    glUniform4f(light_direction_coefficient, lightDirection.x, lightDirection.y, lightDirection.z, 1.0f);

    float lightApertureAngle = lightSource.getApertureAngle();
    static GLuint light_aperture_angle_coefficient = glGetUniformLocation(g_GpuProgramID, "light_aperture_angle");
    glUniform1f(light_aperture_angle_coefficient, lightApertureAngle);

    // Modelo de iluminação do objeto
    static GLuint illumination_model_coefficient = glGetUniformLocation(g_GpuProgramID, "illumination_model");
    glUniform1i(illumination_model_coefficient, illumination_model);

    // Informações adicionais
    glUniform1f(g_object_texture_scale, textureScale);
    glUniform1i(g_object_type_uniform, type);
    

    // Materiais
    for(auto mat : materials){
        mat.bindToShader();
        glDrawElements(
            GL_TRIANGLES,
            mat.getNumIndices(),
            GL_UNSIGNED_INT,
            (void*)(mat.getFirstIndex() * sizeof(GLuint))
        );
        mat.unbind();
    }


    glBindVertexArray(0);
    
}

void GameObject::setTextureScale(float scale){
    textureScale = scale;
}

void GameObject::setIlluminationModel(int illumination_model){
    this->illumination_model = illumination_model;
}


void GameObject::setPosition(glm::vec4 pos){
    position = pos;
}

glm::vec4 GameObject::getPosition(){
    return position;
}

void GameObject::setScale(float x, float y, float z){
    this->scaleMatrix = Matrix_Scale(x, y, z);
}

glm::mat4 GameObject::getScaleMatrix(){
    return scaleMatrix;
}

glm::mat4 GameObject::getRotationMatrix(){
    return rotationMatrix;
}

void GameObject::setEulerAngleX(float angle){
    eulerAngleX = angle;
    rotationMatrix = Matrix_Rotate_X(eulerAngleX) * Matrix_Rotate_Y(eulerAngleY) * Matrix_Rotate_Z(eulerAngleZ);
}

float GameObject::getEulerAngleX(){
    return eulerAngleX;
}

void GameObject::setEulerAngleY(float angle){
    eulerAngleY = angle;
    rotationMatrix = Matrix_Rotate_X(eulerAngleX) * Matrix_Rotate_Y(eulerAngleY) * Matrix_Rotate_Z(eulerAngleZ);
}

float GameObject::getEulerAngleY(){
    return eulerAngleY;
}

void GameObject::setEulerAngleZ(float angle){
    eulerAngleZ = angle;
    rotationMatrix = Matrix_Rotate_X(eulerAngleX) * Matrix_Rotate_Y(eulerAngleY) * Matrix_Rotate_Z(eulerAngleZ);
}

float GameObject::getEulerAngleZ(){
    return eulerAngleZ;
}

glm::vec3 GameObject::getBBoxMaxWorld(){
    glm::vec3 position3d = glm::vec3(position.x, position.y, position.z);
    return bbox_max + position3d;
}

glm::vec3 GameObject::getBBoxMinWorld(){
    glm::vec3 position3d = glm::vec3(position.x, position.y, position.z);
    return bbox_min + position3d;
}

