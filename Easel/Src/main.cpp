#include <iostream>
#include "Engine/Engine.h"

using namespace easel;

int main(const int argc, const char* argv[]) {
	const int Width = 960;
	const int Height = 600;
	easel::Engine* engine = easel::Engine::GetInstance();
	if (!engine->Initialize(Width, Height, L"Title")) {
		engine->Destroy();
		return 1;
	}

	engine->Run();

	engine->Destroy();

	return 0;
}