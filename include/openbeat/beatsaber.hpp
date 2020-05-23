#pragma once

#include <engine/engine.hpp>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

enum class Difficulty { EASY, NORMAL, HARD, EXPERT, EXPERT_PLUS };
enum class NoteType { LEFT, RIGHT };
enum class CutDirection { BOTTOM, TOP, LEFT, RIGHT, TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT, NONDIRECTION };
enum class ObstacleType { WALL, CEILING };

struct Obstacle {
	double timeInSeconds;
	double time;
	int lineIndex;
	double duration;
	ObstacleType type;
	double width;

	Obstacle(const double& _timeInSeconds, const double& _time, const int _lineIndex, const double& _duration, const ObstacleType& _type, const double& _width) : timeInSeconds(_timeInSeconds), time(_time), lineIndex(_lineIndex), duration(_duration), type(_type), width(_width) {}
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
};

struct BeatSaberLevel {
	bool is_filled = false;
	Difficulty difficulty;

	std::vector<Note> notes;
	std::vector<Obstacle> obstacles;
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