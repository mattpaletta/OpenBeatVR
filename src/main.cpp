#include <constants/screen_size.hpp>
#include <engine/engine.hpp>

#include "openbeat/openbeat.hpp"

int main() {
#if DEBUG
	std::cout << "Starting Debug" << std::endl;
#endif

	const ScreenSize size { 800, 600 };
	const std::string root_path = "D:/SteamLibrary/steamapps/common/Beat Saber/";

	const auto open_beat_inst = std::make_shared<OpenBeat>(size, root_path);
	Engine e{ open_beat_inst };

	e.run();

	return 0;
}
