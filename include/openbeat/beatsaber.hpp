#pragma once

#include <fstream>
#include <sstream>


#include <engine/engine.hpp>

#include <iostream>
#include <filesystem>

enum class Difficulty { EASY = 0, NORMAL, HARD, EXPERT, EXPERT_PLUS };
enum class NoteType { LEFT = 0, RIGHT };
enum class CutDirection { BOTTOM = 0, TOP, LEFT, RIGHT, TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT, NONDIRECTION };
enum class ObstacleType { WALL = 0, CEILING };

struct Obstacle {
	double timeInSeconds;
	double time;
	int lineIndex;
	double duration;
	ObstacleType type;
	double width;

	Obstacle(const double& _timeInSeconds, const double& _time, const int _lineIndex, const double& _duration, const ObstacleType& _type, const double& _width) : timeInSeconds(_timeInSeconds), time(_time), lineIndex(_lineIndex), duration(_duration), type(_type), width(_width) {};
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

BeatSaberLevel ParseLevel(const std::string& levelPath, const Difficulty& difficulty, const double& bpm) {
	BeatSaberLevel parsed_level;
	
	// Read in the info file
	std::ifstream i(levelPath);
	using json = nlohmann::json;
	json level;
	i >> level;
	
	// Notes
	for (const auto& [_, note] : level["_notes"].items()) {
		const NoteType hand = (NoteType) note["_type"].get<int>();
		const CutDirection direction = (CutDirection) note["_cutDirection"].get<int>();
		const int lineIndex = note["_lineIndex"].get<int>();
		const int lineLayer = note["_lineLayer"].get<int>();
		const double time = note["_time"].get<double>();
		const double timeInSeconds = (time / bpm) * 60;

		parsed_level.notes.emplace_back(timeInSeconds, time, lineIndex, lineLayer, hand, direction);
	}
	// Events
	// for (const auto& [_, event] : level["_events"]) {
	
	// }
	
	// Obstacles
	for (const auto& [_, obstacle] : level["_obstacles"].items()) {
		const ObstacleType type = (ObstacleType) obstacle["_type"].get<int>();
		const double duration = obstacle["_duration"].get<double>();
		const int lineIndex = obstacle["_lineIndex"].get<int>();
		const double time = obstacle["_time"].get<double>();
		const double timeInSeconds = (time / bpm) * 60;
		const double width = obstacle["_width"].get<double>();
		
		parsed_level.obstacles.emplace_back(timeInSeconds, time, lineIndex, duration, type, width);
	}

	return parsed_level;
}

bool supportedAudioTypes(const std::filesystem::path& filename) {
	const std::string extension = filename.extension().string();
	constexpr std::array supportedAudioTypes = {".wav"};
	for (const auto& supportedType : supportedAudioTypes) {
		if (supportedType == extension) {
			return true;
		}
	}
	return false;
}

std::vector<BeatSaberSong> ParseBeatSaberLevels(const std::string& levelsPath) {
	std::vector<BeatSaberSong> out;
	for (const auto& p : std::filesystem::directory_iterator(levelsPath)) {
		// Find Levels
		BeatSaberSong song;
		std::cout << "-- Processing Song: " << p.path() << std::endl;
		
		// Find Info
		const auto info_file = p.path().string() + "/info.dat";
		if (std::filesystem::exists(info_file)) {
			// Read in the info file
			std::ifstream i(info_file);
			using json = nlohmann::json;
			json info;
			i >> info;
			
			// Extract Song Info
			song.info.songName = info["_songName"];
			song.info.AuthorName = info["_songAuthorName"];
			song.info.levelAuthorName = info["_levelAuthorName"];
			song.info.beatsPerMinute = info["_beatsPerMinute"].get<int>();
			song.info.songTimeOffset = info["_songTimeOffset"].get<int>();
			song.info.songFilename = p.path().string() + "/" + info["_songFilename"].get<std::string>();
			song.info.coverImageFilename = info["_coverImageFilename"];
			song.info.environmentName = info["_environmentName"];

			song.info.previewStartTime = info["_previewStartTime"].get<double>();
			song.info.previewDuration = info["_previewDuration"].get<double>();
			
			// Extract each level
			for (const auto& [_, difficultySets] : info["_difficultyBeatmapSets"].items()) {
				for (const auto& [_, difficulty] : difficultySets["_difficultyBeatmaps"].items()) {
					const std::string raw_difficulty = difficulty["_difficulty"];
					const std::string difficulty_src = difficulty["_beatmapFilename"];
					const int rank = difficulty["_difficultyRank"].get<int>();
					const int noteJumpMovementSpeed = difficulty["_noteJumpMovementSpeed"].get<int>();
					const int noteJumpStartBeatOffset = difficulty["_noteJumpStartBeatOffset"].get<int>();
					
					Difficulty parsed_difficulty;
					if (raw_difficulty == "Easy") {
						parsed_difficulty = Difficulty::EASY;
					} else if (raw_difficulty == "Normal") {
						parsed_difficulty = Difficulty::NORMAL;
					} else if (raw_difficulty == "Hard") {
						parsed_difficulty = Difficulty::HARD;
					} else if (raw_difficulty == "Expert") {
						parsed_difficulty = Difficulty::EXPERT;
					} else if (raw_difficulty == "ExpertPlus") {
						parsed_difficulty = Difficulty::EXPERT_PLUS;
					} else {
						std::cout << "Unknown Difficulty: " << raw_difficulty << std::endl;
						continue;
					}
					song.levels.emplace_back( ParseLevel(p.path().string() + "/" + difficulty_src, parsed_difficulty, song.info.beatsPerMinute));
				}
			}
		} else {
			continue;
		}
		
		if (song.levels.size() > 0) {
			out.emplace_back(std::move(song));
		}
	}
	return out;
}

// Read Beat Saber File
std::vector<BeatSaberSong> GetBeatSaberLocation() {
	const std::string root_path = "D:/SteamLibrary/steamapps/common/Beat Saber/";
	const std::string data_path = root_path + "Beat Saber_Data/";
	const std::string custom_levels = data_path + "CustomLevels";

	bool found_root = false;
	if (std::filesystem::exists(root_path)) {
		std::cout << "Found Beat Saber Root" << std::endl;
		found_root = true;
	}
	
	bool found_data = false;
	if (found_root && std::filesystem::exists(data_path)) {
		std::cout << "Found Beat Saber Data Path" << std::endl;
		found_data = true;
	}
	
	bool found_songs = false;
	if (found_data && std::filesystem::exists(custom_levels)) {
		std::cout << "Found Beat Saber Custom Levels" << std::endl;
		found_songs = true;
	}

	if (found_songs) {
		return ParseBeatSaberLevels(custom_levels);
	}

	return {};
}
