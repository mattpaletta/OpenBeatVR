#include "openbeat/cube.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <engine/debug.hpp>
#include <engine/engine.hpp>

Cube::Cube() {}

Shader& get_cube_shader(Engine* engine) {
    return engine->getResourceManager()->GetShader("cube");
}

void Cube::Init(Engine* engine) {
    // set up vertex data (and buffer(s)) and configure vertex attributes of cube
    constexpr float vertices[] = {
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

    this->texture = engine->getResourceManager()->LoadTexture("../textures/container.jpg", false, "container");
    this->shader = engine->getResourceManager()->LoadShader("../shaders/cube_shader.vert", "../shaders/cube_shader.frag", "", "cube");

    // Let the resource manager we are storing it ourselves
    engine->getResourceManager()->SetShaderAsSelfUsed("cube");
    engine->getResourceManager()->SetTextureAsSelfUsed("container");

    assert(this->shader.valid());
    this->shader.\
        use().\
        setInt("texture1", 0);
}

Cube::~Cube() {
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &this->VBO);
}

void Cube::Draw(Renderer3D* renderer) const noexcept {
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
}

void Cube::DrawInstance(Renderer3D* renderer, const glm::mat4& model) const noexcept {
    this->shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glCheckError();
}

CubeInst::CubeInst() {}
CubeInst::~CubeInst() {}

void CubeInst::Init(Engine* engine) {
    this->cube.Init(engine);

    // world space positions of our cubes
    this->positions = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // pass transformation matrices to the shader
    this->cube.shader.\
        setMat4("projection", engine->get3DRenderer()->getProjection()).
        setMat4("view", engine->get3DRenderer()->getView());
    glCheckError();
}

void CubeInst::Draw(Renderer3D* renderer) const noexcept {
    glActiveTexture(GL_TEXTURE0);
    this->cube.texture.Bind();
    this->cube.shader.use();
    glCheckError();

    for (unsigned int i = 0; i < this->positions.size(); ++i) {
        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, this->positions[i]);
        // Update new position
        const float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        this->cube.DrawInstance(renderer, model);
    }
}
