#pragma once
#include <engine/game.hpp>
#include <engine/engine.hpp>
#include <constants/texture.hpp>
#include <constants/shader.hpp>

#include "beatsaber.hpp"
#include "cube.hpp"

class OpenBeat final : public Game {
private:
    double songOffset; // Offset since the beginning of the song.

    std::string root_path;
    CubeInst cubes;
    std::vector<BeatSaberSong> songs;

    BeatSaberSong currentSong;
    BeatSaberLevel currentLevel;

    // Temp
    std::size_t frame_count;

public:
	OpenBeat(const ScreenSize& size, const std::string& _root_path);
    ~OpenBeat();

	void Init() override;

    void LoadLevel(const std::size_t& level_id);

    void SpawnCube(const Note& note) noexcept;
    Colour get_note_colour(const Note& note) const noexcept;

    // game loop
    void ProcessInput(const double& dt) noexcept override;
    void Update(const double& dt) noexcept override;
    void Render() const noexcept override;

    // void pressed(const int key);
    // void released(const int key);
};