#include "AL/al.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "openvr.h"

#include "openbeat/application.hpp"

int main() {
	openbeat::Application app;
	app.Init();

	return 0;
}
