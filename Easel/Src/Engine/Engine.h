#pragma once
#include "../Utilities/Template.h"
namespace easel {
	class Window;
	class Renderer;
	//�`��A�I�u�W�F�N�g�Ǘ��A�C�x���g���s
	class Engine : public Singleton<Engine> {
		friend Singleton<Engine>;
	public:
		bool Initialize(int width, int height, const wchar_t* title);
		bool Run();
		void End();
	private:
		Engine() = default;
		~Engine();
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

	private:
		void Update();
		bool Draw();

	private:
		bool end = false;

		Window* window = nullptr;
		Renderer* renderer = nullptr;
	};
}