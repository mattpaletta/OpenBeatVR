#pragma once

namespace openbeat {
class Application {
public:
	Application() = default;
	~Application() = default;

	void Init();
};

void Application::Init() {
	vr::EVRInitError eError = vr::VRInitError_None;
	auto m_pHMD = vr::VR_Init( &eError, vr::VRApplication_Scene );
}
}// End Namespace
