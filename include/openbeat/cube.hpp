#pragma once
#include <engine/engine_fwd.hpp>
#include <engine/3d_renderer.hpp>
#include <engine/game_object_inst.hpp>

class CubeInst;

class Cube : public GameObject {
private:
    friend CubeInst;

    unsigned int VBO;
    unsigned int VAO;

    mutable Texture2D texture;
    mutable Shader shader;
public:
    Cube();
    ~Cube();
    
    void Init(Engine* engine);
    void Draw(Renderer3D* renderer) const noexcept;
    void DrawInstance(Renderer3D* renderer, const glm::mat4& model) const noexcept;
};

class CubeInst final : public GameObjectInst<Cube, Position3d> {
public:
    CubeInst();
    ~CubeInst();

    void Init(Engine* engine);

    // We only need 1 `cube`, we draw them all through 'instances'
    Cube cube;

    void Draw(Renderer3D* renderer) const noexcept;
};
