#include "openbeat/openbeat.hpp"

#include <engine/debug.hpp>

#include <constants/position.hpp> // glm::mat4::to_string

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <future>

float getHorizontalPosition(const float& lineIndex) {
	return lineIndex / 3 * 1.5 - 0.75;
}

float getVerticalPosition(const float& lineLayer) {
	return lineLayer / 2 + 0.7;
}

// Testing helpers
bool find_test_level(const BeatSaberLevel& level) {
	return level.get_num_mines() > 10 && 
		   level.get_num_walls() > 0;
};

bool did_find_test_song(const BeatSaberSong& song) {
	for (const auto& level : song.levels) {
		if (find_test_level(level)) {
			return true;
		}
	}

	return false;
};

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
		auto new_song = load_levels(p.path().string());
		auto did_find = did_find_test_song(new_song);
		this->songs.emplace_back( std::move(new_song) );
		// Get 2 levels so far
		if (did_find) {
			break;
		}
	}
#endif
    // create transformations
    glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 projection = glm::mat4(1.0f);
	constexpr float camera_radius = 45.0f;
	constexpr float camera_height = -1.5f;

	projection = glm::perspective(glm::radians(camera_radius), (float) this->window_size.WIDTH / (float) this->window_size.HEIGHT, 0.1f, 100.0f);
	view = glm::translate(view, glm::vec3(0.0f, camera_height, 0.0f));

    // Set renderer.
    this->engine->get3DRenderer()->setProjectionMatrix(projection);
    this->engine->get3DRenderer()->setViewMatrix(view);

	this->cubes.Init(this->engine);
	this->mines.Init(this->engine);
	this->walls.Init(this->engine);

	// Load Level (for now)
	this->LoadLevel(this->songs.size() - 1);
}

void OpenBeat::LoadLevel(const std::size_t& level_id) {
	if (level_id >= this->songs.size()) {
		std::cerr << "Invalid level ID: " << level_id << std::endl;
		return;
	}
	this->currentSong = this->songs.at(level_id);
	for (const auto& level : this->currentSong.levels) {
		if (find_test_level(level)) {
			this->currentLevel = level;
			break;
		}
	}	
	std::cout << "Current Song: " << this->currentSong.info.songName << std::endl;
	std::cout << "Level Info: " << this->currentLevel.description() << std::endl;

	// Load music so it's ready.
	const std::string songFilename = this->currentSong.info.songFilename;
	const std::string songName = this->currentSong.info.songName;
	this->engine->getAudioEngine()->LoadSound(
		this->engine->getResourceManager()->RegisterSound(songFilename, songName),
		// Not 3D, not Looping, not Streaming.
		false, false, false);
	this->engine->getAudioEngine()->Play(this->engine->getResourceManager()->GetSound(this->currentSong.info.songName));
	
	// Debug engine

	this->songOffset = -1.0f;
	this->beatSpeed = (int) this->currentLevel.difficulty;
	this->anticipationPosition = -this->beatAnticipationTime * beatSpeed - this->swordOffset;
	this->speed = beatSpeed;
	this->cubes.scale = glm::vec3(this->size);
	this->mines.scale = glm::vec3(this->size);
}

void OpenBeat::ProcessInput(const double& dt) noexcept {}

Colour OpenBeat::get_note_colour(const Note& note) const noexcept {
	// TODO: ability to change colours on demand.
	switch (note.hand) {
	case NoteType::LEFT:
		return Colour::red;
	case NoteType::RIGHT:
		return Colour::blue;
	case NoteType::MINE:
		return Colour::black;
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

	// TODO: Keep pointers to last index, only iterate from there.
	auto in_current_offset = [this, &dt](const double& timeInSeconds) {
		return timeInSeconds > (this->songOffset - dt) && timeInSeconds <= this->songOffset;
	};

	// Spawn in new notes
	for (const auto& note : this->currentLevel.notes) {
		// Look for new notes to spawn, start with just bottom notes.
		if (in_current_offset(note.timeInSeconds)) {
			// TODO: add rotation LERP for new spawned cubes
			if (note.hand == NoteType::MINE) {
				this->SpawnMine(note);
			} else {
				// std::cout << "Spawning cube!" << std::endl;
				this->SpawnCube(note);
			}
		}
	}

	// Spawn in Walls
	for (const auto& obstacle : this->currentLevel.obstacles) {
		if (in_current_offset(obstacle.timeInSeconds)) {
			// Spawn wall.
			this->SpawnWall(obstacle);
		}
	}

	// Spawn in Events
	for (const auto& event : this->currentLevel.events) {
		if (in_current_offset(event.timeInSeconds)) {
			this->SpawnEvent(event);
		}
	}

	// Helper function to determine new location of mine or cube based on DT.
	auto update_position = [this](Position* position, const double& dt) {
		if (position->z < this->anticipationPosition) {
			const auto newPositionZ = position->z + BEAT_WARMUP_SPEED * dt; //(dt / 1000);
			// Warm up / warp in.
			if (newPositionZ < this->anticipationPosition) {
				position->z = newPositionZ;
			} else {
				position->z = this->anticipationPosition;
			}
		} else {
			position->z += this->speed * dt; // (dt / 1000);
		}
	};

	// Move all cubes towards the camera
	for (auto& detail : this->cubes.details) {
		update_position(&detail.position, dt);
	}

	// Mines move the same way
	for (auto& detail : this->mines.details) {
		update_position(&detail.position, dt);
	}

	// Despawn any that go past our camera location.
	// Cubes and mines get despawned the same way.
	this->cubes.details.erase(std::remove_if(this->cubes.details.begin(), this->cubes.details.end(), [this](const CubeInst::CubeDetails detail) {
		return detail.position.z > (-this->swordOffset);
	}), this->cubes.details.end());
	this->mines.details.erase(std::remove_if(this->mines.details.begin(), this->mines.details.end(), [this](const MineInst::MineDetails detail) {
		return detail.position.z > (-this->swordOffset);
	}), this->mines.details.end());
}

void OpenBeat::Render() const noexcept {
	auto renderer = this->engine->get3DRenderer();
	this->cubes.Draw(renderer);
	this->mines.Draw(renderer);
	this->walls.Draw(renderer);
}

Position OpenBeat::get_cube_spawn_position(const Note& note) const noexcept {
	return Position(
		getHorizontalPosition(note.lineIndex), 
		getVerticalPosition(note.lineLayer),
		this->anticipationPosition + this->warmupPosition);
}

void OpenBeat::SpawnMine(const Note& note) noexcept {
	MineInst::MineDetails details;
	// They get placed the same way as the cubes.
	details.position = this->get_cube_spawn_position(note);
	this->mines.details.emplace_back(std::move(details));
}

void OpenBeat::SpawnCube(const Note& note) noexcept {
	const Colour colour = this->get_note_colour(note);
	const float rotation = note.get_rotation();

	// TODO: add mapping extensions
	CubeInst::CubeDetails details;
	details.position = this->get_cube_spawn_position(note);
	details.colour = this->get_note_colour(note);
	details.rotation = note.get_rotation();
	this->cubes.details.emplace_back(std::move(details));
}

void OpenBeat::SpawnWall(const Obstacle& obstacle) noexcept {}
void OpenBeat::SpawnEvent(const Event& event) noexcept {}