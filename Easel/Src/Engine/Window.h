#pragma once
#include <Windows.h>
#include "../Utilities/Template.h"

class Window;
namespace easel {
	enum WindowMode {
		WindowMode_default,
	};
	class Window : public Singleton<Window>{
		friend Singleton<Window>;
	public:
		bool Initialize(HINSTANCE hInstance, int width, int height, const wchar_t* windowTitle, WindowMode mode = WindowMode_default);
		void Update();
		bool Destroyed() const;
		void DestroyWindow();
		int GetWidth() const { return width; }
		int GetHeight() const { return height; }
		HWND GetWindowHandle();
	private:
		Window();
		~Window();
		::Window* window;
		int width = 0;
		int height = 0;
	};
} // namespace easel