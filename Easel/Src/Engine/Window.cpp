#include "WinWindow/Src/Window/window.h"
#include "Window.h"
#include "Engine.h"
namespace easel {
	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
		if (msg == WM_KEYDOWN) {
			if (wp == VK_ESCAPE) {
				Engine::GetInstance()->End();
			}
		}
		if (msg == WM_DESTROY) {
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, msg, wp, lp);
	}	
	bool Window::Initialize(HINSTANCE hInstance, int width, int height, const wchar_t* windowTitle, WindowMode mode) {
		int show = SW_SHOW;
		switch (mode)
		{
		case easel::WindowMode_default:
			show = SW_SHOWDEFAULT;
			break;
		default:
			break;
		}
		this->width = width;
		this->height = height;
		return window->Initialize(hInstance, WndProc, width, height, windowTitle, show);
	}
	void Window::Update() {
		window->UpdateMessage();
	}
	bool Window::Destroyed() const{
		return window->QuitPosted();
	}
	void Window::DestroyWindow() {
		window->Destroy();
	}
	HWND Window::GetWindowHandle() {
		return window->GetWindowHandle();
	}
	Window::Window() {
		window = new ::Window();
	}
	Window::~Window() {
		delete window;
	}



} // namespace easel