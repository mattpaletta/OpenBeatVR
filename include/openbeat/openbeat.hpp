#pragma once
#include <engine/game.hpp>
#include <engine/engine.hpp>
#include <constants/texture.hpp>
#include <constants/shader.hpp>

#include "cube.hpp"

class OpenBeat final : public Game {
private:
	std::string root_path;
    CubeInst cubes;

public:
	OpenBeat(const ScreenSize& size, const std::string& _root_path);
    ~OpenBeat();

	void Init() override;

    // game loop
    void ProcessInput(const double& dt) noexcept override;
    void Update(const double& dt) noexcept override;
    void Render() const noexcept override;

    // void pressed(const int key);
    // void released(const int key);
};