#include "AL/al.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "openvr.h"

#include "openbeat/application.hpp"
#include "openbeat/beatsaber.hpp"

int main() {
	std::cout << "Hello WOrld!" << std::endl;
	openbeat::Application app;
	app.Init();
	
	std::cout << GetBeatSaberLocation().size() << std::endl;

	return 0;
}
