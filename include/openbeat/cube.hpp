#pragma once
#include <constants/colour.hpp>

#include <engine/engine_fwd.hpp>
#include <engine/3d_renderer.hpp>
#include <engine/game_object_inst.hpp>
#include <engine/model.hpp>

class CubeInst;

class Cube final: public GameObject {
private:
    friend CubeInst;

    //unsigned int VBO;
    //unsigned int VAO;

    //mutable Texture2D texture;
    //mutable Shader shader;
    Model model;

public:
    Cube();
    ~Cube();
    
    void Init(Engine* engine);
    void Draw(Renderer3D* renderer) const noexcept;
    void DrawInstance(Renderer3D* renderer, const glm::mat4& model) const noexcept;
};

class CubeInst final : public GameObjectInst<Cube> {
public:
    CubeInst();
    ~CubeInst();

    void Init(Engine* engine);

    // We only need 1 `cube`, we draw them all through 'instances'
    Cube cube;

    struct CubeDetails {
         Position3d position;
         float rotation;
         Colour colour;
    };

    Position3d scale = glm::vec3(1.0);
    std::vector<CubeDetails> details;
    void Draw(Renderer3D* renderer) const noexcept;
};
