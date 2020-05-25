#include "openbeat/wall.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <engine/debug.hpp>
#include <engine/engine.hpp>

Wall::Wall() {}
Wall::~Wall() {
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
}

void Wall::Init(Engine* engine) {
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

    this->shader = engine->getResourceManager()->LoadShader("../shaders/wall_shader.vert", "../shaders/wall_shader.frag", "", "wall");

    // Let the resource manager we are storing it ourselves
    engine->getResourceManager()->SetShaderAsSelfUsed("wall");

    assert(this->shader.valid());
    this->shader.use().setVec3("wallColour", Colour::from_hex(0xDD0000).to_vec3());
}

void Wall::DrawInstance(Renderer3D* renderer, const glm::mat4& model) const noexcept {
    this->shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glCheckError();
}

WallInst::WallInst() {}
WallInst::~WallInst() {}

void WallInst::Init(Engine* engine) {
    this->wall.Init(engine);

    // pass transformation matrices to the shader
    this->wall.shader.use().\
        setMat4("projection", engine->get3DRenderer()->getProjection()).
        setMat4("view", engine->get3DRenderer()->getView());
    glCheckError();
}

void WallInst::Draw(Renderer3D* renderer) const noexcept {
    glActiveTexture(GL_TEXTURE0);
    //this->wall.texture.Bind();
    this->wall.shader.use();
    glCheckError();

    glBindVertexArray(this->wall.VAO);
    for (const auto& wall : this->details) {
        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, wall.position);
        model = glm::scale(model, wall.size);
        model = glm::scale(model, this->scale);

        this->wall.DrawInstance(renderer, model);
    }
    glBindVertexArray(0);
}