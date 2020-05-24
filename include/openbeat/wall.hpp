#pragma once
#include <engine/engine_fwd.hpp>
#include <engine/3d_renderer.hpp>
#include <engine/game_object_inst.hpp>

class WallInst;

class Wall final : public GameObject {
private:
	friend WallInst;
	unsigned int VBO;
	unsigned int VAO;

	mutable Texture2D texture;
	mutable Shader shader;

public:
	Wall();
	~Wall();

	void Init(Engine* engine);
	void Draw(Renderer3D* renderer) const noexcept;
	void DrawInstance(Renderer3D* renderer, const glm::mat4& model) const noexcept;
};

class WallInst final : public GameObjectInst<Wall> {
public:
	WallInst();
	~WallInst();

	void Init(Engine* engine);

	Wall wall;

	struct WallDetails {
		Position3d position;
		float width;
		float height;
		bool is_ceiling;
	};

	Position3d scale = glm::vec3(1.0);
	std::vector<WallDetails> details;
	void Draw(Renderer3D* renderer) const noexcept;
};