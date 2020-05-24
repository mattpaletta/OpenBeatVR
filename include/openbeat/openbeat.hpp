#pragma once
#include <engine/game.hpp>
#include <engine/engine.hpp>
#include <constants/texture.hpp>
#include <constants/shader.hpp>

#include "beatsaber.hpp"
#include "cube.hpp"
#include "wall.hpp"
#include "mine.hpp"

constexpr float BEAT_WARMUP_OFFSET = 25.f;
constexpr float BEAT_WARMUP_SPEED = 155.f;
constexpr float BEAT_WARMUP_TIME = (BEAT_WARMUP_OFFSET / BEAT_WARMUP_SPEED); //* 1000.f;
constexpr float BEAT_ANTICIPATION_POSITION = 0;

class OpenBeat final : public Game {
private:
    // Offset of the player.
    float swordOffset = 1.5;
    float beatSpeed;
    const float beatAnticipationTime = 1.1;
    float anticipationPosition;
    const float warmupPosition = -BEAT_WARMUP_TIME * BEAT_WARMUP_SPEED;
    float speed;
    const float size = 0.4f;

    double songOffset; // Offset since the beginning of the song.

    std::string root_path;
    CubeInst cubes;
    MineInst mines;
    WallInst walls;
    std::vector<BeatSaberSong> songs;

    BeatSaberSong currentSong;
    BeatSaberLevel currentLevel;

    // Temp
    std::size_t frame_count;

    Position get_cube_spawn_position(const Note& note) const noexcept;

public:
	OpenBeat(const ScreenSize& size, const std::string& _root_path);
    ~OpenBeat();

	void Init() override;

    void LoadLevel(const std::size_t& level_id);

    void SpawnMine(const Note& note) noexcept;
    void SpawnCube(const Note& note) noexcept;
    void SpawnWall(const Obstacle& note) noexcept;
    void SpawnEvent(const Event& note) noexcept;
    Colour get_note_colour(const Note& note) const noexcept;

    // game loop
    void ProcessInput(const double& dt) noexcept override;
    void Update(const double& dt) noexcept override;
    void Render() const noexcept override;

    // void pressed(const int key);
    // void released(const int key);
};