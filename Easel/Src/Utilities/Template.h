#pragma once
namespace easel {
	template<typename T>
	class Singleton {
	public:
		static T* GetInstance() {
			if (!instance) {
				Create();
			}
			return instance;
		}

		static void Create() {
			if (!instance) {
				instance = new T();
			}
		}

		static void Destroy() {
			delete instance;
		}
	private:
		static T* instance;
	};

	template<typename T>
	T* Singleton<T>::instance = nullptr;
} // namespace easel