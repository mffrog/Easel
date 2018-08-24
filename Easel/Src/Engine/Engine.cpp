#include "Engine.h"
#include "Window.h"
#include "../Renderer/Renderer.h"

namespace easel {
	bool Engine::Initialize(int width, int height, const wchar_t * title) {
		window = Window::GetInstance();
		if (!window->Initialize(GetModuleHandle(nullptr), width, height, title)) {
			return false;
		}

		renderer = Renderer::GetInstance();
		if (!renderer->Init(window->GetWindowHandle(), width, height)) {
			return false;
		}
		return true;
	}
	bool Engine::Run() {
		while (!end) {
			window->Update();
			Update();
			if (!Draw()) {
				return false;
			}
			end = end | window->Destroyed();
		}
		return true;
	}
	void Engine::End() {
		end = true;
	}
	Engine::~Engine() {
		Window::Destroy();
		Renderer::Destroy();
	}
	void Engine::Update() {
	}
	bool Engine::Draw() {
		return renderer->Draw();
	}
} // namespace easel