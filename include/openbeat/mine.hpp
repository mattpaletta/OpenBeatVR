#pragma once
#include <engine/engine_fwd.hpp>
#include <engine/3d_renderer.hpp>
#include <engine/game_object_inst.hpp>

#include <sphere/sphere.hpp>

class MineInst;

class Mine final : public GameObject {
private:
	friend MineInst;

	unsigned int VAO;
	unsigned int VBO;

	mutable Texture2D texture;
	mutable Shader shader;

	Sphere sphere;

public:
	Mine();
	~Mine();

	void Init(Engine* engine);
	void Draw(Renderer3D* renderer) const noexcept;
	void DrawInstance(Renderer3D* renderer, const glm::mat4& model) const noexcept;
};

class MineInst final : public GameObjectInst<Mine> {
public:
	MineInst();
	~MineInst();

	void Init(Engine* engine);

	Mine mine;

	struct MineDetails {
		Position3d position;
	};

	Position3d scale = glm::vec3(1.0);
	std::vector<MineDetails> details;
	void Draw(Renderer3D* renderer) const noexcept;
};