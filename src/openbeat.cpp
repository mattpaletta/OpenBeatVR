#include "openbeat/openbeat.hpp"

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

#if ENGINE_ENABLE_MULTITHREADED
	std::vector<std::future<BeatSaberSong>> futures;
	for (const auto& p : std::filesystem::directory_iterator(beat_saber_path)) {
		futures.emplace_back(std::async(std::launch::async, load_levels, p.path().string()));
	}

	for (auto& fut : futures) {
		this->songs.emplace_back(std::move(fut.get()));
	}
#else
	for (const auto& p : std::filesystem::directory_iterator(beat_saber_path)) {
		this->songs.emplace_back( load_levels(p.path().string()) );
		// Get 5 levels so far
		if (this->songs.size() > 5) {
			break;
		}
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

	// Load Level (for now)
	this->LoadLevel(0);
}

void OpenBeat::LoadLevel(const std::size_t& level_id) {
	if (level_id >= this->songs.size()) {
		std::cerr << "Invalid level ID: " << level_id << std::endl;
		return;
	}
	this->currentSong = this->songs.at(level_id);
	this->currentLevel = this->currentSong.levels.at(0);
	
	std::cout << "Current Song: " << this->currentSong.info.songName << " difficulty: " << (int) this->currentLevel.difficulty << std::endl;

	// Load music so it's ready.
	const std::string songFilename = this->currentSong.info.songFilename;
	const std::string songName = this->currentSong.info.songName;
	this->engine->getAudioEngine()->LoadSound(
		this->engine->getResourceManager()->RegisterSound(songFilename, songName),
		// Not 3D, not Looping, not Streaming.
		false, false, false);
	this->engine->getAudioEngine()->Play(this->engine->getResourceManager()->GetSound(this->currentSong.info.songName));
	this->songOffset = -1.0f;
}

void OpenBeat::ProcessInput(const double& dt) noexcept {}

Colour OpenBeat::get_note_colour(const Note& note) const noexcept {
	// TODO: ability to change colours on demand.
	switch (note.hand) {
	case NoteType::LEFT:
		return Colour::red;
	case NoteType::RIGHT:
		return Colour::blue;
	}
};

void OpenBeat::Update(const double& dt) noexcept {
	bool is_playing = this->engine->getAudioEngine()->IsPlaying(this->engine->getResourceManager()->GetSound(this->currentSong.info.songName));
	if (!is_playing && this->songOffset < 0.0) {
		// Mark the beginning of the song.
		this->songOffset = 0.0f;
	} else if (!is_playing) {
		// Paused so do nothing.
		return;
	} else {
		this->songOffset += dt;
		this->frame_count = ((this->frame_count + 1) % 25);
		if (this->frame_count == 0) {
			std::cout << "Song Offset: " << this->songOffset << std::endl;
		}
	}

	constexpr double cube_speed = 1;
	// Move all cubes towards the camera
	for (auto& detail : this->cubes.details) {
		detail.position.z += (float) (cube_speed * dt);
	}

	for (const auto& note : this->currentLevel.notes) {
		// Look for new notes to spawn, start with just bottom notes.
		if (note.lineLayer == 0) {
			if (note.timeInSeconds <= (this->songOffset + dt) && note.timeInSeconds > this->songOffset) {
				// Spawn note.
				this->SpawnCube(note);
			}
		}
	}

	// Despawn any that go past our camera location.
	this->cubes.details.erase(std::remove_if(this->cubes.details.begin(), this->cubes.details.end(), [](const CubeInst::CubeDetails detail) {
		constexpr float margin = 20;
		return detail.position.z < (-3.0f - margin);
	}), this->cubes.details.end());
}

void OpenBeat::Render() const noexcept {
	auto renderer = this->engine->get3DRenderer();
	this->cubes.Draw(renderer);
}

void OpenBeat::SpawnCube(const Note& note) noexcept {
	constexpr float vertical_offset = -1.5;
	constexpr float vertical_gap = 1.0;
	constexpr std::array<float, 2> lane_y = {
		vertical_offset, // Height of lower blocks
		vertical_offset + vertical_gap  // Height of 'upper' blocks
	};
	
	constexpr float spawn_point_z = -10; // Distance from user

	constexpr float horizontal_gap = 0.8;
	// Allows for more horizontal lines, final game has 4-6
	constexpr std::array<float, 4> lane_x = {
		-2 * horizontal_gap,
		-1 * horizontal_gap,
		 1 * horizontal_gap,
		 2 * horizontal_gap
	};

	if (note.lineIndex >= lane_x.size()) {
		std::cout << "Got larger Line Index: " << note.lineIndex << std::endl;
		return;
	}

	if (note.lineLayer >= lane_y.size()) {
		std::cout << "got Larger line layer: " << note.lineLayer << std::endl;
		return;
	}

	CubeInst::CubeDetails details;
	
	details.position = Position(
		lane_x[note.lineIndex],
		lane_y[note.lineLayer],
		spawn_point_z);
	details.colour = this->get_note_colour(note);
	details.rotation = note.get_rotation();
	this->cubes.details.emplace_back(std::move(details));
}