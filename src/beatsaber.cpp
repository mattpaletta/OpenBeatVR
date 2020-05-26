#include "openbeat/beatsaber.hpp"

BeatSaberLevel ParseLevel(const std::string& levelPath, const Difficulty& difficulty, const double& bpm) {
	BeatSaberLevel parsed_level;
	parsed_level.difficulty = difficulty;

	// Read in the info file
	std::ifstream i(levelPath);
	using json = nlohmann::json;
	json level;
	i >> level;

	// Notes
	for (const auto& [_, note] : level["_notes"].items()) {
		const int note_type_raw = note["_type"].get<int>();
		// Anything other than 0 or 1 is a mine.
		const NoteType hand = note_type_raw > 1 ? NoteType::MINE : (NoteType) note_type_raw;

		const CutDirection direction = (CutDirection)note["_cutDirection"].get<int>();
		const int lineIndex = note["_lineIndex"].get<int>();
		const int lineLayer = note["_lineLayer"].get<int>();
		const double time = note["_time"].get<double>();
		const double timeInSeconds = (time / bpm) * 60;

		parsed_level.notes.emplace_back(timeInSeconds, time, lineIndex, lineLayer, hand, direction);
	}

	// Events
	for (const auto& [_, event] : level["_events"].items()) {
		const double time = event["_time"].get<double>();
		const double timeInSeconds = (time / bpm) * 60;
		const int type = event["_type"].get<int>();
		const int value = event["_value"].get<int>();
		parsed_level.events.emplace_back(timeInSeconds, time);
	}

	// Obstacles
	for (const auto& [_, obstacle] : level["_obstacles"].items()) {
		const ObstacleType type = (ObstacleType) obstacle["_type"].get<int>();
		const float duration = obstacle["_duration"].get<float>();
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
	constexpr std::array supportedAudioTypes = { ".wav" };
	for (const auto& supportedType : supportedAudioTypes) {
		if (supportedType == extension) {
			return true;
		}
	}
	return false;
}

std::function<BeatSaberSong(const std::string& p)> getBeatSaberFunc() {
	return [&](const std::string& p) {
		// Find Levels
		BeatSaberSong song;
#if DEBUG
		std::cout << "-- Processing Song: " << p << std::endl;
#endif
		// Find Info
		const auto info_file = p + "/info.dat";
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
			song.info.songFilename = p + "/" + info["_songFilename"].get<std::string>();
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
						std::cerr << "WARNING:Unknown Difficulty: " << raw_difficulty << std::endl;
						continue;
					}
					song.levels.emplace_back(ParseLevel(p + "/" + difficulty_src, parsed_difficulty, song.info.beatsPerMinute));
				}
			}
		}
		return song;
	};
}

// Read Beat Saber File
std::string GetBeatSaberLocation(const std::string& root_path) {
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
		return custom_levels;
	} else {
		return "";
	}
}