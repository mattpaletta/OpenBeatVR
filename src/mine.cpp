#include "openbeat/mine.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <engine/debug.hpp>
#include <engine/engine.hpp>

// Radius 0.5, 36 sectors (horizontal), 18 stacks (vertical), smooth=true
Mine::Mine() : sphere(0.2, 36, 18, false) {}

Mine::~Mine() {
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
}

void Mine::Init(Engine* engine) {
	this->shader = engine->getResourceManager()->LoadShader("../shaders/mine_shader.vert", "../shaders/mine_shader.frag", "", "mine");
	this->shader.use();

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);

	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->sphere.getInterleavedVertexSize(), this->sphere.getInterleavedVertices().data(), GL_STATIC_DRAW);
	glCheckError();

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 + 3 + 2) * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	glCheckError();

	// normal coord attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (3 + 3 + 2) * sizeof(float), (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (3 + 3 + 2) * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glCheckError();

	glBindVertexArray(0);

	// Let the resource manager we are storing it ourselves
	engine->getResourceManager()->SetShaderAsSelfUsed("mine");

	// All mines are the same colour, so just send it once.
	assert(this->shader.valid());
	this->shader.\
		use().\
		setVec3("mineColour", Colour::from_hex(0xDDDDDD).to_vec3());
}

void Mine::DrawInstance(Renderer3D* renderer, const glm::mat4& model) const noexcept {
	this->shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, this->sphere.getInterleavedVertexCount());
}

MineInst::MineInst() {}
MineInst::~MineInst() {}

void MineInst::Init(Engine* engine) {
	this->mine.Init(engine);

	this->mine.shader.\
		setMat4("projection", engine->get3DRenderer()->getProjection()).
		setMat4("view", engine->get3DRenderer()->getView());
	glCheckError();
}

void MineInst::Draw(Renderer3D* renderer) const noexcept {
	//glActiveTexture(GL_TEXTURE0);
	//this->mine.texture.Bind();
	this->mine.shader.use();
	glCheckError();

	glBindVertexArray(this->mine.VAO);
	for (const auto& mine : this->details) {
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, mine.position);
		model = glm::scale(model, this->scale);
		
		// No rotation
		this->mine.DrawInstance(renderer, model);
	}
	glBindVertexArray(0);
}