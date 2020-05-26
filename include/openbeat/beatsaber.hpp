#pragma once

#include <engine/engine.hpp>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

enum class Difficulty { EASY, NORMAL, HARD, EXPERT, EXPERT_PLUS };
enum class NoteType { LEFT, RIGHT, MINE };
enum class CutDirection { BOTTOM, TOP, LEFT, RIGHT, TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT, NONDIRECTION };
enum class ObstacleType { WALL, CEILING };

struct Event {
	double timeInSeconds;
	double time;

	Event(const double& _timeInSeconds, const double& _time) : timeInSeconds(_timeInSeconds), time(_time) {}
	~Event() = default;
};

struct Obstacle {
	double timeInSeconds;
	double time;
	int lineIndex;
	float duration;
	ObstacleType type;
	double width;

	Obstacle(const double& _timeInSeconds, const double& _time, const int _lineIndex, const float& _duration, const ObstacleType& _type, const double& _width) : timeInSeconds(_timeInSeconds), time(_time), lineIndex(_lineIndex), duration(_duration), type(_type), width(_width) {}
	~Obstacle() = default;
};

struct Note {
	double timeInSeconds;
	double time;
	int lineIndex;
	int lineLayer;
	NoteType hand;
	CutDirection direction;
	Note(const double& _timeInSeconds, const double& _time, const int _lineIndex, const int _lineLayer, const NoteType& _hand, const CutDirection& _direction) : timeInSeconds(_timeInSeconds), time(_time), lineIndex(_lineIndex), lineLayer(_lineLayer), hand(_hand), direction(_direction) {}
	~Note() = default;

	constexpr float get_rotation() const noexcept {
		switch (this->direction) {
		case CutDirection::TOP:
			return 0.f;
		case CutDirection::BOTTOM:
			return 180.f;
		case CutDirection::LEFT:
			return 270.f;
		case CutDirection::RIGHT:
			return 90.f;
		case CutDirection::TOP_LEFT:
			return 315.f;
		case CutDirection::TOP_RIGHT:
			return 45.f;
		case CutDirection::BOTTOM_LEFT:
			return 225.f;
		case CutDirection::BOTTOM_RIGHT:
			return 125.f;
		case CutDirection::NONDIRECTION:
			return 0.f;
		}


		return 0.f;
	}
};

struct BeatSaberLevel {
	bool is_filled = false;
	Difficulty difficulty;

	std::vector<Note> notes;
	std::vector<Obstacle> obstacles;
	std::vector<Event> events;


	std::size_t get_num_mines() const {
		std::size_t count = 0;
		for (const auto& note : this->notes) {
			if (note.hand == NoteType::MINE) {
				count += 1;
			}
		}
		return count;
	}

	std::size_t get_num_blocks() const {
		return this->notes.size() - this->get_num_mines();
	}

	std::size_t get_num_walls() const {
		return this->obstacles.size();
	}

	std::size_t get_num_events() const {
		return this->events.size();
	}

	std::string description() const {
		std::function<std::string(const Difficulty& diff)> difficulty_description = [](const Difficulty& diff) {
			switch (diff) {
			case Difficulty::EASY:
				return "Easy";
			case Difficulty::NORMAL:
				return "Normal";
			case Difficulty::HARD:
				return "Hard";
			case Difficulty::EXPERT:
				return "Expert";
			case Difficulty::EXPERT_PLUS:
				return "Expert Plus";
			}

			return "Unknown Difficulty";
		};

		return "Difficulty: " + difficulty_description(this->difficulty) + "\n" +
			"Notes: " + std::to_string(this->get_num_blocks()) + "\n" +
			"Mines: " + std::to_string(this->get_num_mines()) + "\n" +
			"Walls: " + std::to_string(this->get_num_walls()) + "\n" +
			"Events: " + std::to_string(this->get_num_events()) + "\n";
	}
};

struct BeatSaberInfo {
	std::string songName;
	std::string AuthorName;
	std::string levelAuthorName;
	int beatsPerMinute;
	int songTimeOffset;
	std::string songFilename;
	std::string coverImageFilename;
	std::string environmentName;

	double previewStartTime;
	double previewDuration;
};

struct BeatSaberSong {
	std::vector<BeatSaberLevel> levels;
	BeatSaberInfo info;
};

BeatSaberLevel ParseLevel(const std::string& levelPath, const Difficulty& difficulty, const double& bpm);
bool supportedAudioTypes(const std::filesystem::path& filename);
std::function<BeatSaberSong(const std::string& p)> getBeatSaberFunc();

// Read Beat Saber File
std::string GetBeatSaberLocation(const std::string& root_path);