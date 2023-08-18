#include "gameobject.h"



// Carrega o objeto de índice "shape" de um arquivo .obj carregado
// É possível que dê problemas para .obj com mais de um shape; cuidado (TO-DO)
GameObject::GameObject(ObjModel* model, GLuint shapeIdx) :
    textureScale(1.0), type(OBJ_GENERIC), defaultTexture("textures/default.jpeg")
{

    std::vector<float>  model_coefficients;
    std::vector<float>  normal_coefficients;
    std::vector<float>  texture_coefficients;
    // Os índices para o vetor de índices
    std::vector<GLuint> indices;

    const float minval = std::numeric_limits<float>::min();
    const float maxval = std::numeric_limits<float>::max();

    glm::vec3 bbox_min = glm::vec3(maxval,maxval,maxval);
    glm::vec3 bbox_max = glm::vec3(minval,minval,minval);

    //https://stackoverflow.com/questions/67638439/how-do-i-draw-an-obj-file-in-opengl-using-tinyobjloader

    // Um objeto dentro do arquivo .obj
    tinyobj::shape_t shape = model->shapes[shapeIdx];

    // Vetor de índices de faces
    // mesh contém informação das faces, e indices as coordenadas de vértice, normais, e textura
    auto const& mesh_indices = shape.mesh.indices;
    // Materiais contidos no .obj
    auto const& model_materials = model->materials;
    // Ids dos materiais de cada face
    auto const& matIds = shape.mesh.material_ids;

    bool hasMaterials = model_materials.size() > 0;
    int curMatIdx = -1; // O índice de material atual, usado para checar se o material trocou entre as faces
    Material* curMat;

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


            if(hasMaterials){
                const int idx = matIds[face_idx];
                // Checa se essa face tem um material diferente
                // Se sim, troca textura
                // TO-DO: Deixar mais eficiente
                if(idx != curMatIdx){
                    const auto& material = model_materials[idx];
                    materials.emplace_back(cur_idx, material);
                    curMat = &materials.back();
                    curMatIdx = idx;
                }

                // Conta mais um índice aplicado ao material
                curMat->addIdx();

            }

        }

    }

    name           = shape.name;
    first_index    = 0; // Primeiro índice
    num_indices    = indices.size(); // Número de indices
    rendering_mode = GL_TRIANGLES;       // Índices correspondem ao tipo de rasterização GL_TRIANGLES.

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



void GameObject::draw(){
    
    glBindVertexArray(vertex_array_object_id);

    // Variáveis enviadas para o fragment shader

    // AABB
    glUniform4f(g_bbox_min_uniform, bbox_min.x, bbox_min.y, bbox_min.z, 1.0f);
    glUniform4f(g_bbox_max_uniform, bbox_max.x, bbox_max.y, bbox_max.z, 1.0f);

    // Iluminação

    // Informações adicionais
    glUniform1f(g_object_texture_scale, textureScale);
    glUniform1i(g_object_type_uniform, type);
    
    // Draw
    if(!materials.size()){
        // Bind à texture unit 0
        defaultTexture.bind(0);
        glDrawElements(
            GL_TRIANGLES,
            num_indices,
            GL_UNSIGNED_INT,
            (void*)(first_index * sizeof(GLuint))
        );
        defaultTexture.unbind();
    }
    else{
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
    }


    glBindVertexArray(0);
    
}

void GameObject::setTextureScale(float scale){
    textureScale = scale;
}
