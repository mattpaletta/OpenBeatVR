#include "openbeat/openbeat.hpp"
#include "openbeat/beatsaber.hpp"

#include <engine/debug.hpp>

#include <constants/position.hpp> // glm::mat4::to_string

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <future>

// Trivial constructor.
OpenBeat::OpenBeat(const ScreenSize& size, const std::string& _root_path) : Game(size, "openbeat"), root_path(_root_path) {}

OpenBeat::~OpenBeat() {}

void OpenBeat::Init() {
	auto beat_saber_path = GetBeatSaberLocation(this->root_path);
	auto load_levels = getBeatSaberFunc();

	std::vector<BeatSaberSong> levels;

#if ENGINE_ENABLE_MULTITHREADED
	std::vector<std::future<BeatSaberSong>> futures;
	for (const auto& p : std::filesystem::directory_iterator(beat_saber_path)) {
		futures.emplace_back(std::async(std::launch::async, load_levels, p.path().string()));
	}

	for (auto& fut : futures) {
		levels.emplace_back(std::move(fut.get()));
	}
#else
	for (const auto& p : std::filesystem::directory_iterator(beat_saber_path)) {
		//levels.emplace_back( load_levels(p.path().string()) );
	}
#endif
    // create transformations
    glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float) this->window_size.WIDTH / (float) this->window_size.HEIGHT, 0.1f, 100.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    // Set renderer.
    this->engine->get3DRenderer()->setProjectionMatrix(projection);
    this->engine->get3DRenderer()->setViewMatrix(view);

	this->cubes.Init(this->engine);
}

void OpenBeat::ProcessInput(const double& dt) noexcept {}

void OpenBeat::Update(const double& dt) noexcept {}

void OpenBeat::Render() const noexcept {
	auto renderer = this->engine->get3DRenderer();
	this->cubes.Draw(renderer);
}