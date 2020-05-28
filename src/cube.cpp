#include "openbeat/cube.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <engine/debug.hpp>
#include <engine/engine.hpp>

Cube::Cube() {}

void Cube::Init(Engine* engine) {
    this->model.loadModel(engine, "../models/cube.obj");
    for (std::size_t i = 0; i < this->model.numMeshes(); ++i) {
        auto& mesh = this->model.getMesh(i);
        mesh.autoCreateShader({mesh.fragmentOutColour + " = vec4(1.0, 1.0, 1.0, 1.0);"});
    }
    this->model.Init();
    /*
    // set up vertex data (and buffer(s)) and configure vertex attributes of cube
    constexpr float vertices[] = {
        // Vertex             // Texture
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glCheckError();

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glCheckError();

    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glCheckError();

    // TODO: Make these instanced, separate VAO per-cube

    glBindVertexArray(0);

    this->texture = engine->getResourceManager()->LoadTexture("../textures/container.jpg", "container");
    this->shader = engine->getResourceManager()->LoadShader("../shaders/cube_shader.vert", "../shaders/cube_shader.frag", "cube");

    // Let the resource manager we are storing it ourselves
    engine->getResourceManager()->SetShaderAsSelfUsed("cube");
    engine->getResourceManager()->SetTextureAsSelfUsed("container");

    assert(this->shader.valid());
    this->shader.\
        use().\
        setInt("texture1", 0);
    */

}

Cube::~Cube() {
    //glDeleteVertexArrays(1, &this->VAO);
    //glDeleteBuffers(1, &this->VBO);
}

void Cube::Draw(Renderer3D* renderer) const noexcept {
    /*
    glActiveTexture(GL_TEXTURE0);
    this->texture.Bind();
    this->shader.use();
    glCheckError();

    // pass transformation matrices to the shader
    shader.\
        setMat4("projection", renderer->getProjection()).
        setMat4("view", renderer->getView());
    glCheckError();
    
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glCheckError();
    */
   
}

void Cube::DrawInstance(Renderer3D* renderer, const glm::mat4& model) const noexcept {
    /*this->shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glCheckError();*/
    this->model.Draw(model);
}

CubeInst::CubeInst() {}
CubeInst::~CubeInst() {}

void CubeInst::Init(Engine* engine) {
    this->cube.Init(engine);

    // pass transformation matrices to the shader
/*    this->cube.shader.use().\
        setMat4("projection", engine->get3DRenderer()->getProjection()).
        setMat4("view", engine->get3DRenderer()->getView());*/
    this->cube.model.UpdatePerspective(engine->get3DRenderer());
    glCheckError();
}

void CubeInst::Draw(Renderer3D* renderer) const noexcept {
    /*
    glActiveTexture(GL_TEXTURE0);
    this->cube.texture.Bind();
    this->cube.shader.use();
    glCheckError();
    */

    Colour last_colour = Colour::black;
    //glBindVertexArray(this->cube.VAO);
    for (const auto& cube : this->details) {
        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cube.position);
        model = glm::scale(model, this->scale);
        
        // Rotate around the z-axis by the rotation amount.
        model = glm::rotate(model, cube.rotation, glm::vec3(0.0f, 0.0f, 1.0f));

        if (cube.colour != last_colour) {
            // Only send on change
            //this->cube.shader.setVec3("cubeColour", cube.colour.to_vec3());
            last_colour = cube.colour;
        }
        this->cube.DrawInstance(renderer, model);
    }
    //glBindVertexArray(0);
}
